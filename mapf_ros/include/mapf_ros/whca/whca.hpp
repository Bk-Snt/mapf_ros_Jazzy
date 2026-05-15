/*********************************************************************
 * WHCA* algorithm core (stub).
 * Drop this file at: mapf_ros/include/mapf_ros/whca/whca.hpp
 *
 * This is intentionally a stub for Step 3 — just enough to compile.
 * The actual algorithm (reservation table, RRA*, windowed A*,
 * priority outer loop) gets implemented in Step 5.
 *********************************************************************/
#pragma once

namespace mapf {

class WHCA {
 public:
  WHCA(int window_size, int max_replans)
      : window_size_(window_size), max_replans_(max_replans) {}

  // TODO Step 5: implement actual algorithm
  // bool solve(int dimx, int dimy,
  //            const std::unordered_set<Location>& obstacles,
  //            const std::vector<State>& starts,
  //            const std::vector<Location>& goals,
  //            double time_tolerance,
  //            std::vector<PlanResult<State, Action, int>>& solution);

 private:
  int window_size_;
  int max_replans_;
};

}  // namespace mapf
