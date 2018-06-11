#include "fswatch.h"

#define REGEXP_IGNORECASE (1 << 0) // 1
#define REGEXP_EXTENDED   (1 << 1) // 2

static void fswatch_add_path(FSW_HANDLE handle, VALUE rb_settings) {
  VALUE rb_path = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("path")));

  VALUE path_arr;

  switch (TYPE(rb_path)) {
    case T_STRING:
      path_arr = rb_ary_new_from_args(1, rb_path);
      break;
    case T_ARRAY:
      path_arr = rb_path;
      break;
    default:
      rb_raise(rb_eTypeError, "path is not valid");
      break;
  }

  long length = RARRAY_LEN(path_arr);

  for ( long i = 0 ; i < length ; i++ ){ 
    VALUE rb_arr_item = rb_ary_entry(path_arr, i);

    char* path_cstr = StringValueCStr(rb_arr_item);


    if(fsw_add_path(handle, path_cstr) != FSW_OK) {
      rb_raise(rb_eRuntimeError, "Unable to monitor specified path - '%s'", path_cstr);
    }
  }
}

static void fswatch_add_properties(FSW_HANDLE handle, VALUE rb_settings) {
  VALUE rb_properties = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("properties")));

  if (rb_properties == Qnil) {
    return;
  }

  Check_Type(rb_properties, T_HASH);

  VALUE rb_property_keys = rb_funcall(rb_properties, rb_intern("keys"), 0);

  long arr_len = RARRAY_LEN(rb_property_keys);
  for ( long i = 0 ; i < arr_len ; i++ ){ 
    VALUE rb_key = rb_ary_entry(rb_property_keys, i);
    VALUE rb_value = rb_hash_fetch(rb_properties, rb_key);

    if (TYPE(rb_key) == T_SYMBOL) {
      rb_key = rb_sym2str(rb_key);
    }

    Check_Type(rb_key, T_STRING);
    Check_Type(rb_value, T_STRING);

    char* key_cstr = StringValueCStr(rb_key);
    char* value_cstr = StringValueCStr(rb_value);


    if(fsw_add_property(handle, key_cstr, value_cstr) != FSW_OK) {
      rb_raise(rb_eRuntimeError, "Unable to add specified property - '%s' = '%s'", key_cstr, value_cstr);
    }
  }
}

static void fswatch_add_event_type_filters(FSW_HANDLE handle, VALUE rb_settings) {
  VALUE rb_event_types = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("event_flags")));

  if (rb_event_types == Qnil) {
    return;
  }

  Check_Type(rb_event_types, T_ARRAY);

  long arr_len = RARRAY_LEN(rb_event_types);
  for ( long i = 0 ; i < arr_len ; i++ ){ 
    VALUE rb_event_type = rb_ary_entry(rb_event_types, i);

    fsw_event_type_filter filter = { .flag = fswatch_event_type(rb_event_type) };

    VALUE rb_str = rb_sym2str(rb_event_type);


    if (fsw_add_event_type_filter(handle, filter) != FSW_OK) {
      rb_raise(rb_eRuntimeError, "Unable to event type filter - '%s' (%d)", StringValueCStr(rb_str), filter.flag);
    }
  }
}

static void fswatch_add_monitor_filters(FSW_HANDLE handle, VALUE rb_settings) {
  VALUE rb_monitor_filters = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("filters")));

  if (rb_monitor_filters == Qnil) {
    return;
  }

  Check_Type(rb_monitor_filters, T_HASH);

  VALUE rb_monitor_keys = rb_funcall(rb_monitor_filters, rb_intern("keys"), 0);

  long arr_len = RARRAY_LEN(rb_monitor_keys);
  for ( long i = 0 ; i < arr_len ; i++ ){ 
    VALUE rb_key = rb_ary_entry(rb_monitor_keys, i);
    VALUE rb_value = rb_hash_fetch(rb_monitor_filters, rb_key);

    Check_Type(rb_key, T_REGEXP);
    Check_Type(rb_value, T_SYMBOL);

    VALUE rb_regexp = RREGEXP_SRC(rb_key);
    int options = rb_reg_options(rb_key);

    enum fsw_filter_type filter_type;

    if (SYM2ID(rb_value) == rb_intern("exclude")) {
      filter_type = filter_exclude;
    } else if (SYM2ID(rb_value) == rb_intern("include")) {
      filter_type = filter_include;
    } else {
      VALUE rb_str = rb_sym2str(rb_value);
      rb_raise(rb_eRuntimeError, "Unknown filter type - '%s'", StringValueCStr(rb_str));
    }

    const fsw_cmonitor_filter filter = {
      .text           = StringValueCStr(rb_regexp),
      .type           = filter_type,
      .case_sensitive = (options & REGEXP_IGNORECASE) == 0,
      .extended       = (options & REGEXP_EXTENDED)   != 0
    };


    if(fsw_add_filter(handle, filter) != FSW_OK) {
      rb_raise(rb_eRuntimeError, "Unable to path filter - '%s' => '%s'", filter.text, StringValueCStr(rb_value));
    }
  }
}

void fswatch_configure(FSW_HANDLE handle, VALUE rb_settings) {
  fswatch_add_path(handle, rb_settings);
  fswatch_add_properties(handle, rb_settings);

  fswatch_add_event_type_filters(handle, rb_settings);
  fswatch_add_monitor_filters(handle, rb_settings);

  VALUE rb_latency = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("latency")));
  if (rb_latency != Qnil) {
    Check_Type(rb_latency, T_FLOAT);


    if (fsw_set_latency(handle, NUM2DBL(rb_latency)) != FSW_OK) {
      rb_raise(rb_eRuntimeError, "Failed to set latency = %0.3f", NUM2DBL(rb_latency));
    }
  }

  VALUE rb_allow_overflow = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("allow_overflow")));
  if (rb_allow_overflow != Qnil) {

    if (fsw_set_allow_overflow(handle, rb_allow_overflow != Qfalse) != FSW_OK) {
      rb_raise(rb_eRuntimeError, "Failed to set overflow mode = %d", rb_allow_overflow != Qfalse);
    }
  }

  VALUE rb_recursive = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("recursive")));
  if (rb_recursive != Qnil) {
    if (fsw_set_recursive(handle, rb_recursive != Qfalse) != FSW_OK) {
      rb_raise(rb_eRuntimeError, "Failed to set recursive mode = %d", rb_recursive != Qfalse);
    }
  }

  VALUE rb_directory_only = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("directory_only")));
  if (rb_directory_only != Qnil) {
    if (fsw_set_directory_only(handle, rb_directory_only != Qfalse) != FSW_OK) {
      rb_raise(rb_eRuntimeError, "Failed to set directory-only mode = %d", rb_directory_only != Qfalse);
    }
  }

  VALUE rb_follow_symlinks = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("follow_symlinks")));
  if (rb_follow_symlinks != Qnil) {
    if (fsw_set_directory_only(handle, rb_follow_symlinks != Qfalse) != FSW_OK) {
      rb_raise(rb_eRuntimeError, "Failed to set follow-symlinks mode = %d", rb_follow_symlinks != Qfalse);
    }
  }

  VALUE rb_verbose = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("debug")));
  if (rb_verbose != Qnil) {
    fsw_set_verbose(rb_verbose != Qfalse);
  }

}