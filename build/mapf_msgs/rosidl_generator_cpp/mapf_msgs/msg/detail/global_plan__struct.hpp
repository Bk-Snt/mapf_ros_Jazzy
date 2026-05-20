// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mapf_msgs:msg/GlobalPlan.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__STRUCT_HPP_
#define MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'global_plan'
#include "mapf_msgs/msg/detail/single_plan__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mapf_msgs__msg__GlobalPlan __attribute__((deprecated))
#else
# define DEPRECATED__mapf_msgs__msg__GlobalPlan __declspec(deprecated)
#endif

namespace mapf_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct GlobalPlan_
{
  using Type = GlobalPlan_<ContainerAllocator>;

  explicit GlobalPlan_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->makespan = 0l;
    }
  }

  explicit GlobalPlan_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->makespan = 0l;
    }
  }

  // field types and members
  using _makespan_type =
    int32_t;
  _makespan_type makespan;
  using _global_plan_type =
    std::vector<mapf_msgs::msg::SinglePlan_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mapf_msgs::msg::SinglePlan_<ContainerAllocator>>>;
  _global_plan_type global_plan;

  // setters for named parameter idiom
  Type & set__makespan(
    const int32_t & _arg)
  {
    this->makespan = _arg;
    return *this;
  }
  Type & set__global_plan(
    const std::vector<mapf_msgs::msg::SinglePlan_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mapf_msgs::msg::SinglePlan_<ContainerAllocator>>> & _arg)
  {
    this->global_plan = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mapf_msgs::msg::GlobalPlan_<ContainerAllocator> *;
  using ConstRawPtr =
    const mapf_msgs::msg::GlobalPlan_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mapf_msgs::msg::GlobalPlan_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mapf_msgs::msg::GlobalPlan_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mapf_msgs::msg::GlobalPlan_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mapf_msgs::msg::GlobalPlan_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mapf_msgs::msg::GlobalPlan_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mapf_msgs::msg::GlobalPlan_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mapf_msgs::msg::GlobalPlan_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mapf_msgs::msg::GlobalPlan_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mapf_msgs__msg__GlobalPlan
    std::shared_ptr<mapf_msgs::msg::GlobalPlan_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mapf_msgs__msg__GlobalPlan
    std::shared_ptr<mapf_msgs::msg::GlobalPlan_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const GlobalPlan_ & other) const
  {
    if (this->makespan != other.makespan) {
      return false;
    }
    if (this->global_plan != other.global_plan) {
      return false;
    }
    return true;
  }
  bool operator!=(const GlobalPlan_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct GlobalPlan_

// alias to use template instance with default allocator
using GlobalPlan =
  mapf_msgs::msg::GlobalPlan_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mapf_msgs

#endif  // MAPF_MSGS__MSG__DETAIL__GLOBAL_PLAN__STRUCT_HPP_
