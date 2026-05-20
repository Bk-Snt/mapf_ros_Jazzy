// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mapf_msgs:msg/SinglePlan.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__BUILDER_HPP_
#define MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mapf_msgs/msg/detail/single_plan__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mapf_msgs
{

namespace msg
{

namespace builder
{

class Init_SinglePlan_plan
{
public:
  explicit Init_SinglePlan_plan(::mapf_msgs::msg::SinglePlan & msg)
  : msg_(msg)
  {}
  ::mapf_msgs::msg::SinglePlan plan(::mapf_msgs::msg::SinglePlan::_plan_type arg)
  {
    msg_.plan = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mapf_msgs::msg::SinglePlan msg_;
};

class Init_SinglePlan_time_step
{
public:
  Init_SinglePlan_time_step()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_SinglePlan_plan time_step(::mapf_msgs::msg::SinglePlan::_time_step_type arg)
  {
    msg_.time_step = std::move(arg);
    return Init_SinglePlan_plan(msg_);
  }

private:
  ::mapf_msgs::msg::SinglePlan msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mapf_msgs::msg::SinglePlan>()
{
  return mapf_msgs::msg::builder::Init_SinglePlan_time_step();
}

}  // namespace mapf_msgs

#endif  // MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__BUILDER_HPP_
