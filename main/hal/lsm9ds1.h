#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

struct device_transaction {
    uint8_t     address;
    uint8_t     data;
};



void lsm9ds1_init(void);
esp_err_t lsm9ds1_connect(void);
esp_err_t lsm9ds1_write(uint8_t data);
esp_err_t lsm9ds1_read(int16_t *buf);

esp_err_t lsm9ds1_activate(void);

// Register Definitions
#define LSM9DS1_ADDRESS                 0x6B
#define LSM9DS1_ACCEL_CTRL_REG1         0x20
#define LSM9DS1_GYRO_CTRL_REG1          0x10

#define LSM9DS1_ACCEL_X_L               0x28
#define LSM9DS1_ACCEL_X_H               0x29
#define LSM9DS1_ACCEL_Y_L               0x2A
#define LSM9DS1_ACCEL_Y_H               0x2B
#define LSM9DS1_ACCEL_Z_L               0x2C
#define LSM9DS1_ACCEL_Z_H               0x2D

// Register Values
#define LSM9DS1_ACCEL_CTRL_REG1_VAL     0xD8
#define LSM9DS1_GYRO_CTRL_REG1_VAL      0xC8

#ifdef __cplusplus
    }
#endif