#include "imu.h"
#include <new>
#include "freertos/FreeRTOS.h"

#define PIN_SDA 4
#define PIN_CLK 5

void IMU::init()
{
    /*i2c_config_t conf = {};

    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)PIN_SDA;
    conf.scl_io_num = (gpio_num_t)PIN_CLK;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode));


    mpu = new(mpu_buffer) MPU6050();
	mpu->initialize();

	// This need to be setup individually
	//mpu->setXGyroOffset(220);
	//mpu->setYGyroOffset(76);
	//mpu->setZGyroOffset(-85);
	//mpu->setZAccelOffset(1788);
*/
}

void IMU::step(float /*dt*/)
{
    //mpu->getMotion6(&data.ax, &data.ay, &data.az, &data.gx, &data.gy, &data.gz);
    data.ax = 0;
    data.ay = 0;
    data.az = 0;
    data.gx = 1;
    data.gy = 3;
    data.gz = 2;


    printf("AX: %d, ", data.ax);
    printf("AY: %d, ", data.ay);
    printf("AZ: %d, ", data.az);
    printf("GX: %d, ", data.gx);
    printf("GY: %d, ", data.gy);
    printf("GZ: %d \n", data.gz);

}