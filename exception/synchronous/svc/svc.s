.section svc_table
.org 0x00, b svc_muart_write
.org 0x04, b svc_muart_read
.org 0x08, b svc_muart_write_char
.org 0x0C, b svc_muart_read_char
.org 0x10, b svc_muart_settings
.org 0x14, b svc_muart_availablity
.org 0x18, b svc_get_task_id
.org 0x20, b svc_tsleep_ms
.org 0x24, b svc_termination_request
.org 0x28, b svc_gpalloc
.org 0x2C, b svc_gpfree
.org 0x30, b svc_gpset
.org 0x34, b svc_gpclear
.org 0x38, b svc_gpvalue
.org 0x3C, b svc_create_ipcmailbox
.org 0x40, b svc_write_ipcmailbox
.org 0x44, b svc_read_ipcmailbox
.ltorg