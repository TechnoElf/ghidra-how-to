#include "peripherals.h"

/* ── tiny helpers (no libc) ─────────────────────────────────────────── */

static void debug_puts(const char* s) {
    while (*s) srp_write_blocking(SRP0, *s++);
}

static void debug_hex8(uint8_t v) {
    const char hex[] = "0123456789ABCDEF";
    srp_write_blocking(SRP0, hex[v >> 4]);
    srp_write_blocking(SRP0, hex[v & 0x0F]);
}

static void debug_hex16(uint16_t v) {
    debug_hex8(v >> 8);
    debug_hex8(v & 0xFF);
}

/* ── CRC-8 (poly 0x07, init 0xFF) ──────────────────────────────────── */

static uint8_t crc8(const uint8_t* buf, uint8_t len) {
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= buf[i];
        for (uint8_t b = 0; b < 8; b++)
            crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
    }
    return crc;
}

/* ── XOR obfuscation key (rotating) ─────────────────────────────────── */

static const uint8_t xor_key[8] = {
    0xA5, 0x3C, 0x96, 0x5A, 0xF0, 0x1E, 0x78, 0xC3
};

static void xor_buf(uint8_t* buf, uint8_t len, uint8_t key_offset) {
    for (uint8_t i = 0; i < len; i++)
        buf[i] ^= xor_key[(key_offset + i) & 7];
}

/* ── protocol definitions ───────────────────────────────────────────── */

#define SYNC_BYTE     0x7E
#define PROTO_VERSION 0x02

#define CMD_PING          0x01
#define CMD_GPIO_READ     0x10
#define CMD_GPIO_WRITE    0x11
#define CMD_GPIO_DIR      0x12
#define CMD_ADC_READ      0x20
#define CMD_SERIAL_SEND   0x30
#define CMD_STATUS        0x40
#define CMD_RESET_CTR     0xF0

#define RESP_ACK   0x00
#define RESP_NAK   0x01
#define RESP_DATA  0x02
#define RESP_ERR   0xFF

#define MAX_PAYLOAD 16

typedef struct __attribute__((packed)) {
    uint8_t sync;
    uint8_t version;
    uint8_t seq;
    uint8_t cmd;
    uint8_t flags;
    uint8_t payload_len;
    uint8_t payload[MAX_PAYLOAD];
    uint8_t crc;
} Packet;

/* flags field bits */
#define FLAG_ENCRYPTED  (1 << 0)
#define FLAG_RESPONSE   (1 << 1)
#define FLAG_PRIORITY   (1 << 2)
#define FLAG_FRAGMENT   (1 << 3)

/* ── device state ───────────────────────────────────────────────────── */

static uint8_t  tx_seq = 0;
static uint8_t  rx_seq_expected = 0;
static uint32_t rx_good_count = 0;
static uint32_t rx_bad_count = 0;
static uint16_t uptime_ticks = 0;

/* ── serial I/O for protocol packets ────────────────────────────────── */

static void send_raw(const uint8_t* buf, uint8_t len) {
    for (uint8_t i = 0; i < len; i++)
        srp_write_blocking(SRP1, buf[i]);
}

static void recv_raw(uint8_t* buf, uint8_t len) {
    for (uint8_t i = 0; i < len; i++)
        srp_read_blocking(SRP1, &buf[i]);
}

/* ── build and send a response packet ───────────────────────────────── */

static void send_response(uint8_t cmd, uint8_t resp_code,
                          const uint8_t* data, uint8_t data_len) {
    Packet pkt;
    pkt.sync = SYNC_BYTE;
    pkt.version = PROTO_VERSION;
    pkt.seq = tx_seq++;
    pkt.cmd = cmd;
    pkt.flags = FLAG_RESPONSE;
    pkt.payload_len = data_len + 1;
    pkt.payload[0] = resp_code;

    for (uint8_t i = 0; i < data_len && i < MAX_PAYLOAD - 1; i++)
        pkt.payload[1 + i] = data[i];

    /* obfuscate payload */
    xor_buf(pkt.payload, pkt.payload_len, pkt.seq);
    pkt.flags |= FLAG_ENCRYPTED;

    /* CRC covers everything from version through payload */
    uint8_t crc_len = 4 + pkt.payload_len; /* version+seq+cmd+flags+payload_len+payload */
    crc_len += 1; /* payload_len field itself */
    pkt.crc = crc8(&pkt.version, crc_len);

    uint8_t total = 1 + crc_len + 1; /* sync + (version..payload) + crc */
    send_raw((uint8_t*)&pkt, total);
}

