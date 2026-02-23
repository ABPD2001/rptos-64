#include "./uart.h"

void initialize_uart(u8_t uart_nth, u8_t crts_en)
{ // contrary to mini uart, uarts settings must be set before initialization.
    u64_t bases[5] = {UART0_BASE, UART2_BASE, UART3_BASE, UART4_BASE, UART5_BASE};
    volatile struct uart_settings_t *settings = __global_uart0_settings__ + uart_nth * 48;
    volatile struct uart_metadata_t *metadata = __global_uart0_metadata__ + uart_nth * 54;

    // set gpio funcationalities.

    gpfunction(settings->tx_gptable, settings->tx_gpnth, settings->tx_gpalt);
    gpfunction(settings->rx_gptable, settings->rx_gpnth, settings->rx_gpalt);
    if (crts_en)
    {
        gpfunction(settings->cts_gptable, settings->cts_gpnth, settings->cts_gpalt);
        gpfunction(settings->rts_gptable, settings->rts_gpnth, settings->rts_gpalt);
    }
}