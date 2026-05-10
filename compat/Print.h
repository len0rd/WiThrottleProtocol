#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include "WString.h"

/**
 * Minimal Arduino Print base class.
 * Provides print/println overloads for all types used by WiThrottleProtocol.
 */
class Print
{
public:
    virtual ~Print() = default;

    virtual size_t write(uint8_t c) = 0;
    virtual size_t write(const uint8_t* buf, size_t size);

    // --- print overloads ---
    size_t print(const char* s);
    size_t print(const String& s)
    {
        return print(s.c_str());
    }
    size_t print(char c)
    {
        return write((uint8_t) c);
    }
    size_t print(bool val)
    {
        return print(val ? "1" : "0");
    }
    size_t print(int val);
    size_t print(unsigned int val);
    size_t print(long val);
    size_t print(unsigned long val);
    size_t print(double val);

    // Formatted print (used by WiThrottleProtocol for logging)
    size_t printf(const char* fmt, ...) __attribute__((format(printf, 2, 3)));

    // --- println overloads ---
    size_t println();
    size_t println(const char* s);
    size_t println(const String& s)
    {
        return println(s.c_str());
    }
    size_t println(char c);
    size_t println(bool val);
    size_t println(int val);
    size_t println(unsigned int val);
    size_t println(long val);
    size_t println(unsigned long val);
    size_t println(double val);
};

/**
 * Minimal Arduino Stream abstract class.
 */
class Stream : public Print
{
public:
    virtual int  available() = 0;
    virtual int  read()      = 0;
    virtual int  peek()      = 0;
    virtual void flush() {}
};
