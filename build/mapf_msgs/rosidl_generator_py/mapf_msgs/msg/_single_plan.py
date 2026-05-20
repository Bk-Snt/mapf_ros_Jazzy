# generated from rosidl_generator_py/resource/_idl.py.em
# with input from mapf_msgs:msg/SinglePlan.idl
# generated code does not contain a copyright notice


# Import statements for member types

# Member 'time_step'
import array  # noqa: E402, I100

import builtins  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_SinglePlan(type):
    """Metaclass of message 'SinglePlan'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('mapf_msgs')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'mapf_msgs.msg.SinglePlan')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__single_plan
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__single_plan
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__single_plan
            cls._TYPE_SUPPORT = module.type_support_msg__msg__single_plan
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__single_plan

            from nav_msgs.msg import Path
            if Path.__class__._TYPE_SUPPORT is None:
                Path.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class SinglePlan(metaclass=Metaclass_SinglePlan):
    """Message class 'SinglePlan'."""

    __slots__ = [
        '_time_step',
        '_plan',
    ]

    _fields_and_field_types = {
        'time_step': 'sequence<int32>',
        'plan': 'nav_msgs/Path',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.BasicType('int32')),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['nav_msgs', 'msg'], 'Path'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.time_step = array.array('i', kwargs.get('time_step', []))
        from nav_msgs.msg import Path
        self.plan = kwargs.get('plan', Path())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.time_step != other.time_step:
            return False
        if self.plan != other.plan:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def time_step(self):
        """Message field 'time_step'."""
        return self._time_step

    @time_step.setter
    def time_step(self, value):
        if isinstance(value, array.array):
            assert value.typecode == 'i', \
                "The 'time_step' array.array() must have the type code of 'i'"
            self._time_step = value
            return
        if __debug__:
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 all(isinstance(v, int) for v in value) and
                 all(val >= -2147483648 and val < 2147483648 for val in value)), \
                "The 'time_step' field must be a set or sequence and each value of type 'int' and each integer in [-2147483648, 2147483647]"
        self._time_step = array.array('i', value)

    @builtins.property
    def plan(self):
        """Message field 'plan'."""
        return self._plan

    @plan.setter
    def plan(self, value):
        if __debug__:
            from nav_msgs.msg import Path
            assert \
                isinstance(value, Path), \
                "The 'plan' field must be a sub message of type 'Path'"
        self._plan = value
