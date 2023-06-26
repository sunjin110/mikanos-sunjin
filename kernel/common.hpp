#pragma once

#include <cstddef>

void Halt(void);

void *operator new(size_t size, void *buf);

void operator delete(void *obj) noexcept;
