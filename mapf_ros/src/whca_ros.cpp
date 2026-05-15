/*********************************************************************
 * WHCA* (Windowed Hierarchical Cooperative A*) ROS2 plugin.
 * Drop this file at: mapf_ros/src/whca_ros.cpp
 *
 * All ROS plumbing complete. Algorithm core lives in whca.hpp.
 *********************************************************************/
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

WHCAROS::WHCAROS()
    : costmap_(nullptr), update_obstacle_thread_(nullptr),
      window_size_(8), max_replans_(500), priority_strategy_(0),
      initialized_(false) {}

WHCAROS::WHCAROS(std::string name,
                 std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros,
                 nav2_util::LifecycleNode::SharedPtr node) : WHCAROS() {
  initialize(name, costmap_ros, node);
}

void WHCAROS::initialize(
    std::string name,
    std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros,
    nav2_util::LifecycleNode::SharedPtr node) {
  if (initialized_) return;
  node_ = node;
  clock_ = node_->get_clock();
  logger_ = node_->get_logger();
  costmap_ = costmap_ros->getCostmap();
  global_frame_ = costmap_ros->getGlobalFrameID();

  node_->declare_parameter<int>("whca.window_size", 8);
  node_->declare_parameter<int>("whca.max_replans", 500);
  node_->declare_parameter<int>("whca.priority_strategy", 0);
  node_->get_parameter("whca.window_size", window_size_);
  node_->get_parameter("whca.max_replans", max_replans_);
  node_->get_parameter("whca.priority_strategy", priority_strategy_);

  RCLCPP_INFO(logger_,
              "WHCAROS initialised: window=%d, max_replans=%d, prio=%d",
              window_size_, max_replans_, priority_strategy_);

  algo_ = std::make_unique<WHCA>(window_size_, max_replans_);
  update_obstacle_thread_ =
      new boost::thread(boost::bind(&WHCAROS::updateObstacleThread, this));
  initialized_ = true;
}

void WHCAROS::updateObstacleThread() {
  RCLCPP_INFO(logger_, "WHCA obstacle refresh thread starting...");
  rclcpp::Rate loop_rate(0.5);
  try {
    while (rclcpp::ok()) {
      const int dimx = costmap_->getSizeInCellsX();
      const int dimy = costmap_->getSizeInCellsY();
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
                obstacles_.insert(Location(j, i));
              }
              ++offset;
            }
          }
        }
      }
      loop_rate.sleep();
      boost::this_thread::interruption_point();
    }
  } catch (boost::thread_interrupted const &) {
    RCLCPP_INFO(logger_, "WHCA obstacle thread interrupted, exiting.");
  }
}

