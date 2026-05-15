/*********************************************************************
 *
 * MIT License
 *
 * WHCA* (Windowed Hierarchical Cooperative A*) ROS2 plugin for mapf_ros.
 * Algorithm reference: Silver, D. (2005) "Cooperative Pathfinding".
 *
 * Drop this file at:
 *   mapf_ros/include/mapf_ros/whca/whca_ros.hpp
 *
 *********************************************************************/
#pragma once

#ifndef WHCA_ROS_H
#define WHCA_ROS_H

#include <boost/thread.hpp>
#include <unordered_set>

#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/path.hpp"
#include "nav2_costmap_2d/costmap_2d.hpp"
#include "nav2_costmap_2d/costmap_2d_ros.hpp"

#include "../utils/timer.hpp"
#include "../cbs/cbs_env.hpp"   // reuse State, Location, Action, Conflict
#include "mapf_ros/mapf_ros.hpp"

// You will create this header next — it owns the algorithm.
// Keeps the plugin file thin and ROS-free at the algorithm layer.
#include "whca.hpp"

namespace mapf {

class WHCAROS : public mapf::MAPFROS {
public:
  WHCAROS();

  WHCAROS(std::string name,
          std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros,
          nav2_util::LifecycleNode::SharedPtr node);

  void initialize(std::string name,
                  std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros,
                  nav2_util::LifecycleNode::SharedPtr node) override;

  bool makePlan(const nav_msgs::msg::Path &start,
                const nav_msgs::msg::Path &goal,
                mapf_msgs::msg::GlobalPlan &plan,
                double &cost,
                const double &time_tolerance) override;

  // Background obstacle refresh — same pattern as CBSROS.
  void updateObstacleThread();

  // Coord helpers — same signatures as CBSROS so we stay consistent.
  void worldToMap(const double &wx, const double &wy,
                  unsigned int &mx, unsigned int &my);
  void mapToWorld(const unsigned int &mx, const unsigned int &my,
                  double &wx, double &wy);

  // Pack a vector of agent solution paths into mapf_msgs/GlobalPlan.
  void generatePlan(const std::vector<PlanResult<State, Action, int>> &solution,
                    const nav_msgs::msg::Path &goal,
                    mapf_msgs::msg::GlobalPlan &plan,
                    double &cost);

  void clearCell(const unsigned int &mx, const unsigned int &my);
  bool checkIsObstacle(const unsigned int &mx, const unsigned int &my);
  bool checkSurroundObstacle(const unsigned int &mx, const unsigned int &my);

  ~WHCAROS();

protected:
  std::mutex mtx_obs_update_;

  nav2_costmap_2d::Costmap2D *costmap_;
  std::string global_frame_;
  boost::thread *update_obstacle_thread_;

  std::unordered_set<Location> obstacles_;

  // WHCA-specific parameters (loaded inside initialize()).
  int    window_size_;        // depth W of each space-time search window
  int    max_replans_;        // safety cap on outer-loop iterations
  int    priority_strategy_;  // 0 = static order, 1 = longest-goal-first, ...

  // Algorithm core (no ROS includes inside whca.hpp).
  std::unique_ptr<WHCA> algo_;

  bool initialized_;

  rclcpp::Clock::SharedPtr clock_;
  rclcpp::Logger logger_{rclcpp::get_logger("WHCAPlanner")};
  nav2_util::LifecycleNode::SharedPtr node_;
};

}; // namespace mapf

#endif
