import argparse
from time import time
import pygatt
from uuid import UUID

parser = argparse.ArgumentParser(description="Subscribe to LED Arduino characteristic and print LED notification status.")
parser.add_argument('mac', metavar='MAC', type=str, nargs=1, help='Arduino MAC Address')

LED_SERVICE_UUID_STR = "19B10000-E8F2-537E-4F6C-D104768A1214"
LED_SERVICE_UUID = UUID(LED_SERVICE_UUID_STR)
CHANGE_LED_SERVICE_UUID_STR = "19B10001-E8F2-537E-4F6C-D104768A1214"
CHANGE_LED_SERVICE_UUID = UUID(CHANGE_LED_SERVICE_UUID_STR)

def data_handler_cb(handler, value):
    led_status = bool.from_bytes(value, byteorder="big", signed=False)
    print(f"LED Status: {led_status}")

def main():
    args = parser.parse_args()
    mac_address = vars(args)['mac'][0]
    adapter = pygatt.GATTToolBackend()
    adapter.start()
    available_devices = adapter.scan(timeout=10)

    for avail_device in available_devices:
        print(avail_device)

    device = adapter.connect(mac_address)
    device.subscribe("00001525-1212-EFDE-1523-785FEABCD123", callback=data_handler_cb, wait_for_response=False)

    while True:
        pass

    adapter.stop()
    return 0

if __name__ == '__main__':
    exit(main())