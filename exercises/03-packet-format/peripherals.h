#pragma once

#include <stdint.h>

typedef struct GpioPer {
    volatile uint32_t direction; // Direction configuration (0: input, 1: output)
    volatile uint32_t data_out; // Data to be written to the output pins
    volatile uint32_t data_in; // Data read from the input pins
} GpioPer;

#define GPP0_BASE_ADDRESS 0x10000000
#define GPP0 ((GpioPer*)GPP0_BASE_ADDRESS)
#define GPP1_BASE_ADDRESS 0x10000100
#define GPP1 ((GpioPer*)GPP1_BASE_ADDRESS)
#define GPP2_BASE_ADDRESS 0x10000200
#define GPP2 ((GpioPer*)GPP2_BASE_ADDRESS)
#define GPP3_BASE_ADDRESS 0x10000300
#define GPP3 ((GpioPer*)GPP3_BASE_ADDRESS)

int gpp_make_input(GpioPer* per, uint8_t pin);
int gpp_make_output(GpioPer* per, uint8_t pin);
int gpp_get(GpioPer* per, uint8_t pin, uint8_t* val);
int gpp_set(GpioPer* per, uint8_t pin, uint8_t val);

typedef struct SerialPer {
    volatile uint32_t baud_rate; // Baud rate configuration
    volatile uint32_t bit_count; // Number of data bits (5-8)
    volatile uint32_t rx_data; // Received data, fed by a FIFO
    volatile uint32_t rx_available_count; // Number of bytes available in the receive FIFO
    volatile uint32_t rx_enable; // Enable/disable the receiver
    volatile uint32_t tx_data; // Data to be written to the transmit FIFO
    volatile uint32_t tx_ready; // Flag indicating if the transmitter FIFO is ready to accept data
    volatile uint32_t tx_enable; // Enable/disable the transmitter
} SerialPer;

#define SRP0_BASE_ADDRESS 0x10001000
#define SRP0 ((SerialPer*)SRP0_BASE_ADDRESS)
#define SRP1_BASE_ADDRESS 0x10001100
#define SRP1 ((SerialPer*)SRP1_BASE_ADDRESS)
#define SRP2_BASE_ADDRESS 0x10001200
#define SRP2 ((SerialPer*)SRP2_BASE_ADDRESS)
#define SRP3_BASE_ADDRESS 0x10001300
#define SRP3 ((SerialPer*)SRP3_BASE_ADDRESS)

int srp_init(SerialPer* per, uint32_t baud_rate, uint32_t bit_count);
int srp_read_blocking(SerialPer* per, uint8_t* data);
int srp_write_blocking(SerialPer* per, uint8_t data);

typedef struct AdcPer {
    volatile uint32_t enable; // Enable/disable the ADC
    volatile uint32_t value; // ADC result value
} AdcPer;

#define ADP0_BASE_ADDRESS 0x10002000
#define ADP0 ((AdcPer*)ADP0_BASE_ADDRESS)
#define ADP1_BASE_ADDRESS 0x10002100
#define ADP1 ((AdcPer*)ADP1_BASE_ADDRESS)
#define ADP2_BASE_ADDRESS 0x10002200
#define ADP2 ((AdcPer*)ADP2_BASE_ADDRESS)
#define ADP3_BASE_ADDRESS 0x10002300
#define ADP3 ((AdcPer*)ADP3_BASE_ADDRESS)

int adp_init(AdcPer* per);
int adp_read(AdcPer* per, uint16_t* data);
