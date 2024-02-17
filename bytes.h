#ifndef BYTES_H_INCLUDED
#define BYTES_H_INCLUDED

#include <stdint.h>

#define LSH_BYTE(num) (num << 8)
#define GET_BYTE(num, n) (num >> n) & 0xFF
#define GET_HEX(num, n) (num >> n) & 0xF

#define GET_BIT(num, n) (num >> n) & 1
#define SET_BIT(num, n) num |= 1 << n
#define TOGGLE_BIT(num, n) num ^= 1 << n
#define CLEAR_BIT(num, n) num &= ~(1 << n)

typedef uint8_t Byte;
typedef uint16_t Word;
typedef uint32_t Dword;


#endif // BYTES_H_INCLUDED
