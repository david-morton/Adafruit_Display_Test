#include <Arduino.h>
#include <functions_display.h>
#include <ptScheduler.h>

ptScheduler ptUpdateDisplayData = ptScheduler(PT_TIME_200MS);

void setup(void) {
  setupDisplay();
}

void loop() {
  if (ptUpdateDisplayData.call()) {
      tftPrintTest();
  }
}
