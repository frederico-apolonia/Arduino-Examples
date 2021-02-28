import argparse
import pygatt

RSSI_VALUES = "3509B2C7-2F88-4D48-9B13-FB0092951128"

MAC_SIZE_BYTES = 6
RSSI_SIZE_BYTES = 1
NUM_RRSI = 10

parser = argparse.ArgumentParser(description="Subscribe to MACSerializer Arduino characteristic and unserialize the notification values.")
parser.add_argument('mac', metavar='MAC', type=str, nargs=1, help='Arduino MAC Address')

def data_handler_cb(handler, value):
    result = {}
    curr_byte = 0
    while curr_byte + (NUM_RRSI * RSSI_SIZE_BYTES) + MAC_SIZE_BYTES < len(value):
        mac_address = value[curr_byte:(curr_byte+MAC_SIZE_BYTES)].hex(':')
        curr_byte += MAC_SIZE_BYTES
        rssis = []
        curr_rssi = 0
        while curr_rssi < NUM_RRSI:
            rssis += [-(value[curr_byte])]
            curr_byte += RSSI_SIZE_BYTES
            curr_rssi += 1

        result[mac_address] = rssis

    print(result)

def main():
    args = parser.parse_args()
    mac_address = vars(args)['mac'][0]
    adapter = pygatt.GATTToolBackend()
    adapter.start()

    device = adapter.connect(mac_address)

    # set max characteristic value size to match Arduino's characteristic size
    device.exchange_mtu(512)

    # subscribe to arduino's RSSI value
    device.subscribe(RSSI_VALUES, callback=data_handler_cb, wait_for_response=False)

    while True:
        pass

    adapter.stop()
    return 0

if __name__ == '__main__':
    exit(main())