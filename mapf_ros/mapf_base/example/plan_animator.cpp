/*********************************************************************
 * Plan Animator: Moves robots along WHCA* planned paths by publishing
 * dynamic TF transforms and visualization markers for RViz.
 *********************************************************************/
#include <cmath>
#include <mutex>
#include <string>
#include <vector>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "mapf_msgs/msg/global_plan.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "visualization_msgs/msg/marker_array.hpp"

class PlanAnimator : public rclcpp::Node {
public:
  PlanAnimator() : Node("plan_animator") {
    this->declare_parameter<int>("agent_num", 4);
    this->declare_parameter<double>("step_duration", 1.0);
    this->declare_parameter<std::string>("global_frame_id", "map");

    this->get_parameter("agent_num", agent_num_);
    this->get_parameter("step_duration", step_duration_);
    this->get_parameter("global_frame_id", global_frame_id_);

    agent_frames_.resize(agent_num_);
    initial_positions_.resize(agent_num_);
    for (int i = 0; i < agent_num_; ++i) {
      std::string param_name = "base_frame_id.agent_" + std::to_string(i);
      this->declare_parameter<std::string>(param_name,
                                           "robot_" + std::to_string(i) + "/base_footprint");
      this->get_parameter(param_name, agent_frames_[i]);

      std::string x_param = "init_pos.agent_" + std::to_string(i) + ".x";
      std::string y_param = "init_pos.agent_" + std::to_string(i) + ".y";
      this->declare_parameter<double>(x_param, 0.0);
      this->declare_parameter<double>(y_param, 0.0);
      this->get_parameter(x_param, initial_positions_[i].first);
      this->get_parameter(y_param, initial_positions_[i].second);
    }

    tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
    marker_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>(
        "robot_markers", 10);

    sub_plan_ = this->create_subscription<mapf_msgs::msg::GlobalPlan>(
        "global_plan", 1, std::bind(&PlanAnimator::planCallback, this, std::placeholders::_1));

    timer_ = this->create_wall_timer(std::chrono::milliseconds(33),
                                     std::bind(&PlanAnimator::timerCallback, this));

    RCLCPP_INFO(this->get_logger(), "Plan animator ready. Waiting for global_plan...");
  }

private:
  void planCallback(const mapf_msgs::msg::GlobalPlan::SharedPtr msg) {
    std::lock_guard<std::mutex> lock(plan_mtx_);
    if (animating_) {
      return;
    }
    plan_ = *msg;
    plan_received_ = true;
    animating_ = true;
    animation_start_time_ = this->now();
    RCLCPP_INFO(this->get_logger(), "Animating plan: %d agents, makespan %d",
                static_cast<int>(msg->global_plan.size()), msg->makespan);
  }

  void publishMarkers(const std::vector<std::pair<double, double>> &positions) {
    visualization_msgs::msg::MarkerArray markers;

    // Colors: red, blue, green, purple, orange, cyan, yellow, magenta
    float colors[][3] = {{1.0, 0.0, 0.0}, {0.0, 0.0, 1.0},
                         {0.0, 0.8, 0.0}, {0.6, 0.0, 0.8},
                         {1.0, 0.5, 0.0}, {0.0, 0.8, 0.8},
                         {0.9, 0.9, 0.0}, {0.9, 0.0, 0.5}};

    for (int i = 0; i < agent_num_ && i < static_cast<int>(positions.size()); ++i) {
      // Cylinder body
      visualization_msgs::msg::Marker m;
      m.header.frame_id = global_frame_id_;
      m.header.stamp = this->now();
      m.ns = "robots";
      m.id = i;
      m.type = visualization_msgs::msg::Marker::CYLINDER;
      m.action = visualization_msgs::msg::Marker::ADD;
      m.pose.position.x = positions[i].first;
      m.pose.position.y = positions[i].second;
      m.pose.position.z = 0.3;
      m.pose.orientation.w = 1.0;
      m.scale.x = 0.8;
      m.scale.y = 0.8;
      m.scale.z = 0.6;
      m.color.r = colors[i % 8][0];
      m.color.g = colors[i % 8][1];
      m.color.b = colors[i % 8][2];
      m.color.a = 0.9;
      m.lifetime.sec = 0;
      markers.markers.push_back(m);

      // Text label above
      visualization_msgs::msg::Marker txt;
      txt.header.frame_id = global_frame_id_;
      txt.header.stamp = this->now();
      txt.ns = "labels";
      txt.id = i;
      txt.type = visualization_msgs::msg::Marker::TEXT_VIEW_FACING;
      txt.action = visualization_msgs::msg::Marker::ADD;
      txt.pose.position.x = positions[i].first;
      txt.pose.position.y = positions[i].second;
      txt.pose.position.z = 1.0;
      txt.pose.orientation.w = 1.0;
      txt.scale.z = 0.6;
      txt.color.r = 1.0;
      txt.color.g = 1.0;
      txt.color.b = 1.0;
      txt.color.a = 1.0;
      txt.text = "R" + std::to_string(i);
      txt.lifetime.sec = 0;
      markers.markers.push_back(txt);
    }

    marker_pub_->publish(markers);
  }

