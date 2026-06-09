#include "bitops.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void buf_init(BitBuffer *buf, size_t capacity_bytes) { //инициализация
    buf->capacity = capacity_bytes;
    buf->data = calloc(1, capacity_bytes);
    if (!buf->data) { perror("calloc"); exit(EXIT_FAILURE); }
}

void buf_ensure(BitBuffer *buf, size_t needed_bytes) { //расширение буфера
    if (needed_bytes > buf->capacity) {
        size_t new_cap = needed_bytes * 2;
        uint8_t *tmp = realloc(buf->data, new_cap);
        if (!tmp) { perror("realloc"); exit(EXIT_FAILURE); }
        memset(tmp + buf->capacity, 0, new_cap - buf->capacity);
        buf->data = tmp;
        buf->capacity = new_cap;
    }
}

void buf_free(BitBuffer *buf) { //очистка буфера
    if (buf->data) free(buf->data);
    buf->data = NULL;
    buf->capacity = 0;
}

void setbits(uint8_t *arr, size_t offset, uint32_t val, uint8_t len) { //запись бит поля
    for (int i = 0; i < len; i++) {
        int bit = (val >> (len - 1 - i)) & 1; //старший бит идёт первым
        size_t byte_idx = (offset + i) / 8; //позиция в массиве байтов
        int bit_shift = 7 - ((offset + i) % 8); //реализация мсб
        arr[byte_idx] = (arr[byte_idx] & ~(1 << bit_shift)) | (bit << bit_shift); //очищаем и устанавливаем новое
    }
}

uint32_t getbits(const uint8_t *arr, size_t offset, uint8_t len) { //чтение бит поля
    uint32_t res = 0;
    for (int i = 0; i < len; i++) {
        size_t byte_idx = (offset + i) / 8;
        int bit_shift = 7 - ((offset + i) % 8);
        res = (res << 1) | ((arr[byte_idx] >> bit_shift) & 1); //сдвиг результата и добавляем новый бит
    }
    return res;
}
