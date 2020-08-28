/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)


#include <utility/boards.h>
#include <Arduino.h>
#include <wiring_private.h>


#include <ConfigurableFirmata.h>

#include <FirmataExt.h>
FirmataExt firmataExt;

#include <PulseCounterFirmata.h>
PulseCounterFirmata pulsecounter;

#include <stdio.h>

//#define DEBUG(...) {char buf[128]; sprintf(buf, __VA_ARGS__); SerialUSB.print(buf);}
#define DEBUG(...) {}

void systemResetCallback()
{
    firmataExt.reset();
}

void setup()
{
    Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
    firmataExt.addFeature(pulsecounter);

    Firmata.attach(SYSTEM_RESET, systemResetCallback);

    SerialUSB.begin(115200);  
    Firmata.begin(SerialUSB);
    Firmata.parse(SYSTEM_RESET);  // reset to default config
}

void loop()
{
    pulsecounter.report();

    while (Firmata.available()) {
        Firmata.processInput();
    }
}
