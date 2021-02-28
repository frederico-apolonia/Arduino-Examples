/* 
    Arduino MAC and RSSI serializer

    This example creates a BLE peripheral that will serialize and send 32 BLE devices
    readings with 10 RSSI values after receiving a reading on the characteristic. No 
    pins required, only Arduino with BLE support.

    Frederico Apolónia - 26th Feb 2021
 */
#include <Arduino.h>

#include <ArduinoJson.h>
#include <ArduinoBLE.h>

BLEService rssiNotifierService("3509B2C6-2F88-4D48-9B13-FB0092951128");

/* Max size of this characteristic is 512 bytes, but the size isn't fixed.
 * Per MAC rssi, the vector is as follows:
 * [<MACADDRESS[6 bytes]>, <[<RSSI[1 byte ea]>]>]
 * This gives 16 bytes per device and a max of 32 devices per message
 */
BLECharacteristic rssiValuesCharacteristic("3509B2C7-2F88-4D48-9B13-FB0092951128", BLERead | BLENotify, 512, false);

DynamicJsonDocument rssisJson(8192);

const char unparsedRssiJson[] = "{\"9D:FF:70:45:D3:EC\":[-33,-21,-62,-46,-54,-45],\"AD:DB:1B:3B:4A:EE\":[-91,-35,-98,-20,-37],\"C8:4F:C0:45:D4:44\":[-21,-78,-51,-29,-36,-33,-57,-92,-80,-94],\"49:91:C8:E6:5B:26\":[-96,-50,-42,-21,-66,-78,-26,-55,-70],\"65:2A:00:9C:8D:B4\":[-45,-30,-91,-44,-97,-27,-87,-36,-53,-46],\"DF:9B:3E:F8:79:93\":[-78,-90,-73,-43,-91,-64,-59,-57,-51],\"17:A6:2C:C8:1E:31\":[-90,-84,-32],\"FC:D7:98:C7:85:2B\":[-36,-45,-42,-59,-94,-95,-40,-89],\"31:3A:AD:EB:48:17\":[-88,-25,-83,-46,-40],\"4E:F5:C2:7B:E4:38\":[-33,-69,-44,-56],\"DF:F5:41:08:24:2D\":[-82,-60,-92,-93,-74,-30,-58],\"10:85:8A:CC:8C:24\":[-34,-50,-28,-100,-87],\"E9:0D:D3:5A:24:F6\":[-78,-71,-48],\"50:E7:41:45:1D:AA\":[-47,-78,-73,-25],\"77:6F:38:31:13:EF\":[-64,-82,-100],\"63:98:79:FC:9D:4D\":[-95,-23,-72],\"E8:20:5C:AC:30:B6\":[-32,-89,-78,-66,-84,-48,-35,-79],\"9F:8A:26:99:A3:11\":[-34,-21,-34,-63,-38,-58,-47,-95,-29,-73],\"C0:E5:E2:2C:A4:C0\":[-44,-99,-30,-43,-95,-82],\"06:C4:78:A0:72:A9\":[-100,-71,-25,-42],\"94:23:74:43:42:82\":[-73,-92,-29,-85,-27,-51],\"4A:FF:EB:FE:2B:29\":[-94,-73,-62,-54,-33,-94,-95],\"3D:9A:F3:48:76:BE\":[-21,-97,-38,-65,-77,-26,-40],\"A0:1F:29:17:30:61\":[-56,-69,-75,-60,-52,-97,-45,-76],\"C5:0E:02:74:F0:EF\":[-60,-68,-70,-85,-91,-99,-95,-81,-44],\"32:7B:93:93:3B:25\":[-38,-75,-87,-34,-100],\"44:C8:42:90:79:E8\":[-29,-95,-73,-31,-55,-54,-60,-31],\"C5:CB:5A:A9:D2:46\":[-22,-52,-60,-61,-67,-88],\"5D:98:8C:DD:0A:54\":[-90,-40,-59,-30],\"CE:54:83:C4:98:FE\":[-63,-100,-63,-71,-91,-90,-28],\"2A:7F:CB:51:9F:91\":[-70,-82,-47,-52,-53,-62,-88,-91],\"B6:EB:03:FD:24:1A\":[-49,-96,-50]}";

const int MAC_ADDRESS_SIZE_BYTES = 6;
const int MAC_ADDRESS_BASE = 16;
const int NULL_RSSI = 255;

void setup() {
	Serial.begin(9600);
    while(!Serial);

    if (!BLE.begin()) {
        Serial.println("Couldn't start BLE");
        while(1);
    }

    if (!populateJson()) {
        while(1);
    }

    BLE.setLocalName("MACSerializer");
    BLE.setAdvertisedService(rssiNotifierService);
    rssiNotifierService.addCharacteristic(rssiValuesCharacteristic);
    BLE.addService(rssiNotifierService);

    rssiValuesCharacteristic.setEventHandler(BLESubscribed, rssiCharacteristicSubscribed);

    BLE.advertise();

    Serial.println("BLE RSSI service active, waiting for connections");
}

bool populateJson() {
    DeserializationError error = deserializeJson(rssisJson, unparsedRssiJson);
    if (error) {
        Serial.println(F("Failed to deserialize json:"));
        Serial.println(error.f_str());
        return false;
    }
    return true;
}

/* Convert string seperated by sep to bytes and save it on buffer.
 * Offset indicates the first position of buffer to write the data; 
 * requires buffer + maxBytes < buffer length
 */
void parseBytes(const char* str, char sep, byte* buffer, int maxBytes, int base, int offset=0) {
    for (int i = offset; i < offset + maxBytes; i++) {
        buffer[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}

/* Handler for when the characteristic is subscribed. When RSSI Values is subscribed, it's
 * expected that the arduino will now serialize and send all the device values via
 * notification to subscribed device.
 */
void rssiCharacteristicSubscribed(BLEDevice central, BLECharacteristic characteristic) {
    Serial.println("New central device subscribed to the characteristic.");
    Serial.println("Going to prepare and send the values");

    JsonObject rssisObject = rssisJson.as<JsonObject>();
    int numDevices = rssisObject.size();
    const int bufferSize = numDevices * 16;
    // create buffer
    byte buffer[bufferSize];
    int bufferPos = 0;

    int startingRssiBytes;
    for (JsonPair scannedDevice : rssisObject) {
        JsonArray rssis = rssisJson[scannedDevice.key().c_str()];

        const char* macAddress = scannedDevice.key().c_str();
        /* write MAC Address on buffer */
        parseBytes(macAddress, ':', buffer, MAC_ADDRESS_SIZE_BYTES, MAC_ADDRESS_BASE, bufferPos);
        bufferPos += MAC_ADDRESS_SIZE_BYTES;

        startingRssiBytes = bufferPos;
        /* write RSSIs from MAC Address on buffer */
        for(JsonVariant v : rssis) {
            buffer[bufferPos] = (byte) abs(v.as<int>());
            bufferPos++;
        }

        while (bufferPos - startingRssiBytes != 10) {
            buffer[bufferPos] = (byte) NULL_RSSI;
            bufferPos++;
        }
    }
    rssiValuesCharacteristic.writeValue(buffer, bufferPos);
}

void loop() {
    // poll for BLE events
	BLE.poll();
}