bool WHCAROS::makePlan(const nav_msgs::msg::Path &start,
                       const nav_msgs::msg::Path &goal,
                       mapf_msgs::msg::GlobalPlan &plan,
                       double &cost,
                       const double &time_tolerance) {
  if (goal.header.frame_id != global_frame_) {
    RCLCPP_ERROR(logger_, "Goal frame %s != global frame %s",
                 goal.header.frame_id.c_str(), global_frame_.c_str());
    return false;
  }
  if (start.header.frame_id != global_frame_) {
    RCLCPP_ERROR(logger_, "Start frame %s != global frame %s",
                 start.header.frame_id.c_str(), global_frame_.c_str());
    return false;
  }
  if (start.poses.empty() || goal.poses.empty()) {
    RCLCPP_ERROR(logger_, "Empty start or goal vector");
    return false;
  }
  if (start.poses.size() != goal.poses.size()) {
    RCLCPP_ERROR(logger_, "Start/goal size mismatch (%zu vs %zu)",
                 start.poses.size(), goal.poses.size());
    return false;
  }

  std::lock_guard<std::mutex> lock(mtx_obs_update_);

  const int agent_num = static_cast<int>(start.poses.size());
  std::vector<State> startStates;
  std::vector<Location> goals;
  startStates.reserve(agent_num);
  goals.reserve(agent_num);

  for (int i = 0; i < agent_num; ++i) {
    unsigned int sx, sy, gx, gy;
    worldToMap(start.poses[i].pose.position.x,
               start.poses[i].pose.position.y, sx, sy);
    worldToMap(goal.poses[i].pose.position.x,
               goal.poses[i].pose.position.y, gx, gy);
    startStates.emplace_back(State(0, sx, sy));

    if (std::find(goals.begin(), goals.end(), Location(gx, gy)) != goals.end()) {
      RCLCPP_ERROR(logger_, "Duplicate goal location (%u, %u)", gx, gy);
      return false;
    }
    goals.emplace_back(Location(gx, gy));

    if (checkSurroundObstacle(gx, gy)) {
      RCLCPP_ERROR(logger_, "Goal is surrounded by Obstacles: (x, y) = (%u, %u)",
                   gx, gy);
      return false;
    }
    clearCell(gx, gy);
  }

  // ============================================================
  // === ALGORITHM CORE — implemented in whca.hpp (Step 5) ======
  // ============================================================
  std::vector<PlanResult<State, Action, int>> solution;
  Timer timer;

  // TODO Step 5: replace stub with real algorithm call:
  // bool success = algo_->solve(costmap_->getSizeInCellsX(),
  //                             costmap_->getSizeInCellsY(),
  //                             obstacles_, startStates, goals,
  //                             time_tolerance, solution);
  bool success = false;
  RCLCPP_WARN(logger_, "WHCA::solve() not yet implemented — stub returns false");
  timer.stop();

  if (timer.elapsedSeconds() > time_tolerance) {
    RCLCPP_ERROR(logger_, "WHCA planning timed out (%.3fs > %.3fs)",
                 timer.elapsedSeconds(), time_tolerance);
    return false;
  }

  if (success) {
    cost = 0;
    generatePlan(solution, goal, plan, cost);
    RCLCPP_INFO(logger_,
                "WHCA Planning successful! runtime=%.3fs cost=%.2f makespan=%d",
                timer.elapsedSeconds(), cost, plan.makespan);
  } else {
    RCLCPP_ERROR(logger_, "WHCA Planning NOT successful!");
  }
  return success;
}

// ----- Helper methods: same as CBSROS so metrics compare cleanly ---------

void WHCAROS::generatePlan(
    const std::vector<PlanResult<State, Action, int>> &solution,
    const nav_msgs::msg::Path &goal, mapf_msgs::msg::GlobalPlan &plan,
    double &cost) {
  int &makespan = plan.makespan;
  for (const auto &s : solution) cost += s.cost;
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
    single_path.poses.back() = goal.poses[i];
    if (single_path.poses.size() > 1) {
      single_path.poses.erase(single_path.poses.begin());
      single_plan.time_step.erase(single_plan.time_step.begin());
    }
  }

  makespan = 0;
  for (const auto &single_plan : plan.global_plan) {
    plan.makespan = std::max<int>(plan.makespan, single_plan.plan.poses.size());
  }
}

void WHCAROS::worldToMap(const double &wx, const double &wy,
                         unsigned int &mx, unsigned int &my) {
  if (!costmap_->worldToMap(wx, wy, mx, my)) {
    RCLCPP_WARN(logger_,
                "Position (%.3f, %.3f) is off the global costmap.", wx, wy);
  }
}

void WHCAROS::mapToWorld(const unsigned int &mx, const unsigned int &my,
                         double &wx, double &wy) {
  costmap_->mapToWorld(mx, my, wx, wy);
}

void WHCAROS::clearCell(const unsigned int &mx, const unsigned int &my) {
  if (obstacles_.find(Location(mx, my)) != obstacles_.end()) {
    obstacles_.erase(Location(mx, my));
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
    if (mx + x >= 0 && mx + x < (unsigned)dimx &&
        my + y >= 0 && my + y < (unsigned)dimy) {
      check_surround &= checkIsObstacle(mx + x, my + y);
    }
  }
  return check_surround;
}

WHCAROS::~WHCAROS() {
  if (update_obstacle_thread_) {
    update_obstacle_thread_->interrupt();
    update_obstacle_thread_->join();
    delete update_obstacle_thread_;
  }
  costmap_ = nullptr;
  RCLCPP_INFO(logger_, "Exit WHCA planner.");
}

} // namespace mapf
