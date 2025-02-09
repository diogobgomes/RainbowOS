/**
 * @file io.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Basic input/output system, a try at a decent, extensible one
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <klib/cstdlib.hpp>
#include <klib/string.h>
#include <devices/BIOSVideoIO.hpp>
#include <stdarg.h>

// FIXME get rid of this
extern "C" size_t outFormat(const char* fmt, va_list parameters, int (*putfunc)(const char* ,size_t));

// FIXME add what the requirements of the backEnd class are (I think it's just putchar and clear)

namespace io
{

template<class backEnd>
class _outstream
{
private:
    backEnd* _backEnd;
    int _base;

public:
    void clear() { _backEnd->clear(); }

    // FIXME
    void init(backEnd* aaaa) { _backEnd = aaaa; _base=10; /*_backEnd->init();*/ }

    backEnd* getBackEnd() { return _backEnd; }

    //auto &getBackEnd() { return _backEnd; }

    size_t writeString(const char *str) {
        size_t i = 0;
        for(size_t size = strlen(str);i<size;i++){
            _backEnd->putchar(*(str++));}
        return i;
    }

    size_t writeString(const char *str, size_t size) {
        size_t i = 0;
        for (; i < size; i++)
        {
            _backEnd->putchar(str[i]);
        }
        return i;
    }

    size_t writeHex( uint64_t num) {
        char str[MAX_NUM_STR_SIZE];
        xtoa(num,str,16);
        return writeString(str);
    }

    size_t writeInt( int64_t num) {
        char str[MAX_NUM_STR_SIZE];
        xtoa(num,str,10);
        return writeString(str);
    }

    template<typename T> size_t writeNum( T num ) {
        char str[MAX_NUM_STR_SIZE];
        xtoa(num,str,_base);
        return writeString(str);
    }

    int changeBase(int num) {
        if (num < 2 || num > 16)
        {
            return -1;
        }
        _base = num;
        return base;
    }

    auto &operator<<(const char* str) {
        writeString(str);
        return *this;
    }

    auto &operator<<(char* str) {
        writeString(str);
        return *this;
    }

    template<typename T> auto &operator<<(T num) {
        writeNum(num);
        return *this;
    }

    template<typename T> auto &operator<<(T* ptr) {
        writeNum(reinterpret_cast<uint64_t>(ptr));
        return *this;
    }

    auto &hex() { _base=16; return *this;}

    auto &dec() { _base=10; return *this;}

    auto &base(int num) {
        if (num < 2 || num > 16)
        {
            return *this; // Don't change it
            // TODO Maybe static_assert?
        }
        _base = num; return *this;
    }

    auto &operator<<(_outstream&) {return *this;}

}; // End of class "_outstream"

} // End of namespace "io"

/*#if defined(__is32__) || defined(__is64__)
extern io::_outstream<io::framebuffer_terminal> out;
#endif

#if defined(__is16__)
extern io::_outstream<io::realMode_terminal> out;
#endif*/

extern io::_outstream<io::framebuffer_terminal> out;