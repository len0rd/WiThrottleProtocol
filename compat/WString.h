#pragma once
#include <string>
#include <stdint.h>
#include <stdlib.h> // strtol, strtof

/**
 * Minimal Arduino String shim wrapping std::string.
 * Adds only the methods called by WiThrottleProtocol.
 */
class String : public std::string
{
public:
    // --- Constructors ---
    String() = default;
    String(const char* s) : std::string(s ? s : "") {}
    String(const std::string& s) : std::string(s) {}
    String(std::string&& s) : std::string(std::move(s)) {}
    String(char c) : std::string(1, c) {}
    String(int v) : std::string(std::to_string(v)) {}
    String(unsigned int v) : std::string(std::to_string(v)) {}
    String(long v) : std::string(std::to_string(v)) {}
    String(unsigned long v) : std::string(std::to_string(v)) {}
    String(double v) : std::string(std::to_string(v)) {}

    // --- Assignment from char* ---
    String& operator=(const char* s)
    {
        std::string::operator=(s ? s : "");
        return *this;
    }

    // --- Arduino API shims ---

    int indexOf(const char* s, int from = 0) const
    {
        auto pos = find(s, (size_type) from);
        return pos == npos ? -1 : (int) pos;
    }
    int indexOf(const String& s, int from = 0) const
    {
        return indexOf(s.c_str(), from);
    }
    int indexOf(char c, int from = 0) const
    {
        auto pos = find(c, (size_type) from);
        return pos == npos ? -1 : (int) pos;
    }

    bool startsWith(const char* s) const
    {
        return find(s) == 0;
    }
    bool startsWith(const String& s) const
    {
        return find(s.c_str()) == 0;
    }

    // Arduino substring(from) and substring(from, to) — 'to' is exclusive end index
    String substring(int from, int to = -1) const
    {
        int len = (int) size();
        if (from < 0)
            from = 0;
        if (from > len)
            from = len;
        if (to < 0 || to > len)
            to = len;
        if (to < from)
            to = from;
        return String(substr((size_type) from, (size_type) (to - from)));
    }

    long toInt() const
    {
        return strtol(c_str(), nullptr, 10);
    }
    float toFloat() const
    {
        return strtof(c_str(), nullptr);
    }

    bool equals(const char* s) const
    {
        return *this == s;
    }
    bool equals(const String& s) const
    {
        return static_cast<const std::string&>(*this) == static_cast<const std::string&>(s);
    }

    char charAt(int i) const
    {
        return (*this)[(size_type) i];
    }

    void concat(const char* s)
    {
        append(s);
    }
    void concat(const String& s)
    {
        append(s);
    }
    void concat(char c)
    {
        push_back(c);
    }

    // Arduino String::remove(index) and remove(index, count)
    void remove(int index, int count = -1)
    {
        if (index < 0 || index >= (int) size())
            return;
        if (count < 0 || index + count > (int) size())
            count = (int) size() - index;
        erase((size_type) index, (size_type) count);
    }

    void trim()
    {
        auto s = find_first_not_of(" \t\r\n");
        if (s == npos)
        {
            clear();
            return;
        }
        erase(0, s);
        auto e = find_last_not_of(" \t\r\n");
        if (e != npos)
            erase(e + 1);
    }

    // --- Operator overloads ---
    // std::string already provides operator+ returning std::string;
    // these overloads ensure String is returned when left operand is String.
    String operator+(const String& rhs) const
    {
        return String(static_cast<const std::string&>(*this) +
                      static_cast<const std::string&>(rhs));
    }
    String operator+(const char* rhs) const
    {
        return String(static_cast<const std::string&>(*this) + rhs);
    }
    String operator+(char rhs) const
    {
        return String(static_cast<const std::string&>(*this) + rhs);
    }
    String& operator+=(const String& rhs)
    {
        append(rhs);
        return *this;
    }
    String& operator+=(const char* rhs)
    {
        append(rhs);
        return *this;
    }
    String& operator+=(char rhs)
    {
        push_back(rhs);
        return *this;
    }
};
