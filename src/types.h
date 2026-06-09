#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

//структура для хранения кода символа (1+1+4байта = 6байт)
typedef struct {
    uint8_t  symbol;
    uint8_t  code_len;
    uint32_t code_val;
} __attribute__((packed)) CodeEntry;

//вспомогательная структура для подсчёта частот
typedef struct {
    uint8_t symbol;
    uint64_t freq;
} SymFreq;

//динамический битовый буфер
typedef struct {
    uint8_t *data;
    size_t   capacity; //в байтах
} BitBuffer;

#endif
