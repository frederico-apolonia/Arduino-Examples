/*
   Arduino BLE Notifier

   This example creates a BLE peripher with a turn on/off LED service which will
   switch every 500 ms and notify the central devices connected. No pins required,
   only an Arduino with BLE support. 

   Frederico Apolónia - 25th Feb 2021
*/
#include <Arduino.h>
#include <ArduinoBLE.h>

BLEService ledNotifierService("00001523-1212-EFDE-1523-785FEABCD123");

BLEBoolCharacteristic ledStatusCharacteristic("00001525-1212-EFDE-1523-785FEABCD123", BLERead | BLENotify);

bool ledStatus = false;

void setup()
{
	Serial.begin(9600);    // initialize serial communication
    while (!Serial);

    // initialize the built-in LED pin to indicate when a central is connected
    pinMode(LED_BUILTIN, OUTPUT); 

    if (!BLE.begin()) {
        Serial.println("Couldn't start BLE");
        while(1);
    }

    BLE.setLocalName("LEDChangeNotifier");
    BLE.setAdvertisedService(ledNotifierService);
    ledNotifierService.addCharacteristic(ledStatusCharacteristic);
    BLE.addService(ledNotifierService);

    ledStatusCharacteristic.writeValue(ledStatus);

    // start advertising
    BLE.advertise();

    Serial.println("BLE Led service active, waiting for central device");
}

void loop()
{
	BLEDevice central = BLE.central();

    if(central) {
        Serial.print("Central connected: ");
        Serial.println(central.address());

        while (central.connected()) {
            waitMs(500);
            updateLedStatus();
        }

        Serial.println("Central disconnected!");
    }
}

void waitMs(int ms) {
    int starting_time = millis();
    while(millis() - starting_time < ms);
}

void updateLedStatus() {
    if (ledStatus) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    ledStatus = !ledStatus;
    ledStatusCharacteristic.writeValue(ledStatus);
}
