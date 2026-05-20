#pragma once

#ifndef WHCA_ENV_H
#define WHCA_ENV_H

#include "../utils/utility.hpp"
#include "../utils/neighbor.hpp"
#include "../utils/planresult.hpp"

using mapf::Neighbor;
using mapf::PlanResult;

struct WHCAState {
  WHCAState(int time, int x, int y) : time(time), x(x), y(y) {}

  bool operator==(const WHCAState &s) const {
    return time == s.time && x == s.x && y == s.y;
  }

  bool equalExceptTime(const WHCAState &s) const {
    return x == s.x && y == s.y;
  }

  friend std::ostream &operator<<(std::ostream &os, const WHCAState &s) {
    return os << s.time << ": (" << s.x << "," << s.y << ")";
  }

  int time;
  int x;
  int y;
};

namespace std {
template <> struct hash<WHCAState> {
  size_t operator()(const WHCAState &s) const {
    size_t seed = 0;
    boost::hash_combine(seed, s.time);
    boost::hash_combine(seed, s.x);
    boost::hash_combine(seed, s.y);
    return seed;
  }
};
} // namespace std

enum class WHCAAction {
  Up,
  Down,
  Left,
  Right,
  Wait,
};

std::ostream &operator<<(std::ostream &os, const WHCAAction &a) {
  switch (a) {
  case WHCAAction::Up:
    os << "Up";
    break;
  case WHCAAction::Down:
    os << "Down";
    break;
  case WHCAAction::Left:
    os << "Left";
    break;
  case WHCAAction::Right:
    os << "Right";
    break;
  case WHCAAction::Wait:
    os << "Wait";
    break;
  }
  return os;
}

struct WHCALocation {
  WHCALocation(int x, int y) : x(x), y(y) {}
  int x;
  int y;

  bool operator==(const WHCALocation &other) const {
    return x == other.x && y == other.y;
  }

  friend std::ostream &operator<<(std::ostream &os, const WHCALocation &c) {
    return os << "(" << c.x << "," << c.y << ")";
  }
};

namespace std {
template <> struct hash<WHCALocation> {
  size_t operator()(const WHCALocation &s) const {
    size_t seed = 0;
    boost::hash_combine(seed, s.x);
    boost::hash_combine(seed, s.y);
    return seed;
  }
};
} // namespace std

// Reservation table: maps (x, y, t) to whether it's reserved
class ReservationTable {
public:
  void reserve(int x, int y, int t) {
    reservations_.insert(makeKey(x, y, t));
  }

  void reserveEdge(int x1, int y1, int x2, int y2, int t) {
    edge_reservations_.insert(makeEdgeKey(x1, y1, x2, y2, t));
  }

  bool isReserved(int x, int y, int t) const {
    return reservations_.find(makeKey(x, y, t)) != reservations_.end();
  }

  bool isEdgeReserved(int x1, int y1, int x2, int y2, int t) const {
    return edge_reservations_.find(makeEdgeKey(x1, y1, x2, y2, t)) !=
           edge_reservations_.end();
  }

  void clear() {
    reservations_.clear();
    edge_reservations_.clear();
  }

  size_t vertexCount() const { return reservations_.size(); }
  size_t edgeCount() const { return edge_reservations_.size(); }

private:
  struct Key {
    int x, y, t;
    bool operator==(const Key &other) const {
      return x == other.x && y == other.y && t == other.t;
    }
  };

  struct KeyHash {
    size_t operator()(const Key &k) const {
      size_t seed = 0;
      boost::hash_combine(seed, k.x);
      boost::hash_combine(seed, k.y);
      boost::hash_combine(seed, k.t);
      return seed;
    }
  };

  struct EdgeKey {
    int x1, y1, x2, y2, t;
    bool operator==(const EdgeKey &other) const {
      return x1 == other.x1 && y1 == other.y1 && x2 == other.x2 &&
             y2 == other.y2 && t == other.t;
    }
  };

  struct EdgeKeyHash {
    size_t operator()(const EdgeKey &k) const {
      size_t seed = 0;
      boost::hash_combine(seed, k.x1);
      boost::hash_combine(seed, k.y1);
      boost::hash_combine(seed, k.x2);
      boost::hash_combine(seed, k.y2);
      boost::hash_combine(seed, k.t);
      return seed;
    }
  };

