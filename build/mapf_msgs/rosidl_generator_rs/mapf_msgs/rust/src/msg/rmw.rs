#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "mapf_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mapf_msgs__msg__Goal() -> *const std::ffi::c_void;
}

#[link(name = "mapf_msgs__rosidl_generator_c")]
extern "C" {
    fn mapf_msgs__msg__Goal__init(msg: *mut Goal) -> bool;
    fn mapf_msgs__msg__Goal__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Goal>, size: usize) -> bool;
    fn mapf_msgs__msg__Goal__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Goal>);
    fn mapf_msgs__msg__Goal__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Goal>, out_seq: *mut rosidl_runtime_rs::Sequence<Goal>) -> bool;
}

// Corresponds to mapf_msgs__msg__Goal
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Goal {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::rmw::Header,


    // This member is not documented.
    #[allow(missing_docs)]
    pub initial: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goal: nav_msgs::msg::rmw::Path,

}



impl Default for Goal {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mapf_msgs__msg__Goal__init(&mut msg as *mut _) {
        panic!("Call to mapf_msgs__msg__Goal__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Goal {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mapf_msgs__msg__Goal__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mapf_msgs__msg__Goal__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mapf_msgs__msg__Goal__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Goal {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Goal where Self: Sized {
  const TYPE_NAME: &'static str = "mapf_msgs/msg/Goal";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mapf_msgs__msg__Goal() }
  }
}


#[link(name = "mapf_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mapf_msgs__msg__SinglePlan() -> *const std::ffi::c_void;
}

#[link(name = "mapf_msgs__rosidl_generator_c")]
extern "C" {
    fn mapf_msgs__msg__SinglePlan__init(msg: *mut SinglePlan) -> bool;
    fn mapf_msgs__msg__SinglePlan__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<SinglePlan>, size: usize) -> bool;
    fn mapf_msgs__msg__SinglePlan__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<SinglePlan>);
    fn mapf_msgs__msg__SinglePlan__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<SinglePlan>, out_seq: *mut rosidl_runtime_rs::Sequence<SinglePlan>) -> bool;
}

// Corresponds to mapf_msgs__msg__SinglePlan
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SinglePlan {

    // This member is not documented.
    #[allow(missing_docs)]
    pub time_step: rosidl_runtime_rs::Sequence<i32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub plan: nav_msgs::msg::rmw::Path,

}



impl Default for SinglePlan {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mapf_msgs__msg__SinglePlan__init(&mut msg as *mut _) {
        panic!("Call to mapf_msgs__msg__SinglePlan__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for SinglePlan {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mapf_msgs__msg__SinglePlan__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mapf_msgs__msg__SinglePlan__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mapf_msgs__msg__SinglePlan__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for SinglePlan {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for SinglePlan where Self: Sized {
  const TYPE_NAME: &'static str = "mapf_msgs/msg/SinglePlan";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mapf_msgs__msg__SinglePlan() }
  }
}


#[link(name = "mapf_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__mapf_msgs__msg__GlobalPlan() -> *const std::ffi::c_void;
}

#[link(name = "mapf_msgs__rosidl_generator_c")]
extern "C" {
    fn mapf_msgs__msg__GlobalPlan__init(msg: *mut GlobalPlan) -> bool;
    fn mapf_msgs__msg__GlobalPlan__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<GlobalPlan>, size: usize) -> bool;
    fn mapf_msgs__msg__GlobalPlan__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<GlobalPlan>);
    fn mapf_msgs__msg__GlobalPlan__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<GlobalPlan>, out_seq: *mut rosidl_runtime_rs::Sequence<GlobalPlan>) -> bool;
}

// Corresponds to mapf_msgs__msg__GlobalPlan
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GlobalPlan {

    // This member is not documented.
    #[allow(missing_docs)]
    pub makespan: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub global_plan: rosidl_runtime_rs::Sequence<super::super::msg::rmw::SinglePlan>,

}



impl Default for GlobalPlan {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !mapf_msgs__msg__GlobalPlan__init(&mut msg as *mut _) {
        panic!("Call to mapf_msgs__msg__GlobalPlan__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for GlobalPlan {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mapf_msgs__msg__GlobalPlan__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mapf_msgs__msg__GlobalPlan__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { mapf_msgs__msg__GlobalPlan__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for GlobalPlan {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for GlobalPlan where Self: Sized {
  const TYPE_NAME: &'static str = "mapf_msgs/msg/GlobalPlan";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__mapf_msgs__msg__GlobalPlan() }
  }
}


