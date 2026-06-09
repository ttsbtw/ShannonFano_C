#include "shennon.h"
#include <stdlib.h>
#include <string.h>

static int find_split(const SymFreq *syms, int start, int end) { //поиск точки разбиения
    uint64_t total = 0;
    for (int i = start; i < end; i++) total += syms[i].freq;

    uint64_t left_sum = 0;
    int best_split = start + 1;
    uint64_t min_diff = (uint64_t)-1;

    for (int i = start; i < end - 1; i++) {
        left_sum += syms[i].freq;
        uint64_t right_sum = total - left_sum;
        uint64_t diff = (left_sum > right_sum) ? (left_sum - right_sum) : (right_sum - left_sum);
        if (diff < min_diff) {
            min_diff = diff;
            best_split = i + 1;
        }
    }
    return best_split;
}

static void sf_build(SymFreq *syms, int start, int end, uint32_t code, int depth, CodeEntry *table, int *idx) { //рекурсия и постройка дерева
    if (start >= end) return;
    
    if (start == end - 1) { //если 1 символ
        table[*idx].symbol = syms[start].symbol;
        table[*idx].code_len = depth;
        table[*idx].code_val = code;
        (*idx)++;
        return;
    }

    int split = find_split(syms, start, end); //разбиваем
    sf_build(syms, start, split, code << 1, depth + 1, table, idx); //в левую добавляем 0
    sf_build(syms, split, end, (code << 1) | 1, depth + 1, table, idx); //в правую 1
}

static int cmp_freq_desc(const void *a, const void *b) { //для qsort
    const SymFreq *fa = (const SymFreq *)a;
    const SymFreq *fb = (const SymFreq *)b;
    if (fb->freq != fa->freq) return (fb->freq > fa->freq) - (fa->freq > fb->freq); //основное сравнение по частоте
    return fa->symbol - fb->symbol; //вторич сравн по коду символа
}

int build_shannon_table(const uint8_t *data, size_t data_len, CodeEntry *table) { //построение таблицы
    SymFreq freq[256] = {0}; //подсчитываем частоты
    for (size_t i = 0; i < data_len; i++) {
        freq[data[i]].symbol = data[i];
        freq[data[i]].freq++;
    }

    SymFreq sorted[256]; //фильтруем и копируем ненулевые
    int num_syms = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i].freq > 0) sorted[num_syms++] = freq[i];
    }
    qsort(sorted, num_syms, sizeof(SymFreq), cmp_freq_desc); //сорт по убыванию част

    int table_idx = 0;
    sf_build(sorted, 0, num_syms, 0, 0, table, &table_idx); //само построение таблицы кодов
    return num_syms;
}
