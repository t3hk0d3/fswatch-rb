#include "fswatch.h"

static VALUE fswatch_monitor_init(VALUE self, VALUE rb_settings) {
  FSW_HANDLE *handle;
  Data_Get_Struct(self, FSW_HANDLE, handle);

  Check_Type(rb_settings, T_HASH);

  VALUE rb_system_monitor = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("monitor_type")));

  enum fsw_monitor_type monitor_type = fswatch_monitor_type_by(rb_system_monitor);

  *handle = fsw_init_session(monitor_type);

  VALUE rb_callback = rb_hash_lookup(rb_settings, ID2SYM(rb_intern("callback")));

  if(rb_obj_is_proc(rb_callback) != Qtrue) {
    rb_raise(rb_eArgError, "Callback is not specified!");
    return self;
  }

  // Saving callback to instance variable to prevent callback being garbage-collected
  rb_ivar_set(self, rb_intern("callback"), rb_callback);

  fsw_set_callback(*handle, fswatch_callback_handler_no_gvl, (void*)self);

  fswatch_configure(*handle, rb_settings);

  return self;
}

static VALUE fswatch_monitor_start(VALUE self) {
  FSW_HANDLE *handle;
  Data_Get_Struct(self, FSW_HANDLE, handle);

  // fswatch spend most of the time doing nothing
  // so its good idea to remove GIL during wait phase
  // only to temporarily reaquire it for calling ruby callbacks
  FSW_STATUS status = (FSW_STATUS)(intptr_t)
    rb_thread_call_without_gvl(
      fswatch_start_monitor_no_gvl, (void*)(*handle),
      fswatch_interrupt_monitor_no_gvl, (void*)(*handle)
    );

  if (status != FSW_OK) {
    rb_raise(rb_eRuntimeError, "Failed to start monitor - err: %d", status);
  }

  return Qtrue;
}

static VALUE fswatch_monitor_stop(VALUE self) {
  FSW_HANDLE *handle;
  Data_Get_Struct(self, FSW_HANDLE, handle);

  FSW_STATUS status = fsw_stop_monitor(*handle);
  if (status != FSW_OK) {
    rb_raise(rb_eRuntimeError, "Failed to stop monitor - err: %d", status);
  }

  return Qtrue;
}

static VALUE fswatch_monitor_is_running(VALUE self) {
  FSW_HANDLE *handle;
  Data_Get_Struct(self, FSW_HANDLE, handle);

  return fsw_is_running(*handle) ? Qtrue : Qfalse;
}

static void fswatch_monitor_free(void *ptr) {
  FSW_HANDLE handle = *(FSW_HANDLE*)ptr;

  if (handle) {
    FSW_STATUS status = fsw_destroy_session(handle);

    if (status == FSW_ERR_MONITOR_ALREADY_RUNNING) { 
      fsw_stop_monitor(handle);
      fsw_destroy_session(handle); // try to destroy monitor in a blind fate
    } else if (status != FSW_OK) {
      rb_warn("Failed to destroy fswatch session - err:%d", status);
    }
  }

  ruby_xfree(ptr); // free allocated memory for pointer
}


static VALUE fswatch_monitor_alloc(VALUE klass) {
  FSW_HANDLE *handle = ruby_xmalloc(sizeof(FSW_HANDLE));

  *handle = NULL; // IMPORTANT! Initialize handler with NULL

  return Data_Wrap_Struct(klass, NULL, fswatch_monitor_free, handle);
}


extern VALUE init_fswatch_monitor() {
  VALUE rb_mFswatch = rb_define_module("Fswatch");
  VALUE rb_cMonitor = rb_define_class_under(rb_mFswatch, "Monitor", rb_cObject);

  rb_define_alloc_func(rb_cMonitor, fswatch_monitor_alloc);

  rb_define_method(rb_cMonitor, "initialize",     fswatch_monitor_init, 1);

  rb_define_method(rb_cMonitor, "start",  fswatch_monitor_start, 0);
  rb_define_method(rb_cMonitor, "stop",   fswatch_monitor_stop, 0);

  rb_define_method(rb_cMonitor, "running?",  fswatch_monitor_is_running, 0);

  return rb_cMonitor;
}