/* ── GPIO helpers (peripherals indexed 0-3) ─────────────────────────── */

static GpioPer* gpio_by_index(uint8_t idx) {
    switch (idx) {
        case 0: return GPP0;
        case 1: return GPP1;
        case 2: return GPP2;
        case 3: return GPP3;
        default: return 0;
    }
}

static AdcPer* adc_by_index(uint8_t idx) {
    switch (idx) {
        case 0: return ADP0;
        case 1: return ADP1;
        case 2: return ADP2;
        case 3: return ADP3;
        default: return 0;
    }
}

static SerialPer* serial_by_index(uint8_t idx) {
    switch (idx) {
        case 2: return SRP2;
        case 3: return SRP3;
        default: return 0;  /* 0 and 1 are reserved */
    }
}

/* ── command handlers ───────────────────────────────────────────────── */

static void handle_ping(void) {
    uint8_t data[4];
    data[0] = (uint8_t)(uptime_ticks >> 8);
    data[1] = (uint8_t)(uptime_ticks);
    data[2] = (uint8_t)(rx_good_count);
    data[3] = (uint8_t)(rx_bad_count);
    send_response(CMD_PING, RESP_ACK, data, 4);
}

/*
 * GPIO_READ payload: [port_index, pin]
 * Response: [value]
 */
static void handle_gpio_read(const uint8_t* payload, uint8_t len) {
    if (len < 2) { send_response(CMD_GPIO_READ, RESP_ERR, 0, 0); return; }
    GpioPer* port = gpio_by_index(payload[0]);
    if (!port) { send_response(CMD_GPIO_READ, RESP_ERR, 0, 0); return; }
    uint8_t val;
    gpp_get(port, payload[1], &val);
    send_response(CMD_GPIO_READ, RESP_DATA, &val, 1);
}

/*
 * GPIO_WRITE payload: [port_index, pin, value]
 */
static void handle_gpio_write(const uint8_t* payload, uint8_t len) {
    if (len < 3) { send_response(CMD_GPIO_WRITE, RESP_ERR, 0, 0); return; }
    GpioPer* port = gpio_by_index(payload[0]);
    if (!port) { send_response(CMD_GPIO_WRITE, RESP_ERR, 0, 0); return; }
    gpp_set(port, payload[1], payload[2]);
    send_response(CMD_GPIO_WRITE, RESP_ACK, 0, 0);
}

/*
 * GPIO_DIR payload: [port_index, pin, direction (0=in, 1=out)]
 */
static void handle_gpio_dir(const uint8_t* payload, uint8_t len) {
    if (len < 3) { send_response(CMD_GPIO_DIR, RESP_ERR, 0, 0); return; }
    GpioPer* port = gpio_by_index(payload[0]);
    if (!port) { send_response(CMD_GPIO_DIR, RESP_ERR, 0, 0); return; }
    if (payload[2])
        gpp_make_output(port, payload[1]);
    else
        gpp_make_input(port, payload[1]);
    send_response(CMD_GPIO_DIR, RESP_ACK, 0, 0);
}

/*
 * ADC_READ payload: [adc_index]
 * Response: [value_hi, value_lo]
 */
static void handle_adc_read(const uint8_t* payload, uint8_t len) {
    if (len < 1) { send_response(CMD_ADC_READ, RESP_ERR, 0, 0); return; }
    AdcPer* adc = adc_by_index(payload[0]);
    if (!adc) { send_response(CMD_ADC_READ, RESP_ERR, 0, 0); return; }
    uint16_t val;
    adp_read(adc, &val);
    uint8_t data[2] = { val >> 8, val & 0xFF };
    send_response(CMD_ADC_READ, RESP_DATA, data, 2);
}

/*
 * SERIAL_SEND payload: [serial_index, len, data...]
 */
static void handle_serial_send(const uint8_t* payload, uint8_t len) {
    if (len < 2) { send_response(CMD_SERIAL_SEND, RESP_ERR, 0, 0); return; }
    SerialPer* ser = serial_by_index(payload[0]);
    if (!ser) { send_response(CMD_SERIAL_SEND, RESP_ERR, 0, 0); return; }
    uint8_t data_len = payload[1];
    if (data_len + 2 > len) { send_response(CMD_SERIAL_SEND, RESP_ERR, 0, 0); return; }
    for (uint8_t i = 0; i < data_len; i++)
        srp_write_blocking(ser, payload[2 + i]);
    send_response(CMD_SERIAL_SEND, RESP_ACK, 0, 0);
}

