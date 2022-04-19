import bluetooth, time
from modules.bluetooth import espble

if __name__ == "__main__":
    ble = bluetooth.BLE()
    p = espble(ble)

    i = 0
    while True:
        if p.is_connected():
            for _ in range(3):
                data = str(i) + "_"
                print("TX", data)
                p.send(data)
                i += 1
                time.sleep_ms(1000)
