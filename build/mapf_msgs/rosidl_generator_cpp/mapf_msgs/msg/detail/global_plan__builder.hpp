// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mapf_msgs:msg/GlobalPlan.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__BUILDER_HPP_
#define MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mapf_msgs/msg/detail/global_plan__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mapf_msgs
{

namespace msg
{

namespace builder
{

class Init_GlobalPlan_global_plan
{
public:
  explicit Init_GlobalPlan_global_plan(::mapf_msgs::msg::GlobalPlan & msg)
  : msg_(msg)
  {}
  ::mapf_msgs::msg::GlobalPlan global_plan(::mapf_msgs::msg::GlobalPlan::_global_plan_type arg)
  {
    msg_.global_plan = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mapf_msgs::msg::GlobalPlan msg_;
};

class Init_GlobalPlan_makespan
{
public:
  Init_GlobalPlan_makespan()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_GlobalPlan_global_plan makespan(::mapf_msgs::msg::GlobalPlan::_makespan_type arg)
  {
    msg_.makespan = std::move(arg);
    return Init_GlobalPlan_global_plan(msg_);
  }

private:
  ::mapf_msgs::msg::GlobalPlan msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mapf_msgs::msg::GlobalPlan>()
{
  return mapf_msgs::msg::builder::Init_GlobalPlan_makespan();
}

}  // namespace mapf_msgs

#endif  // MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__BUILDER_HPP_
