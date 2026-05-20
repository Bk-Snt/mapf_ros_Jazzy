#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to mapf_msgs__msg__Goal

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Goal {

    // This member is not documented.
    #[allow(missing_docs)]
    pub header: std_msgs::msg::Header,


    // This member is not documented.
    #[allow(missing_docs)]
    pub initial: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goal: nav_msgs::msg::Path,

}



impl Default for Goal {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Goal::default())
  }
}

impl rosidl_runtime_rs::Message for Goal {
  type RmwMsg = super::msg::rmw::Goal;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Owned(msg.header)).into_owned(),
        initial: msg.initial,
        goal: nav_msgs::msg::Path::into_rmw_message(std::borrow::Cow::Owned(msg.goal)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        header: std_msgs::msg::Header::into_rmw_message(std::borrow::Cow::Borrowed(&msg.header)).into_owned(),
      initial: msg.initial,
        goal: nav_msgs::msg::Path::into_rmw_message(std::borrow::Cow::Borrowed(&msg.goal)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      header: std_msgs::msg::Header::from_rmw_message(msg.header),
      initial: msg.initial,
      goal: nav_msgs::msg::Path::from_rmw_message(msg.goal),
    }
  }
}


// Corresponds to mapf_msgs__msg__SinglePlan

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct SinglePlan {

    // This member is not documented.
    #[allow(missing_docs)]
    pub time_step: Vec<i32>,


    // This member is not documented.
    #[allow(missing_docs)]
    pub plan: nav_msgs::msg::Path,

}



impl Default for SinglePlan {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::SinglePlan::default())
  }
}

impl rosidl_runtime_rs::Message for SinglePlan {
  type RmwMsg = super::msg::rmw::SinglePlan;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        time_step: msg.time_step.into(),
        plan: nav_msgs::msg::Path::into_rmw_message(std::borrow::Cow::Owned(msg.plan)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        time_step: msg.time_step.as_slice().into(),
        plan: nav_msgs::msg::Path::into_rmw_message(std::borrow::Cow::Borrowed(&msg.plan)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      time_step: msg.time_step
          .into_iter()
          .collect(),
      plan: nav_msgs::msg::Path::from_rmw_message(msg.plan),
    }
  }
}


// Corresponds to mapf_msgs__msg__GlobalPlan

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct GlobalPlan {

    // This member is not documented.
    #[allow(missing_docs)]
    pub makespan: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub global_plan: Vec<super::msg::SinglePlan>,

}



impl Default for GlobalPlan {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::GlobalPlan::default())
  }
}

impl rosidl_runtime_rs::Message for GlobalPlan {
  type RmwMsg = super::msg::rmw::GlobalPlan;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        makespan: msg.makespan,
        global_plan: msg.global_plan
          .into_iter()
          .map(|elem| super::msg::SinglePlan::into_rmw_message(std::borrow::Cow::Owned(elem)).into_owned())
          .collect(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      makespan: msg.makespan,
        global_plan: msg.global_plan
          .iter()
          .map(|elem| super::msg::SinglePlan::into_rmw_message(std::borrow::Cow::Borrowed(elem)).into_owned())
          .collect(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      makespan: msg.makespan,
      global_plan: msg.global_plan
          .into_iter()
          .map(super::msg::SinglePlan::from_rmw_message)
          .collect(),
    }
  }
}


