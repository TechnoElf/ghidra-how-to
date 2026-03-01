#include "peripherals.h"

int puts(const char* str) {
    while (*str) {
        srp_write_blocking(SRP0, *str++);
    }
    return 0;
}

int puti(int32_t value) {
    char buffer[12];
    int i = 0;

    if (value < 0) {
        buffer[i++] = '-';
        value = -value;
    }

    if (value == 0) {
        buffer[i++] = '0';
    } else {
        while (value > 0) {
            buffer[i++] = '0' + (value % 10);
            value /= 10;
        }
    }

    while (i > 0) {
        srp_write_blocking(SRP0, buffer[--i]);
    }
    return 0;
}

void control(uint32_t target, uint8_t debug) {
    int32_t integral = 0;
    int32_t prev_error = 0;

    const int32_t kp = 10;
    const int32_t ki = 1;
    const int32_t kd = 5;
    const int32_t scale = 100;

    while (1) {
        uint16_t measured;
        adp_read(ADP0, &measured);

        int32_t error = (int32_t)target - (int32_t)measured;
        integral += error;
        int32_t derivative = error - prev_error;
        prev_error = error;

        int32_t output = (kp * error + ki * integral + kd * derivative) / scale;

        if (output > 0) {
            gpp_set(GPP0, 0, 1);
        } else {
            gpp_set(GPP0, 0, 0);
        }

        if (debug) {
            puts("Measured: ");
            puti(measured);
            puts("\n");
        }
    }
}

int main(void) {
    srp_init(SRP0, 9600, 8);
    adp_init(ADP0);
    gpp_make_output(GPP0, 0);

    puts("Starting controller...\n");

    control(42, 0);
    return 0;
}
