#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdarg>
#include "frame_buffer_config.hpp"
#include "graphics.hpp"
#include "font.hpp"
#include "common.hpp"
#include "console.hpp"
#include "mouse.hpp"

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

    for (int dy = 0; dy < kMouseCursorHeight; dy++) {
        for (int dx = 0; dx < kMouseCursorWidth; dx++) {
            if (mouse_cursor_shape[dy][dx] == '@') {
                pixel_writer->Write(200 + dx, 100 + dy, {0, 0, 0});
            } else if (mouse_cursor_shape[dy][dx] == '.') {
                pixel_writer->Write(200 + dx, 100 + dy, {255, 255, 255});
            }
        }
    }

    Halt();
}

// ref: https://qiita.com/yamoridon/items/4905765cc6e4f320c9b5#52-%E5%88%86%E5%89%B2%E3%82%B3%E3%83%B3%E3%83%91%E3%82%A4%E3%83%AB%E3%81%AE%E8%A3%9C%E8%B6%B3
extern "C" void __cxa_pure_virtual() {
    while (1) __asm__("hlt");
}
