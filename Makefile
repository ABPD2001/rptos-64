compile:
	aarch64-none-elf-gcc -fno-builtin -fno-stack-protector -ffreestanding -mgeneral-regs-only -mcpu=cortex-a72 -c *.c ./*.s ./*/*.c ./*/*/*.c -o compile.o

assemble:
	aarch64-none-elf-gcc -c *.s ./*.s ./*/*.s ./*/*/*.s -o assemble.o

link:
	aarch64-none-elf-ld -T linker.ld assemble.o compile.o -o kernel8.elf

build:
	aarch64-none-elf-gcc -fno-builtin -fno-stack-protector -ffreestanding -mgeneral-regs-only -mcpu=cortex-a72 -c *.c ./*.s ./*/*.c ./*/*/*.c -o compile.o
	echo "compile complete."
	aarch64-none-elf-gcc -c *.s ./*.s ./*/*.s ./*/*/*.s -o assemble.o
	echo "assemble complete."
	aarch64-none-elf-ld -T linker.ld assemble.o compile.o -o kernel8.elf
	echo "linking complete."

clean:
	echo "removing every output file..."
	rm -rf *.o ./*.o ./*/*.o ./*/*/*.o kernel8.elf
	echo "done."