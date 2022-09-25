"""
Bleak Scanner
-------------



Updated on 2020-08-12 by hbldh <henrik.blidh@nedomkull.com>

"""

import asyncio
import platform
import sys

from bleak import BleakScanner


ADDRESS = (
    "30:C6:F7:2F:27:32"
    if platform.system() != "Darwin"
    else "1016407A-D89D-907C-F7C3-0C6B31A3B400"
)


async def main(address):
    device = await BleakScanner.find_device_by_address(address)
    print(device)


if __name__ == "__main__":
    asyncio.run(main(sys.argv[1] if len(sys.argv) == 2 else ADDRESS))
