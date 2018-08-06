#include <stdint.h>

#include "fswatch.h"

void Init_fswatch() {

  FSW_STATUS error = fsw_init_library();

  if (error != FSW_OK) {
    // unable to initialize fswatch library
    rb_raise(rb_eRuntimeError, "Unable to initialize fswatch library!"); 
  }

  init_fswatch_monitor();
}