import logging

from bluezero import peripheral
from bluezero import adapter

PIRIPHERAL_SERV = "070106ff-d31e-4828-a39c-ab6bf7097fe0"
PIRIPHERAL_CHAR = "070106ff-d31e-4828-a39c-ab6bf7097fe1"
PIRIPHERAL_DESC = "070106ff-d31e-4828-a39c-ab6bf7097fe2"

def write_callback(value, options):
    print(f'Received value: {value.decode("utf-8")}')

def main(adapter_address):
    """Create peripheral"""
    print(adapter_address)
    logger = logging.getLogger('GATT')
    logger.setLevel(logging.DEBUG)

    piripheral = peripheral.Peripheral(adapter_address=adapter_address,
                          local_name='Piripheral',
                          appearance=1344)

    piripheral.add_service(srv_id=1, uuid=PIRIPHERAL_SERV, primary=True)
    piripheral.add_characteristic(srv_id=1, chr_id=1, uuid=PIRIPHERAL_CHAR,
                                  value=[], notifying=False,
                                  flags=['write', 'read'],
                                  read_callback=None,
                                  write_callback=write_callback,
                                  notify_callback=None)

    piripheral.add_descriptor(srv_id=1, chr_id=1, dsc_id=1, uuid=PIRIPHERAL_DESC,
                               value=[0x0E, 0xFE, 0x2F, 0x27, 0x01, 0x00,
                                      0x00],
                               flags=['read'])

    piripheral.publish()

if __name__ == '__main__':
    main(list(adapter.Adapter.available())[0].address)