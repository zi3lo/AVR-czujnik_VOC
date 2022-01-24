/*
    Andrzej Zieliński
        - 2020 -
 */

#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

class I2C
{
    public:
        I2C(uint8_t);
        void start();
        void sla_w();
        void sla_r();
        void send(uint8_t);
        uint8_t get_ack();
        uint8_t get_nack();
        void stop();

    private:
        uint8_t addr;
};

#endif // I2C_H
