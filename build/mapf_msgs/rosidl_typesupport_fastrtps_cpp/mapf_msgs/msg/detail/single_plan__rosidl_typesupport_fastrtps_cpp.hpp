// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__rosidl_typesupport_fastrtps_cpp.hpp.em
// with input from mapf_msgs:msg/SinglePlan.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_
#define MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_

#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "mapf_msgs/msg/rosidl_typesupport_fastrtps_cpp__visibility_control.h"
#include "mapf_msgs/msg/detail/single_plan__struct.hpp"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

#include "fastcdr/Cdr.h"

namespace mapf_msgs
{

namespace msg
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_mapf_msgs
cdr_serialize(
  const mapf_msgs::msg::SinglePlan & ros_message,
  eprosima::fastcdr::Cdr & cdr);

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_mapf_msgs
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  mapf_msgs::msg::SinglePlan & ros_message);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_mapf_msgs
get_serialized_size(
  const mapf_msgs::msg::SinglePlan & ros_message,
  size_t current_alignment);

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_mapf_msgs
max_serialized_size_SinglePlan(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

}  // namespace typesupport_fastrtps_cpp

}  // namespace msg

}  // namespace mapf_msgs

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_mapf_msgs
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mapf_msgs, msg, SinglePlan)();

#ifdef __cplusplus
}
#endif

#endif  // MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__ROSIDL_TYPESUPPORT_FASTRTPS_CPP_HPP_
