// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from mapf_msgs:msg/GlobalPlan.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "mapf_msgs/msg/detail/global_plan__rosidl_typesupport_introspection_c.h"
#include "mapf_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "mapf_msgs/msg/detail/global_plan__functions.h"
#include "mapf_msgs/msg/detail/global_plan__struct.h"


// Include directives for member types
// Member `global_plan`
#include "mapf_msgs/msg/single_plan.h"
// Member `global_plan`
#include "mapf_msgs/msg/detail/single_plan__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  mapf_msgs__msg__GlobalPlan__init(message_memory);
}

void mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_fini_function(void * message_memory)
{
  mapf_msgs__msg__GlobalPlan__fini(message_memory);
}

size_t mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__size_function__GlobalPlan__global_plan(
  const void * untyped_member)
{
  const mapf_msgs__msg__SinglePlan__Sequence * member =
    (const mapf_msgs__msg__SinglePlan__Sequence *)(untyped_member);
  return member->size;
}

const void * mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__get_const_function__GlobalPlan__global_plan(
  const void * untyped_member, size_t index)
{
  const mapf_msgs__msg__SinglePlan__Sequence * member =
    (const mapf_msgs__msg__SinglePlan__Sequence *)(untyped_member);
  return &member->data[index];
}

void * mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__get_function__GlobalPlan__global_plan(
  void * untyped_member, size_t index)
{
  mapf_msgs__msg__SinglePlan__Sequence * member =
    (mapf_msgs__msg__SinglePlan__Sequence *)(untyped_member);
  return &member->data[index];
}

void mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__fetch_function__GlobalPlan__global_plan(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const mapf_msgs__msg__SinglePlan * item =
    ((const mapf_msgs__msg__SinglePlan *)
    mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__get_const_function__GlobalPlan__global_plan(untyped_member, index));
  mapf_msgs__msg__SinglePlan * value =
    (mapf_msgs__msg__SinglePlan *)(untyped_value);
  *value = *item;
}

void mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__assign_function__GlobalPlan__global_plan(
  void * untyped_member, size_t index, const void * untyped_value)
{
  mapf_msgs__msg__SinglePlan * item =
    ((mapf_msgs__msg__SinglePlan *)
    mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__get_function__GlobalPlan__global_plan(untyped_member, index));
  const mapf_msgs__msg__SinglePlan * value =
    (const mapf_msgs__msg__SinglePlan *)(untyped_value);
  *item = *value;
}

bool mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__resize_function__GlobalPlan__global_plan(
  void * untyped_member, size_t size)
{
  mapf_msgs__msg__SinglePlan__Sequence * member =
    (mapf_msgs__msg__SinglePlan__Sequence *)(untyped_member);
  mapf_msgs__msg__SinglePlan__Sequence__fini(member);
  return mapf_msgs__msg__SinglePlan__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_message_member_array[2] = {
  {
    "makespan",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mapf_msgs__msg__GlobalPlan, makespan),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "global_plan",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(mapf_msgs__msg__GlobalPlan, global_plan),  // bytes offset in struct
    NULL,  // default value
    mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__size_function__GlobalPlan__global_plan,  // size() function pointer
    mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__get_const_function__GlobalPlan__global_plan,  // get_const(index) function pointer
    mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__get_function__GlobalPlan__global_plan,  // get(index) function pointer
    mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__fetch_function__GlobalPlan__global_plan,  // fetch(index, &value) function pointer
    mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__assign_function__GlobalPlan__global_plan,  // assign(index, value) function pointer
    mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__resize_function__GlobalPlan__global_plan  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_message_members = {
  "mapf_msgs__msg",  // message namespace
  "GlobalPlan",  // message name
  2,  // number of fields
  sizeof(mapf_msgs__msg__GlobalPlan),
  mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_message_member_array,  // message members
  mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_init_function,  // function to initialize message memory (memory has to be allocated)
  mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_message_type_support_handle = {
  0,
  &mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_mapf_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mapf_msgs, msg, GlobalPlan)() {
  mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, mapf_msgs, msg, SinglePlan)();
  if (!mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_message_type_support_handle.typesupport_identifier) {
    mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &mapf_msgs__msg__GlobalPlan__rosidl_typesupport_introspection_c__GlobalPlan_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
