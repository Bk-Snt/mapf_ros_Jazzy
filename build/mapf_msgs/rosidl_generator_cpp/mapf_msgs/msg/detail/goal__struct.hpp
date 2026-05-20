// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mapf_msgs:msg/Goal.idl
// generated code does not contain a copyright notice

#ifndef MAPF_MSGS__MSG__DETAIL__GOAL__STRUCT_HPP_
#define MAPF_MSGS__MSG__DETAIL__GOAL__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.hpp"
// Member 'goal'
#include "nav_msgs/msg/detail/path__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mapf_msgs__msg__Goal __attribute__((deprecated))
#else
# define DEPRECATED__mapf_msgs__msg__Goal __declspec(deprecated)
#endif

namespace mapf_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Goal_
{
  using Type = Goal_<ContainerAllocator>;

  explicit Goal_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init),
    goal(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->initial = false;
    }
  }

  explicit Goal_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init),
    goal(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->initial = false;
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _initial_type =
    bool;
  _initial_type initial;
  using _goal_type =
    nav_msgs::msg::Path_<ContainerAllocator>;
  _goal_type goal;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__initial(
    const bool & _arg)
  {
    this->initial = _arg;
    return *this;
  }
  Type & set__goal(
    const nav_msgs::msg::Path_<ContainerAllocator> & _arg)
  {
    this->goal = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mapf_msgs::msg::Goal_<ContainerAllocator> *;
  using ConstRawPtr =
    const mapf_msgs::msg::Goal_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mapf_msgs::msg::Goal_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mapf_msgs::msg::Goal_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mapf_msgs::msg::Goal_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mapf_msgs::msg::Goal_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mapf_msgs::msg::Goal_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mapf_msgs::msg::Goal_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mapf_msgs::msg::Goal_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mapf_msgs::msg::Goal_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mapf_msgs__msg__Goal
    std::shared_ptr<mapf_msgs::msg::Goal_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mapf_msgs__msg__Goal
    std::shared_ptr<mapf_msgs::msg::Goal_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Goal_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->initial != other.initial) {
      return false;
    }
    if (this->goal != other.goal) {
      return false;
    }
    return true;
  }
  bool operator!=(const Goal_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Goal_

// alias to use template instance with default allocator
using Goal =
  mapf_msgs::msg::Goal_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace mapf_msgs

#endif  // MAPF_MSGS__MSG__DETAIL__GOAL__STRUCT_HPP_
