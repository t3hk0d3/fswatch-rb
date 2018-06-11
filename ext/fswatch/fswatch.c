#include <stdint.h>

#include "fswatch.h"

void Init_fswatch() {

  FSW_STATUS error = fsw_init_library();

  init_fswatch_monitor();
}