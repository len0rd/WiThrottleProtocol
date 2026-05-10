#include "Print.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

size_t Print::write(const uint8_t* buf, size_t size)
{
    size_t n = 0;
    for (size_t i = 0; i < size; i++)
        n += write(buf[i]);
    return n;
}

size_t Print::print(const char* s)
{
    if (!s)
        return 0;
    return write((const uint8_t*) s, strlen(s));
}

size_t Print::print(int val)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", val);
    return print(buf);
}
size_t Print::print(unsigned int val)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%u", val);
    return print(buf);
}
size_t Print::print(long val)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%ld", val);
    return print(buf);
}
size_t Print::print(unsigned long val)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%lu", val);
    return print(buf);
}
size_t Print::print(double val)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%g", val);
    return print(buf);
}

size_t Print::println()
{
    return write((const uint8_t*) "\r\n", 2);
}
size_t Print::println(const char* s)
{
    size_t n = print(s);
    n += println();
    return n;
}
size_t Print::println(char c)
{
    size_t n = write((uint8_t) c);
    n += println();
    return n;
}
size_t Print::println(bool val)
{
    size_t n = print(val);
    n += println();
    return n;
}
size_t Print::println(int val)
{
    size_t n = print(val);
    n += println();
    return n;
}
size_t Print::println(unsigned int val)
{
    size_t n = print(val);
    n += println();
    return n;
}
size_t Print::println(long val)
{
    size_t n = print(val);
    n += println();
    return n;
}
size_t Print::println(unsigned long val)
{
    size_t n = print(val);
    n += println();
    return n;
}
size_t Print::println(double val)
{
    size_t n = print(val);
    n += println();
    return n;
}

size_t Print::printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return print(buf);
}
