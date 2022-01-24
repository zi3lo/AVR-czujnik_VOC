/*
    Andrzej Zieliński
        - 2020 -
 */

#ifndef CCS811_H
#define CCS811_H

#include <avr/io.h>
#include "I2C.hpp"

//// edytować w razie potrzeby
#define CCS811_ADDR 0x5a        // gdy pin adresu na low 0x5a, gdy high 0x5b

#define MODE_REG 0x01
    #define DRIVE_MODE 1        // 3 bity - 0 - idle, 1 - co 1s, 2 - co 10s, 3 - co 60s, 4 - co 250ms (wynik tylko w raw)
    #define INT_DATARDY 1       // pin nINT na low, kiedy próbka gotowa - 0 off,
    #define INT_THRESH 0        // jeśli 1 to nINT tylko po przekroczeniu zadanych poziomów
//////////////////////////////

#define STATUS_REG 0x00         // rejestr statusu
    #define FW_MODE (1 << 7)
    #define APP_VALID (1 << 4)
    #define DATA_READY (1 << 3)
    #define ERROR (1 << 0)

#define ID_REG  0x20            // rejestr zawierający ID
#define APP_START_REG 0xf4      // rejestr startu
#define ALG_RESULT_DATA 0x02    // rejestr wyników pomiaru - 8 bajtów. 2 pierwsze - CO2, 3i4 - TVOC





class CCS811
{
    public:
        CCS811();
        uint8_t init();
        void get_data();                    // odczyt danych (bez względu na flagę statusu)
        uint8_t data_status();              // flaga statusu - 1 gdy nowe dane, 0 gdy brak
        uint16_t co2();                     // odczyt stężenia co2 w ppm
        uint16_t tvoc();                    // odczyt TVOC w ppm
        void comp_val(uint16_t, int16_t);   // zapis wilgotności i temperatury w celu poprawy pomiarów

    private:
        uint32_t read_data();                 // czyta 4 bajty z danymi
        uint16_t co2_;
        uint16_t tvoc_;
        uint8_t read_u8b(uint8_t);
        void write_u8b(uint8_t, uint8_t);   // rejestr i dane
        void write_u32b(uint8_t, uint32_t);
        void write_only(uint8_t);           // samo write bez danych (dla startu)
        I2C ccs;

};

#endif // CCS811_H
