#ifndef SHENNON_H
#define SHENNON_H

#include "types.h"

//построение таблицы кодов Шеннона-Фано
int build_shannon_table(const uint8_t *data, size_t data_len, CodeEntry *table);

#endif 
