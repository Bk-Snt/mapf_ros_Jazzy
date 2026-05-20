// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mapf_msgs:msg/GlobalPlan.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__STRUCT_H_
#define MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'global_plan'
#include "mapf_msgs/msg/detail/single_plan__struct.h"

/// Struct defined in msg/GlobalPlan in the package mapf_msgs.
typedef struct mapf_msgs__msg__GlobalPlan
{
  int32_t makespan;
  mapf_msgs__msg__SinglePlan__Sequence global_plan;
} mapf_msgs__msg__GlobalPlan;

// Struct for a sequence of mapf_msgs__msg__GlobalPlan.
typedef struct mapf_msgs__msg__GlobalPlan__Sequence
{
  mapf_msgs__msg__GlobalPlan * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mapf_msgs__msg__GlobalPlan__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__STRUCT_H_
