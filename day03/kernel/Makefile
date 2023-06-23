build:
	. ./buildenv.sh && echo $${CPPFLAGS}
	. ./buildenv.sh && clang++ $${CPPFLAGS} -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -std=c++17 -c main.cpp
	# ld.lld ${LDFLAGS} --entry KernelMain -z norelro --image-base 0x100000 --static -o kernel.elf main.o
	. ./buildenv.sh && ld.lld $${LDFLAGS} --entry KernelMain -z norelro --image-base 0x100000 --static -o kernel.elf -z separate-code main.o

init:
	wget https://github.com/uchan-nos/mikanos-build/releases/download/v2.0/x86_64-elf.tar.gz
	tar -zxvf x86_64-elf.tar.gz
	rm x86_64-elf.tar.gz
