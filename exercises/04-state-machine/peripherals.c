# include "peripherals.h"

int gpp_make_input(GpioPer* per, uint8_t pin) {
    per->direction &= ~(1 << pin);
    return 0;
}

int gpp_make_output(GpioPer* per, uint8_t pin) {
    per->direction |= (1 << pin);
    return 0;
}

int gpp_get(GpioPer* per, uint8_t pin, uint8_t* val) {
    *val = (per->data_in >> pin) & 1;
    return 0;
}

int gpp_set(GpioPer* per, uint8_t pin, uint8_t val) {
    per->data_out = (per->data_out & ~(1 << pin)) | (val << pin);
    return 0;
}

int srp_init(SerialPer* per, uint32_t baud_rate, uint32_t bit_count) {
    if (bit_count < 5 || bit_count > 8) {
        return -1;
    }

    per->baud_rate = baud_rate;
    per->bit_count = bit_count;
    per->rx_enable = 1;
    per->tx_enable = 1;

    return 0;
}

int srp_read_blocking(SerialPer* per, uint8_t* data) {
    while (per->rx_available_count == 0) {}
    *data = (uint8_t)per->rx_data;
    return 0;
}

int srp_write_blocking(SerialPer* per, uint8_t data) {
    while (!per->tx_ready) {}
    per->tx_data = data;
    return 0;
}

int adp_init(AdcPer* per) {
    per->enable = 1;
    return 0;
}

int adp_read(AdcPer* per, uint16_t* data) {
    *data = per->value;
    return 0;
}
