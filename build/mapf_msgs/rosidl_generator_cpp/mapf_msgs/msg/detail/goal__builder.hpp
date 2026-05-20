// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mapf_msgs:msg/Goal.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__GOAL__BUILDER_HPP_
#define MAPF_MSGS__MSG__DETAIL__GOAL__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mapf_msgs/msg/detail/goal__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mapf_msgs
{

namespace msg
{

namespace builder
{

class Init_Goal_goal
{
public:
  explicit Init_Goal_goal(::mapf_msgs::msg::Goal & msg)
  : msg_(msg)
  {}
  ::mapf_msgs::msg::Goal goal(::mapf_msgs::msg::Goal::_goal_type arg)
  {
    msg_.goal = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mapf_msgs::msg::Goal msg_;
};

class Init_Goal_initial
{
public:
  explicit Init_Goal_initial(::mapf_msgs::msg::Goal & msg)
  : msg_(msg)
  {}
  Init_Goal_goal initial(::mapf_msgs::msg::Goal::_initial_type arg)
  {
    msg_.initial = std::move(arg);
    return Init_Goal_goal(msg_);
  }

private:
  ::mapf_msgs::msg::Goal msg_;
};

class Init_Goal_header
{
public:
  Init_Goal_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Goal_initial header(::mapf_msgs::msg::Goal::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_Goal_initial(msg_);
  }

private:
  ::mapf_msgs::msg::Goal msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mapf_msgs::msg::Goal>()
{
  return mapf_msgs::msg::builder::Init_Goal_header();
}

}  // namespace mapf_msgs

#endif  // MAPF_MSGS__MSG__DETAIL__GOAL__BUILDER_HPP_
