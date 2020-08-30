#include <utility/boards.h>
#include <Arduino.h>
#include <wiring_private.h>


#include <ConfigurableFirmata.h>

#include <FirmataExt.h>
FirmataExt firmataExt;

#include <PulseCounterFirmata.h>
PulseCounterFirmata pulsecounter;

#include <stdio.h>

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
