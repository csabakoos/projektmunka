"""
Services
----------------

An example showing how to fetch all services and print them.

Updated on 2019-03-25 by hbldh <henrik.blidh@nedomkull.com>

"""

import sys
import asyncio
import platform

from bleak import BleakClient

ADDRESS = (
    "30:C6:F7:2F:27:32"
    if platform.system() != "Darwin"
    else "1016407A-D89D-907C-F7C3-0C6B31A3B400"
)


async def main(address: str):
    async with BleakClient(address) as client:
        svcs = await client.get_services()
        print("Services:")
        for service in svcs:
            print(service)


if __name__ == "__main__":
    asyncio.run(main(sys.argv[1] if len(sys.argv) == 2 else ADDRESS))
