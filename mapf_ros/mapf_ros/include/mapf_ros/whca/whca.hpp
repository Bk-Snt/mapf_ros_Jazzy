#pragma once

#ifndef WHCA_H
#define WHCA_H

#include <vector>

#include "../utils/neighbor.hpp"
#include "../utils/planresult.hpp"
#include "../utils/timer.hpp"
#include "whca_env.hpp"

#include <boost/heap/d_ary_heap.hpp>
#include <unordered_map>
#include <unordered_set>

#include <rcutils/logging_macros.h>

namespace mapf {

// Windowed A* search for a single agent within the WHCA* framework
class WindowedAStar {
public:
  bool search(const WHCAState &startState, const WHCALocation &goal,
              int window_size, size_t dimx, size_t dimy,
              const std::unordered_set<WHCALocation> &obstacles,
              const ReservationTable &reservation_table,
              PlanResult<WHCAState, WHCAAction, int> &solution, Timer &timer,
              const double &time_tolerance) {
    WHCAEnvironment env(dimx, dimy, obstacles, goal, window_size,
                        reservation_table);

    solution.states.clear();
    solution.actions.clear();
    solution.cost = 0;

    openSet_t openSet;
    std::unordered_map<WHCAState, handle_t> stateToHeap;
    std::unordered_set<WHCAState> closedSet;
    std::unordered_map<WHCAState, std::tuple<WHCAState, WHCAAction, int, int>>
        cameFrom;

    int h = env.admissibleHeuristic(startState);
    auto handle = openSet.push(Node(startState, h, 0));
    stateToHeap.insert(std::make_pair(startState, handle));
    (*handle).handle = handle;

    std::vector<Neighbor<WHCAState, WHCAAction, int>> neighbors;
    neighbors.reserve(10);

    while (!openSet.empty()) {
      timer.stop();
      if (timer.elapsedSeconds() > time_tolerance) {
        return false;
      }

      Node current = openSet.top();
      env.onExpandNode(current.state, current.fScore, current.gScore);

      if (env.isSolution(current.state)) {
        solution.states.clear();
        solution.actions.clear();
        auto iter = cameFrom.find(current.state);
        while (iter != cameFrom.end()) {
          solution.states.push_back(
              std::make_pair(iter->first, std::get<3>(iter->second)));
          solution.actions.push_back(
              std::make_pair(std::get<1>(iter->second),
                            std::get<2>(iter->second)));
          iter = cameFrom.find(std::get<0>(iter->second));
        }
        solution.states.push_back(std::make_pair(startState, 0));
        std::reverse(solution.states.begin(), solution.states.end());
        std::reverse(solution.actions.begin(), solution.actions.end());
        solution.cost = current.gScore;
        solution.fmin = current.fScore;
        return true;
      }

      openSet.pop();
      stateToHeap.erase(current.state);
      closedSet.insert(current.state);

      neighbors.clear();
      env.getNeighbors(current.state, neighbors);
      for (const auto &neighbor : neighbors) {
        if (closedSet.find(neighbor.state) == closedSet.end()) {
          int tentative_gScore = current.gScore + neighbor.cost;
          auto iter = stateToHeap.find(neighbor.state);
          if (iter == stateToHeap.end()) {
            int fScore =
                tentative_gScore + env.admissibleHeuristic(neighbor.state);
            auto handle =
                openSet.push(Node(neighbor.state, fScore, tentative_gScore));
            (*handle).handle = handle;
            stateToHeap.insert(std::make_pair(neighbor.state, handle));
          } else {
            auto handle = iter->second;
            if (tentative_gScore >= (*handle).gScore) {
              continue;
            }
            int delta = (*handle).gScore - tentative_gScore;
            (*handle).gScore = tentative_gScore;
            (*handle).fScore -= delta;
            openSet.increase(handle);
          }

          cameFrom.erase(neighbor.state);
          cameFrom.insert(std::make_pair(
              neighbor.state,
              std::make_tuple(current.state, neighbor.action, neighbor.cost,
                              tentative_gScore)));
        }
      }
    }

    return false;
  }

private:
  struct Node {
    Node(const WHCAState &state, int fScore, int gScore)
        : state(state), fScore(fScore), gScore(gScore) {}

