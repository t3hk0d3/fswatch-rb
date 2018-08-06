#include <stdint.h>

#include "fswatch.h"


void* fswatch_start_monitor_no_gvl(void* data) {
  FSW_HANDLE handle = (FSW_HANDLE)data;
  FSW_STATUS status = fsw_start_monitor(handle);

  return (void *)(intptr_t)status; // return integer hidden as pointer
}

void* fswatch_callback_handler_with_gvl(void* data) {
  FSW_EVENT_CONTEXT *context = data;

  VALUE rb_aEvents = rb_ary_new2(context->event_num);

  for (unsigned int i = 0 ; i < context->event_num ; i++) {
    const fsw_cevent event = context->events[i];

    VALUE rb_aEvent = rb_ary_new2(2 + event.flags_num);

    rb_ary_push(rb_aEvent, rb_str_new2(event.path));
    rb_ary_push(rb_aEvent, INT2NUM(event.evt_time));

    for (unsigned int c = 0 ; c < event.flags_num ; c++) {
      rb_ary_push(rb_aEvent, fswatch_event_name(event.flags[c]));
    }

    rb_ary_push(rb_aEvents, rb_aEvent);
  }

  VALUE rb_proc_obj = rb_ivar_get(context->object, rb_intern("callback"));

  if (rb_obj_is_proc(rb_proc_obj) == Qtrue) {
    rb_proc_call(rb_proc_obj, rb_ary_new_from_args(1, rb_aEvents));
  } else {
    rb_method_call(1, &rb_aEvents, rb_proc_obj);
  }


  return NULL;
}

void fswatch_interrupt_monitor_no_gvl(void* data) {
  FSW_HANDLE handle = (FSW_HANDLE)data;
  FSW_STATUS status = fsw_stop_monitor(handle);

  if (status != FSW_OK) {
    rb_raise(rb_eRuntimeError, "Failed to interrupt monitor - err: %d", status);
  }

}

void fswatch_callback_handler_no_gvl(fsw_cevent const *const events,
                                      const unsigned int event_num,
                                      void *data) {
  FSW_EVENT_CONTEXT context = { .events = events, .event_num = event_num, .object = (VALUE)data };

  rb_thread_call_with_gvl(fswatch_callback_handler_with_gvl, &context);
}