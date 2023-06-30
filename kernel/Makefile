TARGET = kernel.elf
OBJS = main.o graphics.o font.o common.o hankaku.o newlib_support.o console.o asmfunc.o pci.o
DEPENDS = $(join $(dir $(OBJS)),$(addprefix .,$(notdir $(OBJS:.o=.d))))

BASEDIR = $${HOME}/develop/mikanos-sunjin/kernel/x86_64-elf
EDK2DIR = $${HOME}/develop/edk2

CXXFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -std=c++17

CPPFLAGS = -I$(BASEDIR)/include/c++/v1 -I$(BASEDIR)/include -I$(BASEDIR)/include/freetype2
CPPFLAGS += -I$(EDK2DIR)/MdePkg/Include -I$(EDK2DIR)/MdePkg/Include/X64
CPPFLAGS += -nostdlibinc -D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS
CPPFLAGS += -DEFIAPI='__attribute__((ms_abi))'

CFLAGS += -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone

LDFLAGS = -L$(BASEDIR)/lib
LDFLAGS += --entry KernelMain -z norelro --image-base 0x100000 --static -z separate-code

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf *.bin

kernel.elf: $(OBJS) Makefile 
	ld.lld $(LDFLAGS) -o kernel.elf $(OBJS) -lc

%.o: %.cpp Makefile
	clang++ $(CPPFLAGS) $(CXXFLAGS) -c $<

.%.d: %.cpp
	clang++ $(CPPFLAGS) $(CXXFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.cpp=.o))
	# sed --in-place 's|$(notdir $(OBJ))|$(OBJ)|' $@
	# Mac
	sed -I '' -e 's|$(notdir $(OBJ))|$(OBJ)|' $@

# TODO cのcompileもかく

%.o: %.c Makefile
	clang $(CPPFLAGS) $(CFLAGS) -c $<

.%.d: %.c
	clang $(CPPFLAGS) $(CFLAGS) -MM $< > $@
	$(eval OBJ = $(<:.c=.o))
	sed -I '' -e 's|$(notdir $(OBJ))|$(OBJ)|' $@

%.o: %.asm Makefile
	nasm -f elf64 -o $@ $<

.%.d: %.bin
	touch $@

.PHONY: depends
depends:
	$(MAKE) $(DEPENDS)

-include $(DEPENDS)

hankaku.bin: resource/hankaku.txt
	python3 ../tools/makefont.py -o $@ $<

hankaku.o: hankaku.bin
	objcopy -I binary -O elf64-x86-64 -B i386:x86-64 $< $@
