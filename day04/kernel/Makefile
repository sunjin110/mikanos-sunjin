TARGET = kernel.elf
OBJS = main.o

BASEDIR = $${HOME}/develop/mikanos-sunjin/day03/kernel/x86_64-elf
EDK2DIR = $${HOME}/develop/edk2

CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -std=c++17

CPPFLAGS = -I$(BASEDIR)/include/c++/v1 -I$(BASEDIR)/include -I$(BASEDIR)/include/freetype2
CPPFLAGS += -I$(EDK2DIR)/MdePkg/Include -I$(EDK2DIR)/MdePkg/Include/X64
CPPFLAGS += -nostdlibinc -D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS
CPPFLAGS += -DEFIAPI='__attribute__((ms_abi))'

LDFLAGS = -L$(BASEDIR)/lib
LDFLAGS += --entry KernelMain -z norelro --image-base 0x100000 --static -z separate-code

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o

kernel.elf: $(OBJS) Makefile 
	ld.lld $(LDFLAGS) -o kernel.elf $(OBJS)

%.o: %.cpp Makefile
	clang++ $(CPPFLAGS) $(CXXFLAGS) -c $<
