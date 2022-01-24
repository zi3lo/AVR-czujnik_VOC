/*
    Andrzej Zieliński
        - 2020 -
 */

#ifndef BME280_H
#define BME280_H

#include <avr/io.h>
#include "I2C.hpp"

#define ID_CHIP 0xd0        // id układu (powinno być 0x60)
#define BME_ADDR 0x76       // SDO do GND - 0x76, do VCC - 0x77
#define MODE 0x3            // 0x0 - sleep, 0x1 lub 0x2 - forced, 0x3 - normal
#define OVRS_H 0x5          // oversampling x16 | 0x4 - x8 | 0x3 - x4 | 0x2 - x2 | 0x1 - x1 | 0 - off
#define OVRS_P 0x5          // H - hum, P - ress, T - temp
#define OVRS_T 0x5
// 0xf5
#define T_SB 0x7            // standby time - 20ms
#define FILTER 0x4          // filter - 16, 0x0 - off, 0x1 - 2, 0x2 - 4, 0x3 - 8
#define SPI 0               // spi - 0

class BME280
{
    public:
        BME280();
        int32_t read_temp();
        uint32_t read_press();
        uint32_t read_hum();
        uint8_t init();

    private:
        //void init();

        uint8_t read_u8b(uint8_t);
        uint16_t read_u16b(uint8_t);
        uint32_t read_u24b(uint8_t);
        uint16_t read_u16b_ro(uint8_t);         // reverse order
        int16_t read_s16b_ro(uint8_t);          // reverse order
        void write_u8b(uint8_t, uint8_t);
        void read_com_par();

        int32_t bme280_comp_T(int32_t);
        uint32_t bme280_com_P(int32_t);
        uint32_t bme280_com_H(uint32_t);

        uint16_t dig_T1, dig_P1;                // compensation parametr  - stałe kalibracyjne
        uint8_t dig_H1, dig_H3;
        int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7,
                dig_P8, dig_P9, dig_H2, dig_H4, dig_H5, dig_H6;

        volatile int32_t t_fine;                // wg noty
        I2C bosh;

};

#endif // BME280_H
