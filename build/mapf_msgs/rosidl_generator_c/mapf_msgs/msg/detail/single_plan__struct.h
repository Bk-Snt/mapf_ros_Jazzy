// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mapf_msgs:msg/SinglePlan.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__STRUCT_H_
#define MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'time_step'
#include "rosidl_runtime_c/primitives_sequence.h"
// Member 'plan'
#include "nav_msgs/msg/detail/path__struct.h"

/// Struct defined in msg/SinglePlan in the package mapf_msgs.
typedef struct mapf_msgs__msg__SinglePlan
{
  rosidl_runtime_c__int32__Sequence time_step;
  nav_msgs__msg__Path plan;
} mapf_msgs__msg__SinglePlan;

// Struct for a sequence of mapf_msgs__msg__SinglePlan.
typedef struct mapf_msgs__msg__SinglePlan__Sequence
{
  mapf_msgs__msg__SinglePlan * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mapf_msgs__msg__SinglePlan__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__STRUCT_H_