/*
 * STATUS: no payload
 * Response: [version, uptime_hi, uptime_lo, rx_good_hi, rx_good_lo,
 *            rx_bad_hi, rx_bad_lo, tx_seq, rx_seq_expected]
 */
static void handle_status(void) {
    uint8_t data[9];
    data[0] = PROTO_VERSION;
    data[1] = (uint8_t)(uptime_ticks >> 8);
    data[2] = (uint8_t)(uptime_ticks);
    data[3] = (uint8_t)(rx_good_count >> 8);
    data[4] = (uint8_t)(rx_good_count);
    data[5] = (uint8_t)(rx_bad_count >> 8);
    data[6] = (uint8_t)(rx_bad_count);
    data[7] = tx_seq;
    data[8] = rx_seq_expected;
    send_response(CMD_STATUS, RESP_DATA, data, 9);
}

static void handle_reset_ctr(void) {
    rx_good_count = 0;
    rx_bad_count = 0;
    tx_seq = 0;
    rx_seq_expected = 0;
    uptime_ticks = 0;
    send_response(CMD_RESET_CTR, RESP_ACK, 0, 0);
}

/* ── main packet receive & dispatch loop ────────────────────────────── */

static int receive_packet(Packet* pkt) {
    /* wait for sync byte */
    uint8_t b;
    do {
        srp_read_blocking(SRP1, &b);
    } while (b != SYNC_BYTE);
    pkt->sync = b;

    /* read header */
    recv_raw(&pkt->version, 5); /* version, seq, cmd, flags, payload_len */

    if (pkt->version != PROTO_VERSION)
        return -1;
    if (pkt->payload_len > MAX_PAYLOAD)
        return -2;

    /* read payload + crc */
    recv_raw(pkt->payload, pkt->payload_len);
    srp_read_blocking(SRP1, &pkt->crc);

    /* verify CRC */
    uint8_t crc_len = 5 + pkt->payload_len;
    uint8_t expected_crc = crc8(&pkt->version, crc_len);
    if (pkt->crc != expected_crc)
        return -3;

    /* decrypt payload if needed */
    if (pkt->flags & FLAG_ENCRYPTED)
        xor_buf(pkt->payload, pkt->payload_len, pkt->seq);

    /* verify sequence number */
    if (pkt->seq != rx_seq_expected)
        return -4;
    rx_seq_expected++;

    return 0;
}

static void dispatch(Packet* pkt) {
    switch (pkt->cmd) {
        case CMD_PING:
            handle_ping();
            break;
        case CMD_GPIO_READ:
            handle_gpio_read(pkt->payload, pkt->payload_len);
            break;
        case CMD_GPIO_WRITE:
            handle_gpio_write(pkt->payload, pkt->payload_len);
            break;
        case CMD_GPIO_DIR:
            handle_gpio_dir(pkt->payload, pkt->payload_len);
            break;
        case CMD_ADC_READ:
            handle_adc_read(pkt->payload, pkt->payload_len);
            break;
        case CMD_SERIAL_SEND:
            handle_serial_send(pkt->payload, pkt->payload_len);
            break;
        case CMD_STATUS:
            handle_status();
            break;
        case CMD_RESET_CTR:
            handle_reset_ctr();
            break;
        default:
            send_response(pkt->cmd, RESP_NAK, 0, 0);
            break;
    }
}

/* ── entry point ────────────────────────────────────────────────────── */

int main(void) {
    /* init debug serial */
    srp_init(SRP0, 9600, 8);
    /* init protocol transceiver */
    srp_init(SRP1, 115200, 8);
    /* init extra serials */
    srp_init(SRP2, 9600, 8);
    srp_init(SRP3, 9600, 8);
    /* init ADCs */
    adp_init(ADP0);
    adp_init(ADP1);
    adp_init(ADP2);
    adp_init(ADP3);

    debug_puts("BOOT OK v");
    debug_hex8(PROTO_VERSION);
    debug_puts("\r\n");

    Packet pkt;

    while (1) {
        int rc = receive_packet(&pkt);
        uptime_ticks++;

        if (rc == 0) {
            rx_good_count++;
            dispatch(&pkt);
        } else {
            rx_bad_count++;
            debug_puts("ERR ");
            debug_hex16((uint16_t)(-(int16_t)rc));
            debug_puts(" SEQ=");
            debug_hex8(pkt.seq);
            debug_puts("\r\n");
            send_response(pkt.cmd, RESP_NAK, 0, 0);
        }
    }

    return 0;
}
