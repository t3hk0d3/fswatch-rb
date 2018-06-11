#include "fswatch.h"

VALUE fswatch_event_name(enum fsw_event_flag flag) {
  ID rb_idFlagName;

  switch (flag) {
    case NoOp:              rb_idFlagName = rb_intern("no_op"); break;
    case PlatformSpecific:  rb_idFlagName = rb_intern("platform_specific"); break;
    case Created:           rb_idFlagName = rb_intern("created"); break;
    case Updated:           rb_idFlagName = rb_intern("updated"); break;
    case Removed:           rb_idFlagName = rb_intern("removed"); break;
    case Renamed:           rb_idFlagName = rb_intern("renamed"); break;
    case OwnerModified:     rb_idFlagName = rb_intern("owner_modified"); break;
    case AttributeModified: rb_idFlagName = rb_intern("attribute_modified"); break;
    case MovedFrom:         rb_idFlagName = rb_intern("moved_from"); break;
    case MovedTo:           rb_idFlagName = rb_intern("moved_to"); break;
    case IsFile:            rb_idFlagName = rb_intern("is_file"); break;
    case IsDir:             rb_idFlagName = rb_intern("is_dir"); break;
    case IsSymLink:         rb_idFlagName = rb_intern("is_symlink"); break;
    case Link:              rb_idFlagName = rb_intern("link"); break;
    case Overflow:          rb_idFlagName = rb_intern("overflow"); break;
    default:
      rb_idFlagName = rb_intern("unknown");
  }

  return ID2SYM(rb_idFlagName);
}

enum fsw_event_flag fswatch_event_type(VALUE rb_flag) {
  ID rb_idFlagName = rb_to_id(rb_flag);

  if ( rb_idFlagName == rb_intern("no_op") ) {
    return NoOp;
  } else if ( rb_idFlagName == rb_intern("platform_specific") ) {
    return PlatformSpecific;
  } else if ( rb_idFlagName == rb_intern("created") ) {
    return Created;
  } else if ( rb_idFlagName == rb_intern("updated") ) {
    return Updated;
  } else if ( rb_idFlagName == rb_intern("removed") ) {
    return Removed;
  } else if ( rb_idFlagName == rb_intern("renamed") ) {
    return Renamed;
  } else if ( rb_idFlagName == rb_intern("owner_modified") ) {
    return OwnerModified;
  } else if ( rb_idFlagName == rb_intern("attribute_modified") ) {
    return AttributeModified;
  } else if ( rb_idFlagName == rb_intern("moved_from") ) {
    return MovedFrom;
  } else if ( rb_idFlagName == rb_intern("moved_to") ) {
    return MovedTo;
  } else if ( rb_idFlagName == rb_intern("is_file") ) {
    return IsFile;
  } else if ( rb_idFlagName == rb_intern("is_dir") ) {
    return IsDir;
  } else if ( rb_idFlagName == rb_intern("is_symlink") ) {
    return IsSymLink;
  } else if ( rb_idFlagName == rb_intern("link") ) {
    return Link;
  } else if ( rb_idFlagName == rb_intern("overflow") ) {
    return Overflow;
  }

  VALUE rb_str = rb_sym2str(rb_flag);

  rb_raise(rb_eRuntimeError, "Unknown event type '%s'", StringValueCStr(rb_str));

  return NoOp;
}

enum fsw_monitor_type fswatch_monitor_type_by(VALUE name) {
  if (name == Qnil) {
    return system_default_monitor_type;
  }

  VALUE rb_str = StringValue(name);

  if (rb_str_equal(rb_str, rb_str_new_literal("fsevents"))) {
    return fsevents_monitor_type;
  } else if (rb_str_equal(rb_str, rb_str_new_literal("kqueue"))) {
    return kqueue_monitor_type;
  } else if (rb_str_equal(rb_str, rb_str_new_literal("inotify"))) {
    return inotify_monitor_type;
  } else if (rb_str_equal(rb_str, rb_str_new_literal("windows"))) {
    return windows_monitor_type;
  } else if (rb_str_equal(rb_str, rb_str_new_literal("poll"))) {
    return poll_monitor_type;
  } else if (rb_str_equal(rb_str, rb_str_new_literal("fen"))) {
    return fen_monitor_type;
  }

  rb_raise(rb_eRuntimeError, "Unknown monitor type '%s'", StringValueCStr(rb_str));

  return Qnil;
}
