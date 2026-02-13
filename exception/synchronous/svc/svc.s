.section svc_table
.org 0x00, b svc_mini_uart_write
.org 0x04, b svc_mini_uart_read
.org 0x08, b svc_mini_uart_write_char
.org 0x0C, b svc_mini_uart_read_char
.org 0x10, b svc_mini_uart_settings
.org 0x14, b svc_mini_uart_availablity
.org 0x18, b svc_get_task_id
.org 0x20, b svc_tsleep_ms
.org 0x24, b svc_termination_request
.ltorg

@ .section svc_handlers
@ .equiv AUX_MU_BASE,#0x7E215000
@ .equiv AUX_ENABLES,#0x04
@ .equiv AUX_MU_IO_REG,#0x40