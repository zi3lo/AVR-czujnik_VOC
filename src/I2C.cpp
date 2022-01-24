/*
    Andrzej Zieliński
        - 2020 -
 */

#include "I2C.hpp"


I2C::I2C(uint8_t adr)
{
    addr = adr;
}

void I2C::start()
{
    //TWSR |= (0 << TWPS1) | (0 << TWPS0)                       // prescaler = 1     /////SCL_f =~ 56kHz
    TWBR = 1;                                                   // SCL_f = CPU_f / (16 + 2(TWBR) * Prescaler
    TWCR = (1 << TWINT) | (1 << TWSTA ) | (1 << TWEN);          // ślemy start
    while (!(TWCR & (1 << TWINT)));
}

void I2C::sla_w()
{
    uint8_t slaw = addr << 1;                                   // miejsce na 0 - zapis
    TWDR = slaw;                                                // adres + W - write
    TWCR = (1 << TWINT) | (1 << TWEN);                          // czyścimy twin i ślemy adres
    while (!(TWCR & (1 << TWINT)));                             // czekamy na wysłanie i odebranie bitu ACK/NACK
}

void I2C::sla_r()
{
    uint8_t slar = addr << 1;                                        // miejsce na 1 - odczyt
    slar |= 0x01;
    TWDR = slar;                                                // adres + R - read
    TWCR = (1 << TWINT) | (1 << TWEN);                          // czyścimy twin i ślemy adres
    while (!(TWCR & (1 << TWINT)));                             // czekamy na wysłanie i odebranie bitu ACK/NACK
}

void I2C::send(uint8_t data)
{
    TWDR = data;                                                // dane
    TWCR = (1 << TWINT) | (1 << TWEN);                          // czyścimy twin i ślemy dane
    while (!(TWCR & (1 << TWINT)));                             // czekamy na wysłanie i odebranie bitu ACK/NACK
}

uint8_t I2C::get_ack()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);            // czytamy
    while (!(TWCR & (1 << TWINT)));                             // czekamy na ack
    return TWDR;
}

uint8_t I2C::get_nack()
{
    TWCR = (1 << TWINT) | (1 << TWEN);                          // czytamy
    while (!(TWCR & (1 << TWINT)));                             // czekamy na ack
    return TWDR;
}


void I2C::stop()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

