#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>

#include "./testio.h"

enum class conversion {
    hh, h, NONE, l, ll, j, z, t, L
};

namespace ctest {
    Logger::Logger() {
        depth = 0;
    }

    LoggerChannel* Logger::open(FILE* fd, bool keep) {
        LoggerChannel* c = new LoggerChannel();

        c->fd       =  fd;
        c->__aalloc = !keep;
        c->__logger =  this;

        return c;
    }

    int Logger::close(LoggerChannel* c) {
        if (c->__aalloc) fclose(c->fd);
        return 0;
    }

    int Logger::cprintf(LoggerChannel* channel, char* format, ...) {
        va_list args;
        va_start(args, format);
        int ret = vcprintf(channel, format, args);
        va_end(args);
        return ret;
    }

    int Logger::vcprintf(LoggerChannel* channel, char* format, va_list args) {
        FILE* fd = channel->fd;

        for (unsigned long i = 0; format[i]; i++) {
            char c = format[i];
            switch (c) {
                case '\n': {
                    ::putc('\n', fd);
                    for (int i = 0; i < depth; i++) ::putc(' ', fd);
                    break;
                }
                case '%': {
                    char c = format[++i];

                    bool justify_left = 0,
                         force_sign = 0,
                         pad_sign = 0,
                         altern = 0,
                         zero_padded = 0;

                    for (;;) {
                        switch (c) {
                            case '-': {
                                justify_left = 1;
                                break;
                            }
                            case '+': {
                                force_sign = 1;
                                break;
                            }
                            case ' ': {
                                pad_sign = 1;
                                break;
                            }
                            case '#': {
                                altern = 1;
                                break;
                            }
                            case '0': {
                                zero_padded = 1;
                                break;
                            }
                            default: {
                                goto br_fmt_flags;
                            }
                        }
                        c = format[++i];
                    } br_fmt_flags:;

                    bool min_widthv = 0;
                    int min_width = 0;
                    if (c >= '0' && c <= '9') {
                        for (;c >= '0' && c <= '9';) {
                            min_width *= 10;
                            min_width += c - '0';
                            c = format[++i];
                        }
                    } else if (c == '*') {
                        min_widthv = 1;
                        min_width = va_arg(args, int);
                        c = format[++i];
                    }

                    bool precisionv = 0;
                    int precision = 0;
                    if (c == '.') {
                        c = format[++i];
                        if (c >= '0' && c <= '9') {
                            for (;c >= '0' && c <= '9';) {
                                precision *= 10;
                                precision += c - '0';
                                c = format[++i];
                            }
                        } else if (c == '*') {
                            precisionv = 1;
                            precision = va_arg(args, int);
                            c = format[++i];
                        }
                    }

                    conversion length;
                    switch (c) {
                        case 'h': {
                            c = format[++i];
                            if (c == 'h') {
                                c = format[++i];
                                length = conversion::hh;
                                break;
                            }
                            length = conversion::h;
                            break;
                        }
                        case 'l': {
                            c = format[++i];
                            if (c == 'l') {
                                c = format[++i];
                                length = conversion::ll;
                                break;
                            }
                            length = conversion::l;
                            break;
                        }
                        case 'j': {
                            c = format[++i];
                            length = conversion::j;
                            break;
                        }
                        case 'z': {
                            c = format[++i];
                            length = conversion::z;
                            break;
                        }
                        case 't': {
                            c = format[++i];
                            length = conversion::t;
                            break;
                        }
                        case 'L': {
                            c = format[++i];
                            length = conversion::L;
                            break;
                        }
                        default: {
                            length = conversion::NONE;
                            break;
                        }
                    }

                    switch (c) {
                        case '%': {
                            ::putc('%', fd);
                            break;
                        }
                        case 'c': {
                            switch (length) {
                                default:
                                case conversion::NONE: {
                                    int v = va_arg(args, int);
                                    ::putc(v, fd);
                                    break;
                                }
                                case conversion::l: {
                                    wchar_t v = va_arg(args, int);
                                    ::fputwc(v, fd);
                                    break;
                                }
                            }
                            break;
                        }
                        case 's': {
                            switch (length) {
                                default:
                                case conversion::NONE: {
                                    char* v = va_arg(args, char*);
                                    if (!v) v = "{NULL}";
                                    int len = strlen(v);
                                    if (precision) fwrite(v, 1, precision > len ? precision : len, fd);
                                    else           fwrite(v, 1, len, fd);
                                    break;
                                }
                                case conversion::l: {
                                    wchar_t* v = va_arg(args, wchar_t*);
                                    ::fputws(v, fd);
                                    //int len = strlen(v);
                                    //if (precision) fwrite(v, 1, precision > len ? precision : len, fd);
                                    //else           fwrite(v, 1, len, fd);
                                    break;
                                }
                            }
                            break;
                        }
                        case 'd':
                        case 'i': {
                            switch (length) {
                                case conversion::hh: {
                                    signed char v = va_arg(args, int);
                                    goto cprintf_die;
                                }
                                case conversion::h: {
                                    short v = va_arg(args, int);
                                    goto cprintf_die;
                                }
                                default:
                                case conversion::NONE: {
                                    int v = va_arg(args, int);
                                    ::fprintf(fd, "%i", v);
                                    break;
                                }
                                case conversion::l: {
                                    long v = va_arg(args, long);
                                    goto cprintf_die;
                                }
                                case conversion::ll: {
                                    long long v = va_arg(args, long long);
                                    goto cprintf_die;
                                }
                                case conversion::j: {
                                    intmax_t v = va_arg(args, intmax_t);
                                    goto cprintf_die;
                                }
                                case conversion::z: {
                                    size_t v = va_arg(args, size_t);
                                    goto cprintf_die;
                                }
                                case conversion::t: {
                                    ptrdiff_t v = va_arg(args, ptrdiff_t);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case 'o': {
                            switch (length) {
                                case conversion::hh: {
                                    unsigned char v = va_arg(args, unsigned int);
                                    goto cprintf_die;
                                }
                                case conversion::h: {
                                    unsigned short v = va_arg(args, unsigned int);
                                    goto cprintf_die;
                                }
                                default:
                                case conversion::NONE: {
                                    unsigned int v = va_arg(args, unsigned int);
                                    goto cprintf_die;
                                }
                                case conversion::l: {
                                    unsigned long v = va_arg(args, unsigned long);
                                    goto cprintf_die;
                                }
                                case conversion::ll: {
                                    unsigned long long v = va_arg(args, unsigned long long);
                                    goto cprintf_die;
                                }
                                case conversion::j: {
                                    uintmax_t v = va_arg(args, uintmax_t);
                                    goto cprintf_die;
                                }
                                case conversion::z: {
                                    size_t v = va_arg(args, size_t);
                                    goto cprintf_die;
                                }
                                case conversion::t: {
                                    ptrdiff_t v = va_arg(args, ptrdiff_t);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case 'x':
                        case 'X': {
                            switch (length) {
                                case conversion::hh: {
                                    unsigned char v = va_arg(args, unsigned int);
                                    goto cprintf_die;
                                }
                                case conversion::h: {
                                    unsigned short v = va_arg(args, unsigned int);
                                    goto cprintf_die;
                                }
                                default:
                                case conversion::NONE: {
                                    unsigned int v = va_arg(args, unsigned int);
                                    goto cprintf_die;
                                }
                                case conversion::l: {
                                    unsigned long v = va_arg(args, unsigned long);
                                    goto cprintf_die;
                                }
                                case conversion::ll: {
                                    unsigned long long v = va_arg(args, unsigned long long);
                                    goto cprintf_die;
                                }
                                case conversion::j: {
                                    uintmax_t v = va_arg(args, uintmax_t);
                                    goto cprintf_die;
                                }
                                case conversion::z: {
                                    size_t v = va_arg(args, size_t);
                                    goto cprintf_die;
                                }
                                case conversion::t: {
                                    ptrdiff_t v = va_arg(args, ptrdiff_t);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case 'u': {
                            switch (length) {
                                case conversion::hh: {
                                    unsigned char v = va_arg(args, unsigned int);
                                    goto cprintf_die;
                                }
                                case conversion::h: {
                                    unsigned short v = va_arg(args, unsigned int);
                                    goto cprintf_die;
                                }
                                default:
                                case conversion::NONE: {
                                    unsigned int v = va_arg(args, unsigned int);
                                    goto cprintf_die;
                                }
                                case conversion::l: {
                                    unsigned long v = va_arg(args, unsigned long);
                                    goto cprintf_die;
                                }
                                case conversion::ll: {
                                    unsigned long long v = va_arg(args, unsigned long long);
                                    goto cprintf_die;
                                }
                                case conversion::j: {
                                    uintmax_t v = va_arg(args, uintmax_t);
                                    goto cprintf_die;
                                }
                                case conversion::z: {
                                    size_t v = va_arg(args, size_t);
                                    goto cprintf_die;
                                }
                                case conversion::t: {
                                    ptrdiff_t v = va_arg(args, ptrdiff_t);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case 'f':
                        case 'F': {
                            switch (length) {
                                default:
                                case conversion::NONE: {
                                    double v = va_arg(args, double);
                                    goto cprintf_die;
                                }
                                case conversion::l: {
                                    double v = va_arg(args, double);
                                    goto cprintf_die;
                                }
                                case conversion::L: {
                                    long double v = va_arg(args, long double);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case 'e':
                        case 'E': {
                            switch (length) {
                                default:
                                case conversion::NONE: {
                                    double v = va_arg(args, double);
                                    goto cprintf_die;
                                }
                                case conversion::l: {
                                    double v = va_arg(args, double);
                                    goto cprintf_die;
                                }
                                case conversion::L: {
                                    long double v = va_arg(args, long double);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case 'a':
                        case 'A': {
                            switch (length) {
                                default:
                                case conversion::NONE: {
                                    double v = va_arg(args, double);
                                    goto cprintf_die;
                                }
                                case conversion::l: {
                                    double v = va_arg(args, double);
                                    goto cprintf_die;
                                }
                                case conversion::L: {
                                    long double v = va_arg(args, long double);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case 'g':
                        case 'G': {
                            switch (length) {
                                default:
                                case conversion::NONE: {
                                    double v = va_arg(args, double);
                                    goto cprintf_die;
                                }
                                case conversion::l: {
                                    double v = va_arg(args, double);
                                    goto cprintf_die;
                                }
                                case conversion::L: {
                                    long double v = va_arg(args, long double);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case 'n': {
                            switch (length) {
                                case conversion::hh: {
                                    signed char* v = va_arg(args, signed char*);
                                    goto cprintf_die;
                                }
                                case conversion::h: {
                                    short* v = va_arg(args, short*);
                                    goto cprintf_die;
                                }
                                default:
                                case conversion::NONE: {
                                    int* v = va_arg(args, int*);
                                    goto cprintf_die;
                                }
                                case conversion::l: {
                                    long* v = va_arg(args, long*);
                                    goto cprintf_die;
                                }
                                case conversion::ll: {
                                    long long* v = va_arg(args, long long*);
                                    goto cprintf_die;
                                }
                                case conversion::j: {
                                    intmax_t* v = va_arg(args, intmax_t*);
                                    goto cprintf_die;
                                }
                                case conversion::z: {
                                    size_t* v = va_arg(args, size_t*);
                                    goto cprintf_die;
                                }
                                case conversion::t: {
                                    ptrdiff_t* v = va_arg(args, ptrdiff_t*);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case 'p': {
                            switch (length) {
                                default:
                                case conversion::NONE: {
                                    void* v = va_arg(args, void*);
                                    goto cprintf_die;
                                }
                            }
                            break;
                        }
                        case '>': {
                            depth++;
                            break;
                        }
                        case '<': {
                            if (depth != 0) depth--;
                            break;
                        }
                        default: {
                            cprintf_die:
                            ::putc('%', fd);
                            if (justify_left)        ::putc('-', fd);
                            if (force_sign)          ::putc('+', fd);
                            if (pad_sign)            ::putc(' ', fd);
                            if (altern)              ::putc('#', fd);
                            if (zero_padded)         ::putc('0', fd);
                            if (min_widthv)          ::putc('*', fd);
                            else if (min_width)      ::fprintf(fd, "%i", min_width);
                            if (precisionv)          ::fputs(".*", fd);
                            else if (precision)      ::fprintf(fd, ".%i", precision);
                            switch (length) {
                                case conversion::hh: ::fputs("hh", fd); break;
                                case conversion::h:  ::putc('h', fd); break;
                                case conversion::l:  ::putc('l', fd); break;
                                case conversion::ll: ::fputs("ll", fd); break;
                                case conversion::j:  ::putc('j', fd); break;
                                case conversion::z:  ::putc('z', fd); break;
                                case conversion::t:  ::putc('t', fd); break;
                                case conversion::L:  ::putc('L', fd); break;
                            }
                                                     ::putc(c, fd);
                            break;
                        }
                    }
                    
                    break;
                }
                default: {
                    ::putc(c, fd);
                    break;
                }
            }
        }

        return 0;
    }
};