from machine import Pin, SoftI2C
from time import sleep_ms
import os, sys

mpu_addr = 0x68
mpu_pwr_mgmt = 0x6b

mpu_accel_x_h = 0x3b
mpu_accel_x_l = 0x3c
mpu_accel_y_h = 0x3d
mpu_accel_y_l = 0x3e
mpu_accel_z_h = 0x3f
mpu_accel_z_l = 0x40

mpu_gyro_x_h = 0x43
mpu_gyro_x_l = 0x44
mpu_gyro_y_h = 0x45
mpu_gyro_y_l = 0x46
mpu_gyro_z_h = 0x47
mpu_gyro_z_l = 0x48

mpu_gyro_sel = 340.0
mpu_accel_sel = 16348.0

class mpu6050():
    def __init__(self, i2c):
        self.i2c = i2c
        
    def power(self):
        self.i2c.writeto_mem(mpu_addr, mpu_pwr_mgmt, bytes([0]))

    def combine_register_values(self, h, l):
        return h[0] << 8 | l[0]

    def get_accel(self):
        accel_x_h = self.i2c.readfrom_mem(mpu_addr, mpu_accel_x_h, 1)
        accel_x_l = self.i2c.readfrom_mem(mpu_addr, mpu_accel_x_l, 1)
        accel_y_h = self.i2c.readfrom_mem(mpu_addr, mpu_accel_y_h, 1)
        accel_y_l = self.i2c.readfrom_mem(mpu_addr, mpu_accel_y_l, 1)
        accel_z_h = self.i2c.readfrom_mem(mpu_addr, mpu_accel_z_h, 1)
        accel_z_l = self.i2c.readfrom_mem(mpu_addr, mpu_accel_z_l, 1)

        return [self.accel_sel(self.combine_register_values(accel_x_h, accel_x_l)),
                self.accel_sel(self.combine_register_values(accel_y_h, accel_y_l)),
                self.accel_sel(self.combine_register_values(accel_z_h, accel_z_l))]

    def accel_sel(self, accel):
        return accel / mpu_accel_sel

    def get_gyro(self):
        gyro_x_h = self.i2c.readfrom_mem(mpu_addr, mpu_gyro_x_h, 1)
        gyro_x_l = self.i2c.readfrom_mem(mpu_addr, mpu_gyro_x_l, 1)
        gyro_y_h = self.i2c.readfrom_mem(mpu_addr, mpu_gyro_y_h, 1)
        gyro_y_l = self.i2c.readfrom_mem(mpu_addr, mpu_gyro_y_l, 1)
        gyro_z_h = self.i2c.readfrom_mem(mpu_addr, mpu_gyro_z_h, 1)
        gyro_z_l = self.i2c.readfrom_mem(mpu_addr, mpu_gyro_z_l, 1)

        return [self.gyro_sel(self.combine_register_values(gyro_x_h, gyro_x_l)),
                self.gyro_sel(self.combine_register_values(gyro_y_h, gyro_y_l)),
                self.gyro_sel(self.combine_register_values(gyro_z_h, gyro_z_l))]

    def gyro_sel(self, gyro):
        return gyro / mpu_gyro_sel

