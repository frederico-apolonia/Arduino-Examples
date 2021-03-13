#include <ArduinoBLE.h>

void setup() { 
    Serial.begin(9600);
    while(!Serial);

    if (!BLE.begin()) {
        Serial.println("Couldn't start BLE.");
        while(1);
    }
    Serial.println("BLE Central - Piripheral write");

    // start scanning for Piripheral
    //BLE.scanForName("Piripheral");
    BLE.scanForUuid("070106ff-d31e-4828-a39c-ab6bf7097fe0");
}

void loop() {
    // check if a peripheral has been discovered
    BLEDevice peripheral = BLE.available();

    if (peripheral) {
        // discovered a peripheral, print out address, local name, and advertised service
        Serial.print("Found ");
        Serial.print(peripheral.address());
        Serial.print(" '");
        Serial.print(peripheral.localName());
        Serial.print("' ");
        Serial.print(peripheral.advertisedServiceUuid());
        Serial.println();

        if (peripheral.localName() != "Piripheral") {
            return;
        }

        // stop scanning
        BLE.stopScan();

        if (writeCharValue(peripheral)) {
            while (true);   
        }

        // peripheral disconnected, start scanning again
        BLE.scanForUuid("070106ff-d31e-4828-a39c-ab6bf7097fe0");
        //BLE.scanForName("Piripheral");
    }
}

bool writeCharValue(BLEDevice peripheral) {
    Serial.println("Connecting...");

    if (peripheral.connect()) {
        Serial.println("Connected to peripheral");
    } else {
        Serial.println("Failed to connect.");
        return false;
    }

    for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
        Serial.println(peripheral.advertisedServiceUuid(i));
    }

    Serial.println(peripheral.advertisedServiceUuid(0));
    for (int i = 0; i < peripheral.service(0).characteristicCount(); i++) {
        BLECharacteristic characteristic = peripheral.service(0).characteristic(i);

        Serial.println(characteristic.uuid());
    }

    BLECharacteristic writeCharacteristic = peripheral.characteristic("070106ff-d31e-4828-a39c-ab6bf7097fe1");

    if (!writeCharacteristic) {
        Serial.println("Device doesn't have write char!");
        return false;
    }

    writeCharacteristic.writeValue((byte) 1);
    Serial.println("Wrote value to char");

    return true;
}