/*
    Andrzej Zieliński
        - 2020 -
      czujnik TVOC
 */

//#define F_CPU 1000000UL


#include <avr/io.h>
#include <util/delay.h>
#include "HD44780.hpp"
//#include <I2C.hpp>
#include "BME280.hpp"
#include <stdio.h>
#include "CCS811.hpp"

#define DEG 0b11011111  // znaczek stopnia

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_RED (1 << PB4)
#define LED_YEL (1 << PB2)
#define LED_GRE (1 << PB1)


int main(void)
{
    LED_DDR |= ( LED_RED | LED_YEL | LED_GRE); // piny LED jako wyjścia


    HD44780 lcd;
    CCS811 ccs811;
    BME280 bme;

    int8_t t_1, ccs_stat, bme_stat;
    uint8_t t_01,val_hum_1;
    uint16_t co2 = 0, tvoc = 0;
    int32_t val_temp;
    uint32_t val_press, val_hum;


    lcd.init();
    lcd.s_clear();

    ccs_stat = ccs811.init();
    bme_stat = bme.init();

    char line1[40] = "                     ", line2[40] = "                      ";
    char l1_1[10], l1_2[10], l2_1[10], l2_2[30];
    if (ccs_stat)
    {
        sprintf(line1,"CCS error: %i", ccs_stat);
        lcd.send_text(line1,line2);
        return 0;
    }
    if (bme_stat)
    {
        sprintf(line1,"BME error");
        lcd.send_text(line1,line2);
        return 0;
    }


    while(1)
    {
    val_temp = bme.read_temp();
    t_1 = val_temp / 100;
    t_01 = val_temp % 100;
    val_press = bme.read_press();
    val_press /= 25600;
    val_hum = bme.read_hum();
    val_hum_1 = val_hum / 1024;
    ccs811.comp_val((val_hum*10/1024),val_temp);
    if (ccs811.data_status())
    {
        ccs811.get_data();
        co2 = ccs811.co2();
        tvoc = ccs811.tvoc();
    }

    sprintf (l1_1,"co2 %u",co2);
    sprintf (l1_2,"  %uhPa",(uint16_t)val_press);
    sprintf (l2_1,"v %u",tvoc);
    sprintf (l2_2,"   %u%% %i.%u%cC",(uint8_t)val_hum_1,t_1,t_01/10,DEG);

    lcd.go_to(7,0);
    lcd.send_text(l1_2);
    lcd.go_to(0,0);
    lcd.send_text(l1_1);
    lcd.go_to(3,1);
    lcd.send_text(l2_2);
    lcd.go_to(0,1);
    lcd.send_text(l2_1);

    LED_PORT &= ~(LED_GRE | LED_YEL | LED_RED); // gasimy diody

    if (tvoc < 80 && co2 < 1000 && val_hum_1 > 40 && val_hum_1 < 60)
        LED_PORT |= LED_GRE;
    else if (tvoc > 240 || co2 > 2000 || val_hum_1 < 35 || val_hum_1 > 65)
        LED_PORT |= LED_RED;
    else
        LED_PORT |= LED_YEL;

    _delay_ms(1000); // sekundowe opóźnienie
    }


    return 0;
}
