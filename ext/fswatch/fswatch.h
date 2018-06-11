#ifndef FSWATCH_H
#define FSWATCH_H

#include <ruby.h>
#include <ruby/st.h>
#include <ruby/encoding.h>
#include <ruby/thread.h>

#include <libfswatch/c/libfswatch.h>

typedef struct FSW_EVENT_CONTEXT {
  const fsw_cevent *const events;
  unsigned int event_num;
  VALUE object;
} FSW_EVENT_CONTEXT;

/* Configuration */
extern void fswatch_configure(FSW_HANDLE handle, VALUE rb_settings);

/* Callbacks */
extern void* fswatch_start_monitor_no_gvl(void* data);
extern void* fswatch_callback_handler_with_gvl(void* data);
extern void fswatch_callback_handler_no_gvl(fsw_cevent const *const events, const unsigned int event_num, void *data);
extern void fswatch_interrupt_monitor_no_gvl(void* data);

/* Enum functions */
extern VALUE fswatch_event_name(enum fsw_event_flag flag);
extern enum fsw_monitor_type fswatch_monitor_type_by(VALUE name);
extern enum fsw_event_flag fswatch_event_type(VALUE rb_flag);

/* Monitor Class */
extern VALUE init_fswatch_monitor();

#endif /* FSWATCH_H */