#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdarg>
#include "frame_buffer_config.hpp"
#include "graphics.hpp"
#include "font.hpp"
#include "console.hpp"
#include "mouse.hpp"
#include "pci.hpp"
#include "common.hpp"
#include "logger.hpp"

PixelWriter *NewPixelWriter(const FrameBufferConfig &config, char *buf) {
    switch (config.pixel_format) {
        case kPixelRGBResv8BitPerColor:
            return new(buf)RGBResv8BitPerColorPixelWriter(config);
        case kPixelBGRResv8BitPerColor:
            return new(buf)BGRResv8BitPerColorPixelWriter(config);
        default:
            Halt();
    }
}


// 問題になったら修正する
char console_buf[sizeof(Console)];
Console *console;

const PixelColor kDesktopBGColor{45, 118, 237};
const PixelColor kDesktopFGColor{255, 255, 255};

int printk(const char *format, ...) {
    va_list ap;
    int result = 0;
    char s[1024];

    va_start(ap, format);
    result = vsprintf(s, format, ap);
    va_end(ap);

    console->PutString(s);
    return result;
}



extern "C" void KernelMain(const FrameBufferConfig &frame_buffer_config) {
    // RGBもGBRも同じsizeなのでこれで問題ない
    char pixel_writer_class_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
    PixelWriter *pixel_writer = NewPixelWriter(frame_buffer_config, pixel_writer_class_buf);
    
    console = new(console_buf) Console{*pixel_writer, kDesktopFGColor, kDesktopBGColor};

    const int kFrameWidth = frame_buffer_config.horizontal_resolution;
    const int kFrameHeight = frame_buffer_config.vertical_resolution;

    FillRectangle(*pixel_writer, {0, 0}, {kFrameWidth, kFrameHeight -50}, kDesktopBGColor);
    FillRectangle(*pixel_writer, {0, kFrameHeight - 50}, {kFrameWidth, 50}, {1, 8, 17});
    FillRectangle(*pixel_writer, {0, kFrameHeight - 50}, {kFrameWidth / 5, 50}, {80, 80, 80});
    DrawRectangle(*pixel_writer, {10, kFrameHeight - 40}, {30, 30}, {160, 160, 160});

    printk("Welcome to MikanOS\n");
    SetLogLevel(kDebug);

    for (int dy = 0; dy < kMouseCursorHeight; dy++) {
        for (int dx = 0; dx < kMouseCursorWidth; dx++) {
            if (mouse_cursor_shape[dy][dx] == '@') {
                pixel_writer->Write(200 + dx, 100 + dy, {0, 0, 0});
            } else if (mouse_cursor_shape[dy][dx] == '.') {
                pixel_writer->Write(200 + dx, 100 + dy, {255, 255, 255});
            }
        }
    }

    // ShowDevice
    Error err = pci::ScanAllBus();
    printk("ScanAllBus: %s\n", err.Name());

    for (int i = 0; i < pci::num_device; i++) {
        const pci::Device& dev = pci::devices[i];
        uint16_t vendor_id = pci::ReadVendorId(dev.bus, dev.device, dev.function);
        pci::ClassCode class_code = pci::ReadClassCode(dev.bus, dev.device, dev.function);
        printk("%d.%d.%d: vend %04x, class %08x, head %02x\n",
            dev.bus, dev.device, dev.function,
            vendor_id, class_code, dev.header_type);
    }

    pci::ClassCode want_class_code_mouse = {
        .base = 0x0cu,
        .sub = 0x03u,
        .interface = 0x30u,
    };
    printk("want class code is %08u\n", want_class_code_mouse);


    // find mouse
    // intel製を先に探す xHC
    pci::Device *xhc_dev = nullptr;
    printk("want class_code base:%d, sub:%d, interface:%d\n", 0x0cu, 0x03u, 0x30u);
    for (int i = 0; i < pci::num_device; i++) {

        printk("got class_code base:%d, sub:%d, interface:%d\n", pci::devices[i].class_code.base,
        pci::devices[i].class_code.sub,pci::devices[i].class_code.interface);

        if (pci::devices[i].class_code.Match(0x0cu, 0x03u, 0x30u)) {
            xhc_dev = &pci::devices[i];
            if (0x8086 == pci::ReadVendorId(*xhc_dev)) {
                break;
            }
        }
    }
    if (xhc_dev) {
        Log(kInfo, "xHC has been found: %d.%d.%d\n",
            xhc_dev->bus, xhc_dev->device, xhc_dev->function);
    } else {
        Log(kInfo, "not found xHC\n");
    }

    Halt();
}

// ref: https://qiita.com/yamoridon/items/4905765cc6e4f320c9b5#52-%E5%88%86%E5%89%B2%E3%82%B3%E3%83%B3%E3%83%91%E3%82%A4%E3%83%AB%E3%81%AE%E8%A3%9C%E8%B6%B3
extern "C" void __cxa_pure_virtual() {
    while (1) __asm__("hlt");
}
