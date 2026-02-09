#include "imu.h"
#include <new>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "driver/i2c.h"
#include <cstring>

static const char *TAG = "IMU";
#define I2C_EXAMPLE_MASTER_SCL_IO           2                /*!< gpio number for I2C master clock */
#define I2C_EXAMPLE_MASTER_SDA_IO           14               /*!< gpio number for I2C master data  */
#define I2C_EXAMPLE_MASTER_NUM              I2C_NUM_0        /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE   0                /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE   0                /*!< I2C master do not need buffer */

#define MPU6050_SENSOR_ADDR                 0x68             /*!< slave address for MPU6050 sensor */
#define MPU6050_CMD_START                   0x41             /*!< Command to set measure mode */
#define MPU6050_WHO_AM_I                    0x75             /*!< Command to read WHO_AM_I reg */
#define WRITE_BIT                           I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                            I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                        0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                       0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                             0x0              /*!< I2C ack value */
#define NACK_VAL                            0x1              /*!< I2C nack value */
#define LAST_NACK_VAL                       0x2              /*!< I2C last_nack value */
/**
 * Define the mpu6050 register address:
 */
#define SMPLRT_DIV      0x19
#define CONFIG          0x1A
#define GYRO_CONFIG     0x1B
#define ACCEL_CONFIG    0x1C
#define ACCEL_XOUT_H    0x3B
#define ACCEL_XOUT_L    0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H    0x3F
#define ACCEL_ZOUT_L    0x40
#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42
#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48
#define PWR_MGMT_1      0x6B
#define WHO_AM_I        0x75  /*!< Command to read WHO_AM_I reg */


static esp_err_t i2c_example_master_init()
{
    i2c_port_t i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = (gpio_pullup_t)1;
    conf.scl_io_num = (gpio_num_t)I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = (gpio_pullup_t)1;
    conf.clk_stretch_tick = 300; // 300 ticks, Clock stretch is about 210us, you can make changes according to the actual situation.
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, conf.mode));
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    return ESP_OK;
}

static esp_err_t i2c_example_master_mpu6050_read(i2c_port_t i2c_num, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read(cmd, data, data_len, (i2c_ack_type_t)LAST_NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

static esp_err_t i2c_example_master_mpu6050_write(i2c_port_t i2c_num, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
    i2c_master_write(cmd, data, data_len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

static esp_err_t i2c_example_master_mpu6050_init(i2c_port_t i2c_num)
{
    uint8_t cmd_data;
    vTaskDelay(100 / portTICK_RATE_MS);
    esp_err_t ret =  i2c_example_master_init();
    printf("Error code : %d\n", ret);

    ESP_LOGI(TAG, "Scanning I2C bus...");

    int found = 0;

    for (uint8_t addr = 1; addr < 127; addr++) {

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();

        i2c_master_start(cmd);
        i2c_master_write_byte(cmd,
                              (addr << 1) | I2C_MASTER_WRITE,
                              true);
        i2c_master_stop(cmd);

        esp_err_t ret = i2c_master_cmd_begin(
            I2C_NUM_0,
            cmd,
            pdMS_TO_TICKS(1000));

        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Device found at 0x%02X", addr);
            found++;
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }

    if (!found)
        ESP_LOGI(TAG, "No devices found");


    cmd_data = 0x00;    // reset mpu6050
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, PWR_MGMT_1, &cmd_data, 1));
    cmd_data = 0x07;    // Set the SMPRT_DIV
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, SMPLRT_DIV, &cmd_data, 1));
    cmd_data = 0x06;    // Set the Low Pass Filter
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, CONFIG, &cmd_data, 1));
    cmd_data = 0x18;    // Set the GYRO range
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, GYRO_CONFIG, &cmd_data, 1));
    cmd_data = 0x01;    // Set the ACCEL range
    ESP_ERROR_CHECK(i2c_example_master_mpu6050_write(i2c_num, ACCEL_CONFIG, &cmd_data, 1));
    return ESP_OK;
}

void IMU::init()
{
    i2c_example_master_mpu6050_init(I2C_EXAMPLE_MASTER_NUM);

    //i2c_driver_delete(I2C_EXAMPLE_MASTER_NUM);

	// This need to be setup individually
	//mpu->setXGyroOffset(220);
	//mpu->setYGyroOffset(76);
	//mpu->setZGyroOffset(-85);
	//mpu->setZAccelOffset(1788);
}

void IMU::step(float dt)
{

    uint8_t who_am_i = 0;
    int ret;
    int error_count = 0;
    uint8_t sensor_data[14];
    i2c_example_master_mpu6050_read(I2C_EXAMPLE_MASTER_NUM, WHO_AM_I, &who_am_i, 1);

    if (0x68 != who_am_i) {
        error_count++;
    }

    memset(sensor_data, 0, 14);
    ret = i2c_example_master_mpu6050_read(I2C_EXAMPLE_MASTER_NUM, ACCEL_XOUT_H, sensor_data, 14);

    if (ret == ESP_OK) 
    {
        /*ESP_LOGI(TAG, "*******************\n");
        //ESP_LOGI(TAG, "WHO_AM_I: 0x%02x\n", who_am_i);
        //float Temp = 36.53 + ((double)(int16_t)((sensor_data[6] << 8) | sensor_data[7]) / 340);
        ESP_LOGI(TAG, "TEMP: %d.%d\n", (uint16_t)Temp, (uint16_t)(Temp * 100) % 100);

        for (int i = 0; i < 7; i++) {
            ESP_LOGI(TAG, "sensor_data[%d]: %d\n", i, (int16_t)((sensor_data[i * 2] << 8) | sensor_data[i * 2 + 1]));
        }

        ESP_LOGI(TAG, "error_count: %d\n", error_count);*/

        int16_t ax = (sensor_data[0] << 8) | sensor_data[1];
        int16_t ay = (sensor_data[2] << 8) | sensor_data[3];
        int16_t az = (sensor_data[4] << 8) | sensor_data[5];

        //int16_t temp_raw = (sensor_data[6] << 8) | sensor_data[7];

        int16_t gx = (sensor_data[8] << 8) | sensor_data[9];
        int16_t gy = (sensor_data[10] << 8) | sensor_data[11];
        int16_t gz = (sensor_data[12] << 8) | sensor_data[13];

        data.ax = ax / 16384.0f;
        data.ay = ay / 16384.0f;
        data.az = az / 16384.0f;

        data.gx = gx / 16.4f;
        data.gy = gy / 16.4f;
        data.gz = gz / 16.4f;

        //float temp = 36.53 + temp_raw / 340.0f;

        //ESP_LOGI(TAG, "Temp: %.2f C", temp);
    } 
    else 
    {
        ESP_LOGE(TAG, "No ack, sensor not connected...skip...\n");
    }
    /*printf("GY:%d.%03d ",
        (int)data.gy,
        abs((int)(data.gy * 1000) % 1000));*/
    
    /*printf("AX:%d.%03d ",
        (int)data.ax,
        abs((int)(data.ax * 1000) % 1000));

    printf("AY:%d.%03d ",
        (int)data.ay,
        abs((int)(data.ay * 1000) % 1000));

    printf("AZ:%d.%03d ",
        (int)data.az,
        abs((int)(data.az * 1000) % 1000));

    printf("GX:%d.%03d ",
        (int)data.gx,
        abs((int)(data.gx * 1000) % 1000));

    printf("GY:%d.%03d ",
        (int)data.gy,
        abs((int)(data.gy * 1000) % 1000));

    printf("GZ:%d.%03d\n",
        (int)data.gz,
        abs((int)(data.gz * 1000) % 1000)); */
    
}