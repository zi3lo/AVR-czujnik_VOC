/*
    Andrzej Zieliński
        - 2020 -
 */

#ifndef HD44780_H
#define HD44780_H

#include <avr/io.h>
#include <util/delay.h>

//////// DEFINE DO EDYCJI - PORTY

#define LCD_EN_DDR DDRD
#define LCD_EN_PORT PORTD
#define LCD_EN_PIN PIND
#define LCD_EN (1 << PD5)

#define LCD_RW_DDR DDRD
#define LCD_RW_PORT PORTD
#define LCD_RW_PIN PIND
#define LCD_RW (1 << PD6)

#define LCD_RS_DDR DDRD
#define LCD_RS_PORT PORTD
#define LCD_RS_PIN PIND
#define LCD_RS (1 << PD7)

#define LCD_BL_DDR DDRD
#define LCD_BL_PORT PORTD
#define LCD_BL (1 << PD0)

#define LCD_4_DDR DDRD
#define LCD_4_PORT PORTD
#define LCD_4_PIN PIND
#define LCD_D4 PD4

#define LCD_5_DDR DDRD
#define LCD_5_PORT PORTD
#define LCD_5_PIN PIND
#define LCD_D5 PD3

#define LCD_6_DDR DDRD
#define LCD_6_PORT PORTD
#define LCD_6_PIN PIND
#define LCD_D6 PD2

#define LCD_7_DDR DDRD
#define LCD_7_PORT PORTD
#define LCD_7_PIN PIND
#define LCD_D7 PD1

//////////////////////

#define LCD_4 (1 << LCD_D4)
#define LCD_5 (1 << LCD_D5)
#define LCD_6 (1 << LCD_D6)
#define LCD_7 (1 << LCD_D7)

#define LCD_EN_H LCD_EN_PORT |= LCD_EN
#define LCD_EN_L LCD_EN_PORT &= ~LCD_EN
#define LCD_RS_H LCD_RS_PORT |= LCD_RS
#define LCD_RS_L LCD_RS_PORT &= ~LCD_RS
#define LCD_BL_H LCD_RS_PORT |= LCD_BL
#define LCD_BL_L LCD_RS_PORT &= ~LCD_BL
#define LCD_READ LCD_RW_PORT |= LCD_RW
#define LCD_WRITE LCD_RW_PORT &= ~LCD_RW

/////////// TRYBY WYŚWIETLACZA ///////////

#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ENTRY_MODE 0x04
    #define LCD_EMSHIFT_C 0 // przesuw kursora
    #define LCD_EMSHIFT_D 1 // przesuw ekranu
    #define LCD_EMLEFT 0 // przesuw w lewo
    #define LCD_EMRIGHT 2 // przesuw w prawo
#define LCD_ONOFF_MODE 0x8
    #define LCD_OFF 0 // display off
    #define LCD_ON 4 // zgadnij :D
    #define LCD_C_OFF 0 // kursor off
    #define LCD_C_ON 2 // xd
    #define LCD_C_BLINK 1 // mruganie
    #define LCD_C_NOBLINK 0 // nie mruganie --- w zasadzie te 0 można odpuścić
#define LCD_DISPLAY_SHIFT 0x10 // jak entry, tylko bez zapisu
    #define LCD_SHIFT_C 0
    #define LCD_SHIFT_D 8
    #define LCD_LEFT 0
    #define LCD_RIGHT 4
#define LCD_FUNCTION_SET 0x20 // funkcje
    #define LCD_5x7 0 // znak 5x7
    #define LCD_5x10 4
    #define LCD_1LINE 0 // wyświetlacz jedno liniowy
    #define LCD_2LINE 8
    #define LCD_4BIT 0 // 4 linie danych
    #define LCD_8BIT 16 // 8 linii danych


class HD44780
{
    public:
        HD44780();
        void init();
        void send_text(char [], char []);
        void send_text(char []);
        void send_char(char);
        void send_cmd(uint8_t);
        void go_to(uint8_t,uint8_t);
        void s_clear();

    private:
        void all_to_out();
        void send_4b(uint8_t);
        void send_byte(uint8_t);
        void status();
        uint8_t r_bf(); // read busy flag
        uint8_t read_byte();
        uint8_t read_4b();
};

#endif // HD44780_H
