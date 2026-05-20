// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mapf_msgs:msg/GlobalPlan.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__TRAITS_HPP_
#define MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mapf_msgs/msg/detail/global_plan__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'global_plan'
#include "mapf_msgs/msg/detail/single_plan__traits.hpp"

namespace mapf_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const GlobalPlan & msg,
  std::ostream & out)
{
  out << "{";
  // member: makespan
  {
    out << "makespan: ";
    rosidl_generator_traits::value_to_yaml(msg.makespan, out);
    out << ", ";
  }

  // member: global_plan
  {
    if (msg.global_plan.size() == 0) {
      out << "global_plan: []";
    } else {
      out << "global_plan: [";
      size_t pending_items = msg.global_plan.size();
      for (auto item : msg.global_plan) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const GlobalPlan & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: makespan
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "makespan: ";
    rosidl_generator_traits::value_to_yaml(msg.makespan, out);
    out << "\n";
  }

  // member: global_plan
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.global_plan.size() == 0) {
      out << "global_plan: []\n";
    } else {
      out << "global_plan:\n";
      for (auto item : msg.global_plan) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const GlobalPlan & msg, bool use_flow_style = false)
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
  const mapf_msgs::msg::GlobalPlan & msg,
  std::ostream & out, size_t indentation = 0)
{
  mapf_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mapf_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mapf_msgs::msg::GlobalPlan & msg)
{
  return mapf_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mapf_msgs::msg::GlobalPlan>()
{
  return "mapf_msgs::msg::GlobalPlan";
}

template<>
inline const char * name<mapf_msgs::msg::GlobalPlan>()
{
  return "mapf_msgs/msg/GlobalPlan";
}

template<>
struct has_fixed_size<mapf_msgs::msg::GlobalPlan>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<mapf_msgs::msg::GlobalPlan>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<mapf_msgs::msg::GlobalPlan>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__TRAITS_HPP_
