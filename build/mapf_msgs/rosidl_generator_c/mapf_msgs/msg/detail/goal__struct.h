// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from mapf_msgs:msg/Goal.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__GOAL__STRUCT_H_
#define MAPF_MSGS__MSG__DETAIL__GOAL__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'goal'
#include "nav_msgs/msg/detail/path__struct.h"

/// Struct defined in msg/Goal in the package mapf_msgs.
typedef struct mapf_msgs__msg__Goal
{
  std_msgs__msg__Header header;
  bool initial;
  nav_msgs__msg__Path goal;
} mapf_msgs__msg__Goal;

// Struct for a sequence of mapf_msgs__msg__Goal.
typedef struct mapf_msgs__msg__Goal__Sequence
{
  mapf_msgs__msg__Goal * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} mapf_msgs__msg__Goal__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // MAPF_MSGS__MSG__DETAIL__GOAL__STRUCT_H_
