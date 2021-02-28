macs = ["9D:FF:70:45:D3:EC","AD:DB:1B:3B:4A:EE","C8:4F:C0:45:D4:44","49:91:C8:E6:5B:26","65:2A:00:9C:8D:B4","DF:9B:3E:F8:79:93","17:A6:2C:C8:1E:31","FC:D7:98:C7:85:2B","31:3A:AD:EB:48:17","4E:F5:C2:7B:E4:38","DF:F5:41:08:24:2D","10:85:8A:CC:8C:24","E9:0D:D3:5A:24:F6","50:E7:41:45:1D:AA","77:6F:38:31:13:EF","63:98:79:FC:9D:4D","E8:20:5C:AC:30:B6","9F:8A:26:99:A3:11","C0:E5:E2:2C:A4:C0","06:C4:78:A0:72:A9","94:23:74:43:42:82","4A:FF:EB:FE:2B:29","3D:9A:F3:48:76:BE","A0:1F:29:17:30:61","C5:0E:02:74:F0:EF","32:7B:93:93:3B:25","44:C8:42:90:79:E8","C5:CB:5A:A9:D2:46","5D:98:8C:DD:0A:54","CE:54:83:C4:98:FE","2A:7F:CB:51:9F:91","B6:EB:03:FD:24:1A"]

min_rssi = 20
max_rssi = 100
max_num_rssis = 10
min_num_rssis = 3

import random

result = {}

for mac in macs:
    rssis_mac = []
    num_rssis = random.randint(min_num_rssis, max_num_rssis)
    i = 0
    while (i < num_rssis):
        rssis_mac += [-random.randint(min_rssi, max_rssi)]
        i += 1
    result[mac] = rssis_mac

print(result)
