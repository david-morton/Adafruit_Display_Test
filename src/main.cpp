#include <Arduino.h>
#include <mcp2515_can.h>
#include <ptScheduler.h>

#define CAN_2515

// Some pin assignments all go here
const int SPI_SS_PIN_NISSAN = 10;
const int CAN_INT_PIN = 2;

mcp2515_can CAN_NISSAN(SPI_SS_PIN_NISSAN);

int setupRetriesMax = 3;

ptScheduler ptCanWriteRequest = ptScheduler(PT_TIME_1S);

void setup() {
  SERIAL_PORT_MONITOR.begin(115200);
  while (!Serial) {
  };

  SERIAL_PORT_MONITOR.println("INFO: Initialising Nissan CAN shield");

  for (int i = 0; i < setupRetriesMax; i++) {
    bool result = CAN_NISSAN.begin(CAN_500KBPS);
    if (result == CAN_OK) {
      SERIAL_PORT_MONITOR.println("\tOK - Nissan CAN shield initialised");
      break;
    } else if (result != CAN_OK) {
      SERIAL_PORT_MONITOR.println(
          "\tERROR - Nissan CAN shield init failed, retrying ...");
      delay(500);
    }
    if (i == setupRetriesMax) {
      SERIAL_PORT_MONITOR.println("\tFATAL - Nissan CAN shield init failed");
    }
  }

  // CAN_NISSAN.init_Mask(0, 0, 0xFFF);
  // CAN_NISSAN.init_Filt(0, 0, 0x7e8);
  // CAN_NISSAN.init_Filt(1, 0, 0x7e8);

  // CAN_NISSAN.init_Mask(1, 0, 0xFFF);
  // CAN_NISSAN.init_Filt(2, 0, 0x7e8);
  // CAN_NISSAN.init_Filt(3, 0, 0x7e8);
  // CAN_NISSAN.init_Filt(4, 0, 0x7e8);
  // CAN_NISSAN.init_Filt(5, 0, 0x7e8);

  // CAN_NISSAN.setMode(1);
}

void writeCanRequest() {
  SERIAL_PORT_MONITOR.println("Sending request...");
  unsigned char canPayloadMisc[8] = {0x02, 0x01, 0x01, 0, 0, 0, 0, 0};
  CAN_NISSAN.sendMsgBuf(0x7e0, 0, 8, canPayloadMisc);
}

// Our main loop
void loop() {
  unsigned char len = 0;
  unsigned char buf[8];

  if (ptCanWriteRequest.call()) {
    writeCanRequest();
  }

  if (CAN_MSGAVAIL == CAN_NISSAN.checkReceive()) {
    CAN_NISSAN.readMsgBuf(&len, buf);

    unsigned long canId = CAN_NISSAN.getCanId();

    if (canId != 0x160 && canId != 0x180 && canId != 0x1f9 && canId != 0x182 &&
        canId != 0x580 && canId != 0x551 && canId != 0x6e2) {
      SERIAL_PORT_MONITOR.print("0x");
      SERIAL_PORT_MONITOR.print(canId, HEX);
      SERIAL_PORT_MONITOR.print("\t");

      for (int i = 0; i < len; i++) { // print the data
        SERIAL_PORT_MONITOR.print(buf[i], HEX);
        SERIAL_PORT_MONITOR.print("\t");
      }
      SERIAL_PORT_MONITOR.println();
    }
  }
}