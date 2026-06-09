#ifndef BITOPS_H
#define BITOPS_H

#include "types.h"

//управление битовым буфером
void buf_init(BitBuffer *buf, size_t capacity_bytes);
void buf_ensure(BitBuffer *buf, size_t needed_bytes);
void buf_free(BitBuffer *buf);

//побитовые операции (MSB-first)
void setbits(uint8_t *arr, size_t offset, uint32_t val, uint8_t len);
uint32_t getbits(const uint8_t *arr, size_t offset, uint8_t len);

#endif
