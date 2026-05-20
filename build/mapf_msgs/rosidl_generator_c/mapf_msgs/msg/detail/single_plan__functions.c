// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from mapf_msgs:msg/SinglePlan.idl
// generated code does not contain a copyright notice
#include "mapf_msgs/msg/detail/single_plan__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `time_step`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `plan`
#include "nav_msgs/msg/detail/path__functions.h"

bool
mapf_msgs__msg__SinglePlan__init(mapf_msgs__msg__SinglePlan * msg)
{
  if (!msg) {
    return false;
  }
  // time_step
  if (!rosidl_runtime_c__int32__Sequence__init(&msg->time_step, 0)) {
    mapf_msgs__msg__SinglePlan__fini(msg);
    return false;
  }
  // plan
  if (!nav_msgs__msg__Path__init(&msg->plan)) {
    mapf_msgs__msg__SinglePlan__fini(msg);
    return false;
  }
  return true;
}

void
mapf_msgs__msg__SinglePlan__fini(mapf_msgs__msg__SinglePlan * msg)
{
  if (!msg) {
    return;
  }
  // time_step
  rosidl_runtime_c__int32__Sequence__fini(&msg->time_step);
  // plan
  nav_msgs__msg__Path__fini(&msg->plan);
}

bool
mapf_msgs__msg__SinglePlan__are_equal(const mapf_msgs__msg__SinglePlan * lhs, const mapf_msgs__msg__SinglePlan * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // time_step
  if (!rosidl_runtime_c__int32__Sequence__are_equal(
      &(lhs->time_step), &(rhs->time_step)))
  {
    return false;
  }
  // plan
  if (!nav_msgs__msg__Path__are_equal(
      &(lhs->plan), &(rhs->plan)))
  {
    return false;
  }
  return true;
}

bool
mapf_msgs__msg__SinglePlan__copy(
  const mapf_msgs__msg__SinglePlan * input,
  mapf_msgs__msg__SinglePlan * output)
{
  if (!input || !output) {
    return false;
  }
  // time_step
  if (!rosidl_runtime_c__int32__Sequence__copy(
      &(input->time_step), &(output->time_step)))
  {
    return false;
  }
  // plan
  if (!nav_msgs__msg__Path__copy(
      &(input->plan), &(output->plan)))
  {
    return false;
  }
  return true;
}

mapf_msgs__msg__SinglePlan *
mapf_msgs__msg__SinglePlan__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mapf_msgs__msg__SinglePlan * msg = (mapf_msgs__msg__SinglePlan *)allocator.allocate(sizeof(mapf_msgs__msg__SinglePlan), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(mapf_msgs__msg__SinglePlan));
  bool success = mapf_msgs__msg__SinglePlan__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
mapf_msgs__msg__SinglePlan__destroy(mapf_msgs__msg__SinglePlan * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    mapf_msgs__msg__SinglePlan__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
mapf_msgs__msg__SinglePlan__Sequence__init(mapf_msgs__msg__SinglePlan__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mapf_msgs__msg__SinglePlan * data = NULL;

  if (size) {
    data = (mapf_msgs__msg__SinglePlan *)allocator.zero_allocate(size, sizeof(mapf_msgs__msg__SinglePlan), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = mapf_msgs__msg__SinglePlan__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        mapf_msgs__msg__SinglePlan__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
mapf_msgs__msg__SinglePlan__Sequence__fini(mapf_msgs__msg__SinglePlan__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      mapf_msgs__msg__SinglePlan__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

mapf_msgs__msg__SinglePlan__Sequence *
mapf_msgs__msg__SinglePlan__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  mapf_msgs__msg__SinglePlan__Sequence * array = (mapf_msgs__msg__SinglePlan__Sequence *)allocator.allocate(sizeof(mapf_msgs__msg__SinglePlan__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = mapf_msgs__msg__SinglePlan__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
mapf_msgs__msg__SinglePlan__Sequence__destroy(mapf_msgs__msg__SinglePlan__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    mapf_msgs__msg__SinglePlan__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
mapf_msgs__msg__SinglePlan__Sequence__are_equal(const mapf_msgs__msg__SinglePlan__Sequence * lhs, const mapf_msgs__msg__SinglePlan__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!mapf_msgs__msg__SinglePlan__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
mapf_msgs__msg__SinglePlan__Sequence__copy(
  const mapf_msgs__msg__SinglePlan__Sequence * input,
  mapf_msgs__msg__SinglePlan__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(mapf_msgs__msg__SinglePlan);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    mapf_msgs__msg__SinglePlan * data =
      (mapf_msgs__msg__SinglePlan *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!mapf_msgs__msg__SinglePlan__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          mapf_msgs__msg__SinglePlan__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!mapf_msgs__msg__SinglePlan__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
