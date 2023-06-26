
#include "console.hpp"

#include <cstring>
#include "font.hpp"

Console::Console(PixelWriter &writer, const PixelColor &fg_color, const PixelColor &bg_color)
    : writer_{writer}, fg_color_{fg_color}, bg_color_{bg_color}, buffer_{}, cursor_row_{0}, cursor_column_{0} {
}

void Console::PutString(const char *s) {
    while (*s) {
        if (*s == '\n') {
            Newline();
        } else if (cursor_column_ < kColumns - 1) {
            // 画面内に収まっているときは書き込みするって感じだね
            WriteAscii(writer_, 8 * cursor_column_, 16 * cursor_row_, *s, fg_color_);
            buffer_[cursor_row_][cursor_column_] = *s;
            cursor_column_++;
        }
        s++;
    }
}

void Console::Newline() {
    cursor_column_ = 0;
    if (cursor_row_ < kRows - 1) {
        // 次に書き込む場所を1つしたにずらす
        cursor_row_++;
        return;
    }

    // 下まで行っている場合は、全てをずらす処理

    // 一度全て塗りつぶす
    for (int y = 0; y < 16 * kRows; y++) {
        for (int x = 0; x < 8 * kColumns; x++) {
            writer_.Write(x, y, bg_color_);
        }
    }

    for (int row = 0; row < kRows -1; row++) {
        // 一つ上に、内容を書き込んでいく
        memcpy(buffer_[row], buffer_[row+1], kColumns + 1);
        WriteString(writer_, 0, 16 * row, buffer_[row], fg_color_);
    }
    // 一番最後の行は空欄を表示する
    memset(buffer_[kRows - 1], 0, kColumns+1);
}
