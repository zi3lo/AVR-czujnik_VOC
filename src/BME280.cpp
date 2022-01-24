/*
    Andrzej Zieliński
        - 2020 -
 */

#include "BME280.hpp"

BME280::BME280() : bosh(BME_ADDR)   // iniciujemy I2C
{
     //init();
}

int32_t BME280::read_temp()
{
    return bme280_comp_T( (int32_t)read_u24b(0xfa));
}

uint32_t BME280::read_press()
{
    return bme280_com_P((int32_t)read_u24b(0xf7) );
}

uint32_t BME280::read_hum()
{
    return bme280_com_H((uint32_t)read_u16b(0xfd) );
}
uint8_t BME280::init()
{
    uint8_t data = read_u8b(ID_CHIP);
    if (data != 0x60) return 1;                                             // czy id chipu się zgadza
    else
    {
        read_com_par();                                                     // czytamy dane kalibracyne
        write_u8b(0xf2, OVRS_H);                                            // hum overs
        write_u8b(0xf5, ((T_SB << 5) | (FILTER << 2) | (SPI << 0)));        // config
        write_u8b(0xf4, ((OVRS_T << 5) | (OVRS_P << 2) | (MODE << 0)));     // oversamplingi temp, press i mode
        return 0;
    }

}

uint8_t BME280::read_u8b(uint8_t reg)
{
    uint8_t data;
    bosh.start();
    bosh.sla_w();
    bosh.send(reg);
    bosh.start();
    bosh.sla_r();
    data = bosh.get_nack();
    bosh.stop();
    return data;
}

uint16_t BME280::read_u16b(uint8_t reg)
{
    uint16_t data;
    bosh.start();
    bosh.sla_w();
    bosh.send(reg);
    bosh.start();
    bosh.sla_r();
    data = (uint16_t)bosh.get_ack();
    data <<= 8;
    data |= (uint16_t)bosh.get_nack();
    bosh.stop();
    return data;
}

uint32_t BME280::read_u24b(uint8_t reg)
{
    uint32_t data;
    bosh.start();
    bosh.sla_w();
    bosh.send(reg);
    bosh.start();
    bosh.sla_r();
    data = bosh.get_ack();
    data <<= 8;
    data |= bosh.get_ack();
    data <<= 8;
    data |= bosh.get_nack();
    bosh.stop();
    return data;
}

uint16_t BME280::read_u16b_ro(uint8_t reg)
{
    uint16_t data;
    data = read_u16b(reg);
    return (data << 8) | (data >> 8);
}


int16_t BME280::read_s16b_ro(uint8_t reg)
{
    return (int16_t)read_u16b_ro(reg);
}


void BME280::write_u8b(uint8_t addr, uint8_t data)
{
    bosh.start();
    bosh.sla_w();
    bosh.send(addr);
    bosh.send(data);
    bosh.stop();
}

void BME280::read_com_par()
{
    dig_T1 = read_u16b_ro(0x88);
    dig_T2 = read_s16b_ro(0x8a);
    dig_T3 = read_s16b_ro(0x8c);

    dig_P1 = read_u16b_ro(0x8e);
    dig_P2 = read_s16b_ro(0x90);
    dig_P3 = read_s16b_ro(0x92);
    dig_P4 = read_s16b_ro(0x94);
    dig_P5 = read_s16b_ro(0x96);
    dig_P6 = read_s16b_ro(0x98);
    dig_P7 = read_s16b_ro(0x9a);
    dig_P8 = read_s16b_ro(0x9c);
    dig_P9 = read_s16b_ro(0x9e);

    dig_H1 = read_u8b(0xa1);
    dig_H2 = read_s16b_ro(0xe1);
    dig_H3 = read_u8b(0xe3);
    dig_H4 = (read_u8b(0xe4) << 4) | (read_u8b(0xe5) & 0xf);
    dig_H5 = (read_u8b(0xe6) << 4) | (read_u8b(0xe5) >> 4);
    dig_H6 = (int8_t)read_u8b(0xe7);
}

//// funkcje z noty katalogowej

int32_t BME280::bme280_comp_T(int32_t adc_T)
{
    adc_T >>= 4;
    int32_t var1, var2, T;
    var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}


uint32_t BME280::bme280_com_P(int32_t adc_P)
{
    adc_P >>= 4;
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
    var2 = var2 + (((int64_t)dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
    if (var1 == 0)
    {
    return 0; // avoid exception caused by division by zero
    }
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
    return (uint32_t)p;
}

uint32_t BME280::bme280_com_H(uint32_t adc_H)
{
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (uint32_t)(v_x1_u32r>>12);
}

