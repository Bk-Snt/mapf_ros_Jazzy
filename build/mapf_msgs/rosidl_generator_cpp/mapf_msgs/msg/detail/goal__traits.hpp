// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from mapf_msgs:msg/Goal.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__GOAL__TRAITS_HPP_
#define MAPF_MSGS__MSG__DETAIL__GOAL__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "mapf_msgs/msg/detail/goal__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"
// Member 'goal'
#include "nav_msgs/msg/detail/path__traits.hpp"

namespace mapf_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Goal & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: initial
  {
    out << "initial: ";
    rosidl_generator_traits::value_to_yaml(msg.initial, out);
    out << ", ";
  }

  // member: goal
  {
    out << "goal: ";
    to_flow_style_yaml(msg.goal, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Goal & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: header
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "header:\n";
    to_block_style_yaml(msg.header, out, indentation + 2);
  }

  // member: initial
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "initial: ";
    rosidl_generator_traits::value_to_yaml(msg.initial, out);
    out << "\n";
  }

  // member: goal
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "goal:\n";
    to_block_style_yaml(msg.goal, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Goal & msg, bool use_flow_style = false)
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
  const mapf_msgs::msg::Goal & msg,
  std::ostream & out, size_t indentation = 0)
{
  mapf_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use mapf_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const mapf_msgs::msg::Goal & msg)
{
  return mapf_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<mapf_msgs::msg::Goal>()
{
  return "mapf_msgs::msg::Goal";
}

template<>
inline const char * name<mapf_msgs::msg::Goal>()
{
  return "mapf_msgs/msg/Goal";
}

template<>
struct has_fixed_size<mapf_msgs::msg::Goal>
  : std::integral_constant<bool, has_fixed_size<nav_msgs::msg::Path>::value && has_fixed_size<std_msgs::msg::Header>::value> {};

template<>
struct has_bounded_size<mapf_msgs::msg::Goal>
  : std::integral_constant<bool, has_bounded_size<nav_msgs::msg::Path>::value && has_bounded_size<std_msgs::msg::Header>::value> {};

template<>
struct is_message<mapf_msgs::msg::Goal>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // MAPF_MSGS__MSG__DETAIL__GOAL__TRAITS_HPP_
