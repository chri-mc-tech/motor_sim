#include "client_core.h"
#include "client_ffb.h"

int main() {
  start_graphics();
  client_run();
  ffb_close();
}
