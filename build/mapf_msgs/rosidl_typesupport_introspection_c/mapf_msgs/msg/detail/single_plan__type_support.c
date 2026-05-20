// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mapf_msgs:msg/SinglePlan.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mapf_msgs/msg/detail/single_plan__rosidl_typesupport_introspection_c.h"
#include "mapf_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mapf_msgs/msg/detail/single_plan__functions.h"
#include "mapf_msgs/msg/detail/single_plan__struct.h"


// Include directives for member types
// Member `time_step`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `plan`
#include "nav_msgs/msg/path.h"
// Member `plan`
#include "nav_msgs/msg/detail/path__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mapf_msgs__msg__SinglePlan__init(message_memory);
}

void mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_fini_function(void * message_memory)
{
  mapf_msgs__msg__SinglePlan__fini(message_memory);
}

size_t mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__size_function__SinglePlan__time_step(
  const void * untyped_member)
{
  const rosidl_runtime_c__int32__Sequence * member =
    (const rosidl_runtime_c__int32__Sequence *)(untyped_member);
  return member->size;
}

const void * mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__get_const_function__SinglePlan__time_step(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__int32__Sequence * member =
    (const rosidl_runtime_c__int32__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__get_function__SinglePlan__time_step(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__int32__Sequence * member =
    (rosidl_runtime_c__int32__Sequence *)(untyped_member);
  return &member->data[index];
}

void mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__fetch_function__SinglePlan__time_step(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int32_t * item =
    ((const int32_t *)
    mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__get_const_function__SinglePlan__time_step(untyped_member, index));
  int32_t * value =
    (int32_t *)(untyped_value);
  *value = *item;
}

void mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__assign_function__SinglePlan__time_step(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int32_t * item =
    ((int32_t *)
    mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__get_function__SinglePlan__time_step(untyped_member, index));
  const int32_t * value =
    (const int32_t *)(untyped_value);
  *item = *value;
}

bool mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__resize_function__SinglePlan__time_step(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__int32__Sequence * member =
    (rosidl_runtime_c__int32__Sequence *)(untyped_member);
  rosidl_runtime_c__int32__Sequence__fini(member);
  return rosidl_runtime_c__int32__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_message_member_array[2] = {
  {
    "time_step",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mapf_msgs__msg__SinglePlan, time_step),  // bytes offset in struct
    NULL,  // default value
    mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__size_function__SinglePlan__time_step,  // size() function pointer
    mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__get_const_function__SinglePlan__time_step,  // get_const(index) function pointer
    mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__get_function__SinglePlan__time_step,  // get(index) function pointer
    mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__fetch_function__SinglePlan__time_step,  // fetch(index, &value) function pointer
    mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__assign_function__SinglePlan__time_step,  // assign(index, value) function pointer
    mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__resize_function__SinglePlan__time_step  // resize(index) function pointer
  },
  {
    "plan",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mapf_msgs__msg__SinglePlan, plan),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_message_members = {
  "mapf_msgs__msg",  // message namespace
  "SinglePlan",  // message name
  2,  // number of fields
  sizeof(mapf_msgs__msg__SinglePlan),
  mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_message_member_array,  // message members
  mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_init_function,  // function to initialize message memory (memory has to be allocated)
  mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_message_type_support_handle = {
  0,
  &mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mapf_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mapf_msgs, msg, SinglePlan)() {
  mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, msg, Path)();
  if (!mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_message_type_support_handle.typesupport_identifier) {
    mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mapf_msgs__msg__SinglePlan__rosidl_typesupport_introspection_c__SinglePlan_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
