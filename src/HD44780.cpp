/*
    Andrzej Zieliński
        - 2020 -
 */

#include "HD44780.hpp"

HD44780::HD44780()
{
    //ctor
}

void HD44780::init()
{
    all_to_out(); // ustawiamy wszytko jako wyjścia
    _delay_ms(15); // stabilizacja napięć
    LCD_RS_L; // i RS
    LCD_EN_L; // zerujemy linie EN
    LCD_WRITE; // LCD w trybie zapisu
    LCD_BL_H;
    for (uint8_t i = 0; i < 3; i++) // 3x blok instrukcji (wg noty katalogowej)
    {
        send_4b(0x03); // tryb 8 bit (wg noty)
        _delay_ms(5); // odczekać > 4.1 ms (nota)
    }
    send_4b(0x02); // tryb 4 bit
    _delay_ms(1);
    send_cmd(LCD_FUNCTION_SET | LCD_5x7 | LCD_2LINE | LCD_4BIT);
    send_cmd(LCD_ONOFF_MODE | LCD_OFF);
    send_cmd(LCD_ONOFF_MODE | LCD_ON | LCD_C_OFF | LCD_C_NOBLINK);
    send_cmd(LCD_ENTRY_MODE | LCD_EMSHIFT_C | LCD_EMRIGHT);
    send_cmd(LCD_CLEAR); // tak na wszelki czyścimy ;)

}


void HD44780::send_cmd(uint8_t data)
{
    LCD_RS_L; // ślemy komendę
    send_byte(data);
}

void HD44780::send_char(char ch)
{
    LCD_RS_H; // wysyłamy znak
    send_byte(ch);
}

void HD44780::go_to(uint8_t x, uint8_t l)
{
    send_cmd(0x80 | (x + (0x40 * l)));
}

void HD44780::send_text(char t[])
{
    while (*t)
    {
        send_char(*t++);
    }
}

void HD44780::send_text(char t1[], char t2[])
{
    s_clear();
    send_cmd(LCD_HOME);
    while (*t1)
    {
        send_char(*t1++);
    }
    go_to(0,1);
    while (*t2)
    {
        send_char(*t2++);
    }

}

void HD44780::s_clear()
{
    send_cmd(LCD_CLEAR);
}

////// PRIVATE

void HD44780::all_to_out() // wszystko jako wyjścia
{
    LCD_4_DDR |= LCD_4;
    LCD_5_DDR |= LCD_5;
    LCD_6_DDR |= LCD_6;
    LCD_7_DDR |= LCD_7;
    LCD_BL_DDR |= LCD_BL;
    LCD_EN_DDR |= LCD_EN;
    LCD_RS_DDR |= LCD_RS;
    LCD_RW_DDR |= LCD_RW;
}

void HD44780::send_byte(uint8_t data) // ślemy bajt
{
    send_4b(data >> 4); // 4 starsze
    send_4b(data); // i 4 młodsze
    //_delay_ms(2);
    status(); // czekamy na zwolnienie flagi
}


void HD44780::send_4b(uint8_t data) // ślemy 4 bity
{
    LCD_EN_H;

    LCD_4_PORT = (LCD_4_PORT & ~LCD_4) | (((data & 0x01) >> 0) << LCD_D4); // od najmłodszego bita
    LCD_5_PORT = (LCD_5_PORT & ~LCD_5) | (((data & 0x02) >> 1) << LCD_D5);
    LCD_6_PORT = (LCD_6_PORT & ~LCD_6) | (((data & 0x04) >> 2) << LCD_D6);
    LCD_7_PORT = (LCD_7_PORT & ~LCD_7) | (((data & 0x08) >> 3) << LCD_D7);

    LCD_EN_L; // zatwierdzamy stanem niskim
}

uint8_t HD44780::read_4b()
{
    uint8_t r4b = 0;

    LCD_EN_H;
    if (LCD_4_PIN & LCD_4) r4b |= (1 << 0);
    if (LCD_5_PIN & LCD_5) r4b |= (1 << 1);
    if (LCD_6_PIN & LCD_6) r4b |= (1 << 2);
    if (LCD_7_PIN & LCD_7) r4b |= (1 << 3);
    LCD_EN_L;
    return r4b;
}

uint8_t HD44780::read_byte()
{
    LCD_4_DDR &= ~LCD_4; // piny jako wejścia
    LCD_5_DDR &= ~LCD_5;
    LCD_6_DDR &= ~LCD_6;
    LCD_7_DDR &= ~LCD_7;
    LCD_READ; // tryb odczytu

    uint8_t r8b = 0;
    r8b = (read_4b() << 4); // czytamy 4 starsze
    r8b |= read_4b(); // czytam 4 młodsze

    LCD_7_DDR |= LCD_7; // wracamy do domyślnego trybu (wyjścia, tryp zapisu.. )
    LCD_6_DDR |= LCD_6;
    LCD_5_DDR |= LCD_5;
    LCD_4_DDR |= LCD_4;
    LCD_WRITE;

    return r8b;
}

uint8_t HD44780::r_bf()
{
    uint8_t bf;
    LCD_EN_H;
    if (LCD_7_PIN & LCD_7) bf = 1;
    else bf = 0;
    LCD_EN_L;
    return bf;
}

void HD44780::status()
{
    LCD_RS_L;

    while(read_byte() & 0x80); // najstarszy bit to flaga zajętości

    //return(read_byte() & 0x80); // najstarszy bit to flaga zajętości

}

