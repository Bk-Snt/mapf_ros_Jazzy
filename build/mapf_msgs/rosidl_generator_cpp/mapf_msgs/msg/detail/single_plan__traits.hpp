// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mapf_msgs:msg/SinglePlan.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__TRAITS_HPP_
#define MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mapf_msgs/msg/detail/single_plan__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'plan'
#include "nav_msgs/msg/detail/path__traits.hpp"

namespace mapf_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const SinglePlan & msg,
  std::ostream & out)
{
  out << "{";
  // member: time_step
  {
    if (msg.time_step.size() == 0) {
      out << "time_step: []";
    } else {
      out << "time_step: [";
      size_t pending_items = msg.time_step.size();
      for (auto item : msg.time_step) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: plan
  {
    out << "plan: ";
    to_flow_style_yaml(msg.plan, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const SinglePlan & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: time_step
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.time_step.size() == 0) {
      out << "time_step: []\n";
    } else {
      out << "time_step:\n";
      for (auto item : msg.time_step) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: plan
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "plan:\n";
    to_block_style_yaml(msg.plan, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const SinglePlan & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace mapf_msgs

namespace rosidl_generator_traits
{

[[deprecated("use mapf_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const mapf_msgs::msg::SinglePlan & msg,
  std::ostream & out, size_t indentation = 0)
{
  mapf_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mapf_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mapf_msgs::msg::SinglePlan & msg)
{
  return mapf_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mapf_msgs::msg::SinglePlan>()
{
  return "mapf_msgs::msg::SinglePlan";
}

template<>
inline const char * name<mapf_msgs::msg::SinglePlan>()
{
  return "mapf_msgs/msg/SinglePlan";
}

template<>
struct has_fixed_size<mapf_msgs::msg::SinglePlan>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mapf_msgs::msg::SinglePlan>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mapf_msgs::msg::SinglePlan>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__TRAITS_HPP_
