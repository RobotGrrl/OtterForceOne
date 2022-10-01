// main changes:
// had to change the file extensions to .cpp
// moved source files to /src
// modified the wrapper (consoleIo.cpp)

#include <Arduino.h>
#include "console.h"

void setup() {
  ConsoleInit();
}

void loop() {
  ConsoleProcess();
}

