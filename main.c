#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define BAUD_RATE 9600

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define PACKET_SIZE_MIN 1
#define PACKET_SIZE_MAX 5
#define INTERVAL_MS 1000

#define UART_FIFO_SIZE 4

static void print_packet(const char *prefix, uint8_t *packet, uint packet_size) {
    if (packet_size > 0) {
        // Prefix + 2 characters per packet byte + whitespace + '\0'
        char text[strlen(prefix) + packet_size * 2 + 2];
        char *p = text;

        for (uint i = 0; i < packet_size; i++) {
            p += sprintf(p, "%02hhX", packet[i]);
        }

        printf("%s%s ", prefix, text);
    }
}

static void on_uart_rx() {
    static uint8_t data[UART_FIFO_SIZE];

    uint count = 0;

    while (count < UART_FIFO_SIZE && uart_is_readable(uart0)) {
        data[count++] = uart_getc(uart0);
    }

    print_packet("<< ", data, count);
}

int main() {
    stdio_usb_init();

    uart_init(uart0, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(uart0, true, false);

    uint8_t packet[PACKET_SIZE_MAX];

    for (uint i = 0; i < PACKET_SIZE_MAX; i++) {
        packet[i] = i % 256;
    }

    while (true) {
        for (uint packet_size = PACKET_SIZE_MIN; packet_size <= PACKET_SIZE_MAX; packet_size++) {
            print_packet(">> ", packet, packet_size);

            for (uint i = 0; i < packet_size; i++) {
                uart_putc_raw(uart0, packet[i]);
            }

            sleep_ms(INTERVAL_MS);
        }

        tight_loop_contents();
    }
}
