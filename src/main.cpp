#include "ncurses_display.h"
#include "system.h"

#include "linux_parser.h"
#include "processor.h"
#include "process.h"


int main() {
  System system;
  NCursesDisplay::Display(system);
}