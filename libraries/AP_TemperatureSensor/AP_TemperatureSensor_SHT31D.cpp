#include "AP_TemperatureSensor_config.h"

#if AP_TEMPERATURE_SENSOR_SHT31D_ENABLED

#include "AP_TemperatureSensor_SHT31D.h"

#include <AP_HAL/AP_HAL.h>
#include <AP_HAL/I2CDevice.h>
#include <AP_Math/AP_Math.h>
#include <GCS_MAVLink/GCS.h>

#define ERROR_TEMPERATURE_C -300.0f


extern const AP_HAL::HAL &hal;

#define SHT31_DEFAULT_I2C_ADDR 0x44    // Device default slave address 
// #define MLX90614_BROADCASTADDR  0       // Device broadcast slave address 

//  RAM addresses
#define SHT31_DEFAULT_ADDR 0x44 /**< SHT31 Default Address */
#define SHT31_MEAS_HIGHREP_STRETCH                                             \
  0x2C06 /**< Measurement High Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_MEDREP_STRETCH                                              \
  0x2C0D /**< Measurement Medium Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_LOWREP_STRETCH                                              \
  0x2C10 /**< Measurement Low Repeatability with Clock Stretch Enabled*/
#define SHT31_MEAS_HIGHREP                                                     \
  0x2400 /**< Measurement High Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_MEDREP                                                      \
  0x240B /**< Measurement Medium Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_LOWREP                                                      \
  0x2416 /**< Measurement Low Repeatability with Clock Stretch Disabled */
#define SHT31_READSTATUS 0xF32D   /**< Read Out of Status Register */
#define SHT31_CLEARSTATUS 0x3041  /**< Clear Status */
#define SHT31_SOFTRESET 0x30A2    /**< Soft Reset */
#define SHT31_HEATEREN 0x306D     /**< Heater Enable */
#define SHT31_HEATERDIS 0x3066    /**< Heater Disable */
#define SHT31_REG_HEATER_BIT 0x0d /**< Status Register Heater Bit */

void AP_TemperatureSensor_SHT31D::init()
{
    _params.bus_address.set_default(SHT31_DEFAULT_I2C_ADDR);
    
    _dev = std::move(hal.i2c_mgr->get_device(_params.bus, _params.bus_address));
    if (!_dev) {
        return;
    }

    GCS_SEND_TEXT(MAV_SEVERITY_INFO, "SHT31D initialized");
    
    WITH_SEMAPHORE(_dev->get_semaphore());


    _dev->register_periodic_callback(50 * AP_USEC_PER_MSEC,
                                     FUNCTOR_BIND_MEMBER(&AP_TemperatureSensor_SHT31D::_timer, void));
}



static uint8_t crc8(const uint8_t *data, int len) {
  /*
   *
   * CRC-8 formula from page 14 of SHT spec pdf
   *
   * Test data 0xBE, 0xEF should yield 0x92
   *
   * Initialization data 0xFF
   * Polynomial 0x31 (x8 + x5 +x4 +1)
   * Final XOR 0x00
   */

  const uint8_t POLYNOMIAL(0x31);
  uint8_t crc(0xFF);

  for (int j = len; j; --j) {
    crc ^= *data++;

    for (int i = 8; i; --i) {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}



void AP_TemperatureSensor_SHT31D::_timer()
{
    uint8_t write[2];
    write[0] = static_cast<char>(SHT31_MEAS_HIGHREP >> 8);
    write[1] = static_cast<char>(SHT31_MEAS_HIGHREP & 0xFF);
    uint8_t readbuffer[6];

    if(!_dev->transfer(write, 2, readbuffer, 6)){
      set_temperature(ERROR_TEMPERATURE_C);
      return;
    }
    
    if (readbuffer[2] != crc8(readbuffer, 2) ||
        readbuffer[5] != crc8(readbuffer + 3, 2))
    {
        set_temperature(ERROR_TEMPERATURE_C);
        return;
    }

    int32_t stemp = (int32_t)(((uint32_t)readbuffer[0] << 8) | readbuffer[1]);
    // simplified (65536 instead of 65535) integer version of:
    // temp = (stemp * 175.0f) / 65535.0f - 45.0f;
    stemp = ((4375 * stemp) >> 14) - 4500;
    float temp = (float)stemp / 100.0f;

    // uint32_t shum = ((uint32_t)readbuffer[3] << 8) | readbuffer[4];
    // simplified (65536 instead of 65535) integer version of:
    // humidity = (shum * 100.0f) / 65535.0f;
    // shum = (625 * shum) >> 12    ;
    // float humidity = (float)shum / 100.0f;

    WITH_SEMAPHORE(_dev->get_semaphore());
    set_temperature(temp);
}



// uint16_t AP_TemperatureSensor_SHT31D::read_data(uint8_t cmd)
// {
//     uint8_t val[3];
  
//     if (!_dev->transfer(&cmd, 1, val, 3)) {
//         return 0;
//     }
//     return UINT16_VALUE(val[1],val[0]);
// }
#endif // AP_TEMPERATURE_SENSOR_SHT31D_ENABLED