#include "common.hpp"

void Halt() {
    while (1) __asm__("hlt");
}

// // 配置newの定義
// void *operator new(size_t size, void *buf) {
//     return buf;
// }

void operator delete(void *obj) noexcept {}