  static Key makeKey(int x, int y, int t) { return Key{x, y, t}; }
  static EdgeKey makeEdgeKey(int x1, int y1, int x2, int y2, int t) {
    return EdgeKey{x1, y1, x2, y2, t};
  }

  std::unordered_set<Key, KeyHash> reservations_;
  std::unordered_set<EdgeKey, EdgeKeyHash> edge_reservations_;
};

// Environment for windowed cooperative A* search
class WHCAEnvironment {
public:
  WHCAEnvironment(size_t dimx, size_t dimy,
                  const std::unordered_set<WHCALocation> &obstacles,
                  const WHCALocation &goal, int window_size,
                  const ReservationTable &reservation_table)
      : m_dimx(dimx), m_dimy(dimy), m_obstacles(obstacles), m_goal(goal),
        m_window_size(window_size), m_reservation_table(reservation_table),
        m_expanded(0) {}

  int admissibleHeuristic(const WHCAState &s) {
    return std::abs(s.x - m_goal.x) + std::abs(s.y - m_goal.y);
  }

  bool isSolution(const WHCAState &s) {
    // Reached goal or hit window limit
    if (s.x == m_goal.x && s.y == m_goal.y) {
      return true;
    }
    if (s.time >= m_window_size) {
      return true;
    }
    return false;
  }

  void getNeighbors(const WHCAState &s,
                    std::vector<Neighbor<WHCAState, WHCAAction, int>> &neighbors) {
    neighbors.clear();

    if (s.time >= m_window_size) {
      return;
    }

    // Wait
    {
      WHCAState n(s.time + 1, s.x, s.y);
      if (stateValid(n) && transitionValid(s, n)) {
        neighbors.emplace_back(
            Neighbor<WHCAState, WHCAAction, int>(n, WHCAAction::Wait, 1));
      }
    }
    // Left
    {
      WHCAState n(s.time + 1, s.x - 1, s.y);
      if (stateValid(n) && transitionValid(s, n)) {
        neighbors.emplace_back(
            Neighbor<WHCAState, WHCAAction, int>(n, WHCAAction::Left, 1));
      }
    }
    // Right
    {
      WHCAState n(s.time + 1, s.x + 1, s.y);
      if (stateValid(n) && transitionValid(s, n)) {
        neighbors.emplace_back(
            Neighbor<WHCAState, WHCAAction, int>(n, WHCAAction::Right, 1));
      }
    }
    // Up
    {
      WHCAState n(s.time + 1, s.x, s.y + 1);
      if (stateValid(n) && transitionValid(s, n)) {
        neighbors.emplace_back(
            Neighbor<WHCAState, WHCAAction, int>(n, WHCAAction::Up, 1));
      }
    }
    // Down
    {
      WHCAState n(s.time + 1, s.x, s.y - 1);
      if (stateValid(n) && transitionValid(s, n)) {
        neighbors.emplace_back(
            Neighbor<WHCAState, WHCAAction, int>(n, WHCAAction::Down, 1));
      }
    }
  }

  void onExpandNode(const WHCAState & /*s*/, int /*fScore*/, int /*gScore*/) {
    m_expanded++;
  }

  void onDiscover(const WHCAState & /*s*/, int /*fScore*/, int /*gScore*/) {}

  int expanded() const { return m_expanded; }

private:
  bool stateValid(const WHCAState &s) {
    return s.x >= 0 && s.x < static_cast<int>(m_dimx) && s.y >= 0 &&
           s.y < static_cast<int>(m_dimy) &&
           m_obstacles.find(WHCALocation(s.x, s.y)) == m_obstacles.end() &&
           !m_reservation_table.isReserved(s.x, s.y, s.time);
  }

  bool transitionValid(const WHCAState &s1, const WHCAState &s2) {
    // Check edge conflict (swap)
    return !m_reservation_table.isEdgeReserved(s2.x, s2.y, s1.x, s1.y,
                                               s1.time);
  }

  size_t m_dimx;
  size_t m_dimy;
  const std::unordered_set<WHCALocation> &m_obstacles;
  WHCALocation m_goal;
  int m_window_size;
  const ReservationTable &m_reservation_table;
  int m_expanded;
};

#endif
