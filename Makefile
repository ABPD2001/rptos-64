# تعریف متغیرها برای تغییر راحت در آینده
CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

# فلگ‌های کامپایلر مخصوص Cortex-A72 (RPi4)
CFLAGS = -fno-builtin -fno-stack-protector -ffreestanding -mgeneral-regs-only -mcpu=cortex-a72 -Wall -O2

# پیدا کردن تمام فایل‌های سورس در زیرپوشه‌ها
C_SOURCES = $(shell find . -name "*.c")
S_SOURCES = $(shell find . -name "*.s")
# تبدیل نام سورس‌ها به نام فایل‌های آبجکت (.o)
OBJECTS = $(C_SOURCES:.c=.o) $(S_SOURCES:.s=.o)

build: $(OBJECTS)
	@echo "Linking..."
	$(LD) -T linker.ld $(OBJECTS) -o kernel8.elf
	@echo "Creating kernel8.img..."
	$(OBJCOPY) -O binary kernel8.elf kernel8.img
	@echo "Build complete! kernel8.img is ready."

# قانون برای کامپایل فایل‌های C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# قانون برای اسمبل فایل‌های S
%.o: %.s
	$(CC) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -f $(shell find . -name "*.o") kernel8.elf kernel8.img
	@echo "Done."