  void timerCallback() {
    std::lock_guard<std::mutex> lock(plan_mtx_);
    std::vector<std::pair<double, double>> positions(agent_num_);

    if (!plan_received_) {
      for (int i = 0; i < agent_num_; ++i) {
        positions[i] = initial_positions_[i];

        geometry_msgs::msg::TransformStamped tf;
        tf.header.stamp = this->now();
        tf.header.frame_id = global_frame_id_;
        tf.child_frame_id = agent_frames_[i];
        tf.transform.translation.x = initial_positions_[i].first;
        tf.transform.translation.y = initial_positions_[i].second;
        tf.transform.translation.z = 0.0;
        tf.transform.rotation.w = 1.0;
        tf_broadcaster_->sendTransform(tf);
      }
      publishMarkers(positions);
      return;
    }

    double elapsed = (this->now() - animation_start_time_).seconds();
    double t = elapsed / step_duration_;

    for (int i = 0; i < agent_num_ && i < static_cast<int>(plan_.global_plan.size()); ++i) {
      const auto &poses = plan_.global_plan[i].plan.poses;
      if (poses.empty())
        continue;

      int step = static_cast<int>(t);
      double frac = t - step;

      double x, y;
      if (step >= static_cast<int>(poses.size()) - 1) {
        x = poses.back().pose.position.x;
        y = poses.back().pose.position.y;
      } else {
        double x0 = poses[step].pose.position.x;
        double y0 = poses[step].pose.position.y;
        double x1 = poses[step + 1].pose.position.x;
        double y1 = poses[step + 1].pose.position.y;
        x = x0 + frac * (x1 - x0);
        y = y0 + frac * (y1 - y0);
      }

      positions[i] = {x, y};

      geometry_msgs::msg::TransformStamped tf;
      tf.header.stamp = this->now();
      tf.header.frame_id = global_frame_id_;
      tf.child_frame_id = agent_frames_[i];
      tf.transform.translation.x = x;
      tf.transform.translation.y = y;
      tf.transform.translation.z = 0.0;
      tf.transform.rotation.w = 1.0;
      tf_broadcaster_->sendTransform(tf);
    }

    publishMarkers(positions);

    int current_step = static_cast<int>(t);
    if (current_step != last_logged_step_ && current_step <= plan_.makespan) {
      RCLCPP_INFO(this->get_logger(), "Step %d / %d", current_step, plan_.makespan);
      last_logged_step_ = current_step;
    }
    int max_poses = 0;
    for (const auto &sp : plan_.global_plan) {
      max_poses = std::max(max_poses, static_cast<int>(sp.plan.poses.size()));
    }
    if (current_step >= max_poses) {
      if (animating_) {
        RCLCPP_INFO(this->get_logger(), "Animation complete! All robots reached goals.");
        animating_ = false;
      }
    }
  }

  int agent_num_;
  double step_duration_;
  std::string global_frame_id_;
  std::vector<std::string> agent_frames_;
  std::vector<std::pair<double, double>> initial_positions_;

  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;
  rclcpp::Subscription<mapf_msgs::msg::GlobalPlan>::SharedPtr sub_plan_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::mutex plan_mtx_;
  mapf_msgs::msg::GlobalPlan plan_;
  bool plan_received_{false};
  bool animating_{false};
  rclcpp::Time animation_start_time_;
  int last_logged_step_{-1};
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PlanAnimator>());
  rclcpp::shutdown();
  return 0;
}
