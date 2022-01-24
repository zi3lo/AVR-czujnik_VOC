/*
    Andrzej Zieliński
        - 2020 -
 */

#include "CCS811.hpp"

CCS811::CCS811() : ccs(CCS811_ADDR)  //i2c
{

}

uint8_t CCS811::init()
{
    uint8_t data = read_u8b(ID_REG);        // czytamy ID (powinno być 0x81)
    if (data != 0x81)
        return 1;                           // jeżeli występuje niezgodność ID zwracamy 1
    data = read_u8b(STATUS_REG);            // czytamy rejestr statusu
    if (!(data & APP_VALID))
        return 2;                           // sprawdzamy firmware - jeżeli niezgodność to zwracamy 2
    write_only(APP_START_REG);              // rozpoczynamy pomiary
    data = read_u8b(STATUS_REG);            // czytamy rejestr statusu
    if (!(data & FW_MODE))
        return 3;                           // sprawdzamy czy czyjnik w trybie pomiaru, jeśli nie to zwracamy 3
    data = ((DRIVE_MODE << 4) | (INT_DATARDY << 3) | (INT_THRESH << 2)); // konfiguracja
    write_u8b(MODE_REG, data);
    return 0;                               // jeśli wszystko gra 0
}

void CCS811::get_data()
{
    uint32_t data = read_data();
    co2_ = (uint16_t)(data >> 16);    // dwa starsza bajty
    tvoc_ = (uint16_t)(data & 0xFF);  // dwa młodzsze bajty
}

uint16_t CCS811::co2()
{
    return co2_;
}

uint16_t CCS811::tvoc()
{
    return tvoc_;
}

uint8_t CCS811::data_status()
{
    uint8_t data = read_u8b(STATUS_REG);
    if (data & DATA_READY)
        return 1;
    else
        return 0;
}

void CCS811::comp_val(uint16_t hum, int16_t temp_)                              // zapis do 4 bajtowego rejestru
{
    uint8_t dat;
    uint16_t temp = (uint16_t)(temp_ + 2500);                                       // zgodnie z notą temperatura +25*C
    uint32_t comp_data;

    dat = (hum % 10) > 7 ? ((hum/10 + 1) << 1) : ((hum/10) << 1);                    // 7 bitów części całkowitej i jeden bit 0,5. - jeśli po przecinku 8..9
    if(((hum % 10 ) > 2) && (((hum % 10) ) < 8))                                    // jeśli po przecinku 3..7
    {
        dat |= 1;
    }

    comp_data = dat;
    comp_data <<= 16;

    dat = ((temp % 100) / 10) > 7 ? ((temp/100 + 1) << 1) : ((temp/100)<<1);            // 7 bitów części całkowitej i jeden bit 0,5. - jeśli po przecinku 8..9
    if((((temp % 100) / 10) > 2) && (((temp % 100) / 10) < 8))                        // jeśli po przecinku 3..7
    {
        dat |= 1;
    }

    comp_data |= dat;
    comp_data <<= 8;

    write_u32b(0x05,comp_data);
}

uint32_t CCS811::read_data()
{
    uint32_t data;
    ccs.start();
    ccs.sla_w();
    ccs.send(ALG_RESULT_DATA);
    ccs.start();
    ccs.sla_r();
    data = ccs.get_ack();
    data <<= 8;
    data |= ccs.get_ack();
    data <<= 8;
    data |= ccs.get_ack();
    data <<= 8;
    data |= ccs.get_nack();
    ccs.stop();
    return data;
}


uint8_t CCS811::read_u8b(uint8_t reg)
{
    uint8_t data;
    ccs.start();
    ccs.sla_w();
    ccs.send(reg);
    ccs.start();
    ccs.sla_r();
    data = ccs.get_nack();
    ccs.stop();
    return data;
}

void CCS811::write_u8b(uint8_t reg, uint8_t data)
{
    ccs.start();
    ccs.sla_w();
    ccs.send(reg);
    ccs.send(data);
    ccs.stop();
}

void CCS811::write_u32b(uint8_t reg, uint32_t data)
{
    ccs.start();
    ccs.sla_w();
    ccs.send(reg);
    ccs.send((data & 0xff000000) >> 24);
    ccs.send((data & 0x00ff0000) >> 16);
    ccs.send((data & 0x0000ff00) >> 8);
    ccs.send((data & 0x000000ff) >> 0);
    ccs.stop();
}

void CCS811::write_only(uint8_t reg)
{
    ccs.start();
    ccs.sla_w();
    ccs.send(reg);
    ccs.stop();
}


