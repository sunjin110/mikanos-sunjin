#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdarg>
#include "frame_buffer_config.hpp"
#include "graphics.hpp"
#include "font.hpp"
#include "common.hpp"
#include "console.hpp"

// 問題になったら修正する
char console_buf[sizeof(Console)];
Console *console;

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
    
    // 白で塗りつぶす
    for (int x = 0; x < frame_buffer_config.horizontal_resolution; x++) {
        for (int y = 0; y < frame_buffer_config.vertical_resolution; y++) {
            pixel_writer->Write(x, y, PixelColor{.red = 255, .green = 255, .blue = 255});
        }
    }

    console = new(console_buf) Console{*pixel_writer, {0, 0, 0}, {255, 255, 255}};

    char buf[128];
    for (int i = 0; i < 27; i++) {
        printk("printk: %d\n", i);
    }

    Halt();
}

// ref: https://qiita.com/yamoridon/items/4905765cc6e4f320c9b5#52-%E5%88%86%E5%89%B2%E3%82%B3%E3%83%B3%E3%83%91%E3%82%A4%E3%83%AB%E3%81%AE%E8%A3%9C%E8%B6%B3
extern "C" void __cxa_pure_virtual() {
    while (1) __asm__("hlt");
}