    bool operator<(const Node &other) const {
      if (fScore != other.fScore) {
        return fScore > other.fScore;
      } else {
        return gScore < other.gScore;
      }
    }

    WHCAState state;
    int fScore;
    int gScore;

    typename boost::heap::d_ary_heap<Node, boost::heap::arity<2>,
                                     boost::heap::mutable_<true>>::handle_type
        handle;
  };

  typedef typename boost::heap::d_ary_heap<Node, boost::heap::arity<2>,
                                           boost::heap::mutable_<true>>
      openSet_t;
  typedef typename openSet_t::handle_type handle_t;
};

// WHCA* algorithm: plans for all agents using windowed cooperative A*
class WHCA {
public:
  WHCA(size_t dimx, size_t dimy,
       const std::unordered_set<WHCALocation> &obstacles,
       const std::vector<WHCALocation> &goals, int window_size)
      : m_dimx(dimx), m_dimy(dimy), m_obstacles(obstacles), m_goals(goals),
        m_window_size(window_size) {}

  bool search(const std::vector<WHCAState> &startStates,
              std::vector<PlanResult<WHCAState, WHCAAction, int>> &solution,
              const double &time_tolerance) {
    Timer timer;
    size_t num_agents = startStates.size();
    solution.resize(num_agents);
    ReservationTable reservation_table;

    // Plan for each agent in priority order
    for (size_t i = 0; i < num_agents; ++i) {
      timer.stop();
      if (timer.elapsedSeconds() > time_tolerance) {
        return false;
      }

      WindowedAStar astar;
      bool success = astar.search(startStates[i], m_goals[i], m_window_size,
                                  m_dimx, m_dimy, m_obstacles,
                                  reservation_table, solution[i], timer,
                                  time_tolerance);

      if (!success) {
        return false;
      }

      // Reserve the path in the reservation table for subsequent agents
      RCUTILS_LOG_INFO_NAMED("whca",
          "Agent %zu planned %zu steps. Reserving path:",
          i, solution[i].states.size());
      for (const auto &state_pair : solution[i].states) {
        const WHCAState &s = state_pair.first;
        reservation_table.reserve(s.x, s.y, s.time);
        RCUTILS_LOG_DEBUG_NAMED("whca",
            "  reserve vertex (%d, %d, t=%d)", s.x, s.y, s.time);
      }

      // Reserve edges (for swap detection)
      for (size_t j = 0; j + 1 < solution[i].states.size(); ++j) {
        const WHCAState &s1 = solution[i].states[j].first;
        const WHCAState &s2 = solution[i].states[j + 1].first;
        reservation_table.reserveEdge(s1.x, s1.y, s2.x, s2.y, s1.time);
        RCUTILS_LOG_DEBUG_NAMED("whca",
            "  reserve edge (%d,%d)->(%d,%d) t=%d",
            s1.x, s1.y, s2.x, s2.y, s1.time);
      }

      // If path ends before window, reserve the final position for remaining
      // time steps to prevent other agents from occupying it
      if (!solution[i].states.empty()) {
        const WHCAState &last = solution[i].states.back().first;
        RCUTILS_LOG_INFO_NAMED("whca",
            "  Agent %zu reached goal (%d,%d) at t=%d, holding until t=%d",
            i, last.x, last.y, last.time, m_window_size);
        for (int t = last.time + 1; t <= m_window_size; ++t) {
          reservation_table.reserve(last.x, last.y, t);
        }
      }
      RCUTILS_LOG_INFO_NAMED("whca",
          "  Reservation table size: %zu vertices, %zu edges",
          reservation_table.vertexCount(), reservation_table.edgeCount());
    }

    return true;
  }

private:
  size_t m_dimx;
  size_t m_dimy;
  const std::unordered_set<WHCALocation> &m_obstacles;
  std::vector<WHCALocation> m_goals;
  int m_window_size;
};

} // namespace mapf

#endif
