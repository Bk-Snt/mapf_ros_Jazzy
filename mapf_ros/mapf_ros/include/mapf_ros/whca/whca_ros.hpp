#pragma once

#ifndef WHCA_ROS_H
#define WHCA_ROS_H

#include <boost/thread.hpp>

#include "rclcpp/rclcpp.hpp"

#include "nav_msgs/msg/path.hpp"

#include "nav2_costmap_2d/costmap_2d.hpp"
#include "nav2_costmap_2d/costmap_2d_ros.hpp"

#include "../utils/timer.hpp"
#include "whca.hpp"
#include "whca_env.hpp"
#include "mapf_ros/mapf_ros.hpp"

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
                mapf_msgs::msg::GlobalPlan &plan, double &cost,
                const double &time_tolerance) override;

  void updateObstacleThread();

  void worldToMap(const double &wx, const double &wy, unsigned int &mx,
                  unsigned int &my);
  void mapToWorld(const unsigned int &mx, const unsigned int &my, double &wx,
                  double &wy);

  void generatePlan(
      const std::vector<PlanResult<WHCAState, WHCAAction, int>> &solution,
      const nav_msgs::msg::Path &goal, mapf_msgs::msg::GlobalPlan &plan,
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

  std::unordered_set<WHCALocation> obstacles_;

  bool initialized_;
  int window_size_;

  rclcpp::Clock::SharedPtr clock_;
  rclcpp::Logger logger_{rclcpp::get_logger("WHCAPlanner")};
  nav2_util::LifecycleNode::SharedPtr node_;
};

}; // namespace mapf

#endif
