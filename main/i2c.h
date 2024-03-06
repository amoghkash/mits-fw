#include "board.h"

#define I2C_MASTER_SCL_IO           I2C_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           I2C_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000





// Initalizer Functions
esp_err_t configure_I2C(void);

void i2c_read_byte(uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data);

void i2c_test(void);


void mpu_startup_seq(void);