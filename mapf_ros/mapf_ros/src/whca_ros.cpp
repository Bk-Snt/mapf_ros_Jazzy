#include "rclcpp/rclcpp.hpp"

#include "pluginlib/class_list_macros.hpp"

#include <tf2/utils.hpp>
#include <tf2_ros/transform_listener.hpp>

#include "mapf_msgs/msg/global_plan.hpp"
#include "mapf_msgs/msg/goal.hpp"
#include "mapf_msgs/msg/single_plan.hpp"

#include "mapf_ros/whca/whca_ros.hpp"

PLUGINLIB_EXPORT_CLASS(mapf::WHCAROS, mapf::MAPFROS)

namespace mapf {

WHCAROS::WHCAROS() : costmap_(nullptr), initialized_(false), window_size_(16) {}

WHCAROS::WHCAROS(std::string name,
                 std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros,
                 nav2_util::LifecycleNode::SharedPtr node)
    : costmap_(nullptr), initialized_(false), window_size_(16) {
  initialize(name, costmap_ros, node);
}

void WHCAROS::initialize(
    std::string name,
    std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros,
    nav2_util::LifecycleNode::SharedPtr node) {
  if (!initialized_) {
    node_ = node;
    clock_ = node_->get_clock();
    logger_ = node_->get_logger();

    costmap_ = costmap_ros->getCostmap();
    global_frame_ = costmap_ros->getGlobalFrameID();

    // Read window_size parameter
    node_->declare_parameter("whca.window_size", 16);
    node_->get_parameter("whca.window_size", window_size_);
    RCLCPP_INFO(logger_, "WHCA* window size: %d", window_size_);

    update_obstacle_thread_ =
        new boost::thread(boost::bind(&WHCAROS::updateObstacleThread, this));

    initialized_ = true;
  }
}

void WHCAROS::updateObstacleThread() {
  RCLCPP_INFO(logger_, "WHCA update_obstacle_thread: Updating obstacle state...");
  rclcpp::Rate loop_rate(0.5);

  try {
    while (rclcpp::ok()) {
      int dimx = costmap_->getSizeInCellsX(),
          dimy = costmap_->getSizeInCellsY();
      const unsigned char *costarr = costmap_->getCharMap();

      {
        std::unique_lock<std::mutex> ulock(mtx_obs_update_, std::try_to_lock);
        if (ulock.owns_lock()) {
          obstacles_.clear();

          int offset = 0;
          for (int i = 0; i < dimy; ++i) {
            for (int j = 0; j < dimx; ++j) {
              if (costarr[offset] >=
                  nav2_costmap_2d::INSCRIBED_INFLATED_OBSTACLE) {
                obstacles_.insert(WHCALocation(j, i));
              }
              offset++;
            }
          }
        }
      }

      loop_rate.sleep();
      boost::this_thread::interruption_point();
    }
  } catch (boost::thread_interrupted const &) {
    RCLCPP_INFO(logger_, "whca_planner: Boost interrupt Exit Obstacle.");
  }
}

bool WHCAROS::makePlan(const nav_msgs::msg::Path &start,
                       const nav_msgs::msg::Path &goal,
                       mapf_msgs::msg::GlobalPlan &plan, double &cost,
                       const double &time_tolerance) {
  if (goal.header.frame_id != global_frame_) {
    RCLCPP_ERROR(logger_,
                 "The goal pose passed to this planner must be in the %s frame. "
                 "It is instead in the %s frame.",
                 global_frame_.c_str(), goal.header.frame_id.c_str());
    return false;
  }

  if (start.header.frame_id != global_frame_) {
    RCLCPP_ERROR(logger_,
                 "The start pose passed to this planner must be in the %s frame. "
                 "It is instead in the %s frame.",
                 global_frame_.c_str(), start.header.frame_id.c_str());
    return false;
  }

  if (start.poses.empty() || goal.poses.empty()) {
    RCLCPP_ERROR(logger_, "Start and goal vectors are empty!");
    return false;
  }
  if (start.poses.size() != goal.poses.size()) {
    RCLCPP_ERROR(logger_, "Start and goal vectors are not the same length!");
    return false;
  }

  std::lock_guard<std::mutex> lock(mtx_obs_update_);

  int agent_num = start.poses.size();
  std::vector<WHCAState> startStates;
  std::vector<WHCALocation> goals;

  for (int i = 0; i < agent_num; ++i) {
    unsigned int start_x_i, start_y_i;
    worldToMap(start.poses[i].pose.position.x, start.poses[i].pose.position.y,
               start_x_i, start_y_i);
    startStates.emplace_back(WHCAState(0, start_x_i, start_y_i));

    unsigned int goal_x_i, goal_y_i;
    worldToMap(goal.poses[i].pose.position.x, goal.poses[i].pose.position.y,
               goal_x_i, goal_y_i);

    if (std::find(goals.begin(), goals.end(), WHCALocation(goal_x_i, goal_y_i)) !=
        goals.end()) {
      RCLCPP_ERROR(logger_, "The same goals location exists");
      return false;
    } else {
      goals.emplace_back(WHCALocation(goal_x_i, goal_y_i));
    }

    if (checkSurroundObstacle(goal_x_i, goal_y_i)) {
      RCLCPP_ERROR(logger_,
                   "Goal is surrounded by Obstacles: (x, y) = (%u, %u)",
                   goal_x_i, goal_y_i);
      return false;
    }

    clearCell(goal_x_i, goal_y_i);
  }

  int dimx = costmap_->getSizeInCellsX(), dimy = costmap_->getSizeInCellsY();

  std::vector<PlanResult<WHCAState, WHCAAction, int>> solution;
  WHCA whca(dimx, dimy, obstacles_, goals, window_size_);

  Timer timer;
  bool success = whca.search(startStates, solution, time_tolerance);
  timer.stop();

  if (timer.elapsedSeconds() > time_tolerance) {
    RCLCPP_ERROR(logger_, "Planning time out! Cur time tolerance is %lf",
                 time_tolerance);
    return false;
  }

  if (success) {
    cost = 0;
    generatePlan(solution, goal, plan, cost);

    RCLCPP_DEBUG_STREAM(logger_, "WHCA* planning successful!");
    RCLCPP_DEBUG_STREAM(logger_, "runtime: " << timer.elapsedSeconds());
    RCLCPP_DEBUG_STREAM(logger_, "cost: " << cost);
    RCLCPP_DEBUG_STREAM(logger_, "makespan: " << plan.makespan);
    RCLCPP_DEBUG_STREAM(logger_, "window_size: " << window_size_);
  } else {
    RCLCPP_ERROR(logger_, "WHCA* planning NOT successful!");
  }

  return success;
}

void WHCAROS::generatePlan(
    const std::vector<PlanResult<WHCAState, WHCAAction, int>> &solution,
    const nav_msgs::msg::Path &goal, mapf_msgs::msg::GlobalPlan &plan,
    double &cost) {
  int &makespan = plan.makespan;
  for (const auto &s : solution) {
    cost += s.cost;
  }

  plan.global_plan.resize(solution.size());

  for (size_t i = 0; i < solution.size(); ++i) {
    mapf_msgs::msg::SinglePlan &single_plan = plan.global_plan[i];
    nav_msgs::msg::Path &single_path = single_plan.plan;
    single_path.header.frame_id = global_frame_;
    single_path.header.stamp = clock_->now();

    for (const auto &state : solution[i].states) {
      geometry_msgs::msg::PoseStamped cur_pose;
      cur_pose.header.frame_id = single_path.header.frame_id;
      cur_pose.pose.orientation.w = 1;
      mapToWorld(state.first.x, state.first.y, cur_pose.pose.position.x,
                 cur_pose.pose.position.y);
      single_path.poses.push_back(cur_pose);
      single_plan.time_step.push_back(state.second);
    }

    // Replace end point with goal point if agent reached the goal
    if (!single_path.poses.empty()) {
      const WHCAState &last_state = solution[i].states.back().first;
      WHCALocation goal_loc(0, 0);
      unsigned int gx, gy;
      worldToMap(goal.poses[i].pose.position.x, goal.poses[i].pose.position.y,
                 gx, gy);
      if (last_state.x == static_cast<int>(gx) &&
          last_state.y == static_cast<int>(gy)) {
        single_path.poses.back() = goal.poses[i];
      }
    }

    // Pop start point if it is not an inplace plan
    if (single_path.poses.size() > 1) {
      single_path.poses.erase(single_path.poses.begin());
      single_plan.time_step.erase(single_plan.time_step.begin());
    }
  }

  // Compute makespan
  makespan = 0;
  for (const auto &single_plan : plan.global_plan) {
    plan.makespan = std::max<int>(plan.makespan, single_plan.plan.poses.size());
  }
}

void WHCAROS::worldToMap(const double &wx, const double &wy, unsigned int &mx,
                         unsigned int &my) {
  if (!costmap_->worldToMap(wx, wy, mx, my)) {
    RCLCPP_WARN(logger_,
                "The robot's position (%.3f, %.3f) is off the global costmap. "
                "Planning will fail.",
                wx, wy);
  }
}

void WHCAROS::mapToWorld(const unsigned int &mx, const unsigned int &my,
                         double &wx, double &wy) {
  costmap_->mapToWorld(mx, my, wx, wy);
}

void WHCAROS::clearCell(const unsigned int &mx, const unsigned int &my) {
  if (obstacles_.find(WHCALocation(mx, my)) != obstacles_.end()) {
    obstacles_.erase(WHCALocation(mx, my));
  }
}

bool WHCAROS::checkIsObstacle(const unsigned int &mx, const unsigned int &my) {
  return (costmap_->getCost(mx, my) >=
          nav2_costmap_2d::INSCRIBED_INFLATED_OBSTACLE);
}

bool WHCAROS::checkSurroundObstacle(const unsigned int &mx,
                                    const unsigned int &my) {
  int dimx = costmap_->getSizeInCellsX(), dimy = costmap_->getSizeInCellsY();
  bool check_surround = true;
  std::vector<std::pair<int, int>> step{{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
  for (const auto &s : step) {
    const int &x = s.first, &y = s.second;
    if (static_cast<int>(mx) + x >= 0 && static_cast<int>(mx) + x < dimx &&
        static_cast<int>(my) + y >= 0 && static_cast<int>(my) + y < dimy) {
      check_surround &= checkIsObstacle(mx + x, my + y);
    }
  }
  return check_surround;
}

WHCAROS::~WHCAROS() {
  update_obstacle_thread_->interrupt();
  update_obstacle_thread_->join();
  delete update_obstacle_thread_;

  costmap_ = nullptr;

  RCLCPP_INFO(logger_, "Exit WHCA* planner.");
}

} // namespace mapf
