// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mapf_msgs:msg/SinglePlan.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__STRUCT_HPP_
#define MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'plan'
#include "nav_msgs/msg/detail/path__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mapf_msgs__msg__SinglePlan __attribute__((deprecated))
#else
# define DEPRECATED__mapf_msgs__msg__SinglePlan __declspec(deprecated)
#endif

namespace mapf_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct SinglePlan_
{
  using Type = SinglePlan_<ContainerAllocator>;

  explicit SinglePlan_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : plan(_init)
  {
    (void)_init;
  }

  explicit SinglePlan_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : plan(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _time_step_type =
    std::vector<int32_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int32_t>>;
  _time_step_type time_step;
  using _plan_type =
    nav_msgs::msg::Path_<ContainerAllocator>;
  _plan_type plan;

  // setters for named parameter idiom
  Type & set__time_step(
    const std::vector<int32_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int32_t>> & _arg)
  {
    this->time_step = _arg;
    return *this;
  }
  Type & set__plan(
    const nav_msgs::msg::Path_<ContainerAllocator> & _arg)
  {
    this->plan = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mapf_msgs::msg::SinglePlan_<ContainerAllocator> *;
  using ConstRawPtr =
    const mapf_msgs::msg::SinglePlan_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mapf_msgs::msg::SinglePlan_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mapf_msgs::msg::SinglePlan_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mapf_msgs::msg::SinglePlan_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mapf_msgs::msg::SinglePlan_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mapf_msgs::msg::SinglePlan_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mapf_msgs::msg::SinglePlan_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mapf_msgs::msg::SinglePlan_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mapf_msgs::msg::SinglePlan_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mapf_msgs__msg__SinglePlan
    std::shared_ptr<mapf_msgs::msg::SinglePlan_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mapf_msgs__msg__SinglePlan
    std::shared_ptr<mapf_msgs::msg::SinglePlan_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const SinglePlan_ & other) const
  {
    if (this->time_step != other.time_step) {
      return false;
    }
    if (this->plan != other.plan) {
      return false;
    }
    return true;
  }
  bool operator!=(const SinglePlan_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct SinglePlan_

// alias to use template instance with default allocator
using SinglePlan =
  mapf_msgs::msg::SinglePlan_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mapf_msgs

#endif  // MAPF_MSGS__MSG__DETAIL__SINGLE_PLAN__STRUCT_HPP_
