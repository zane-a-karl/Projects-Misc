#include <event2/event.h>

int main () {
  event_enable_debug_logging(EVENT_DBG_ALL);
  return 0;
}

