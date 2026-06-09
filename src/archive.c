#include "archive.h"
#include "bitops.h"
#include "shennon.h"
#include <stdio.h>
#include <stdlib.h>

void compress(const char *in_path, const char *out_path) { //сжатие
    FILE *fin = fopen(in_path, "rb"); //читаем исходник
    if (!fin) { perror("Нет input файла"); return; }

    fseek(fin, 0, SEEK_END);
    long file_size = ftell(fin);
    if (file_size <= 0) { fprintf(stderr, "Пустой файл\n"); fclose(fin); return; }
    rewind(fin);

    uint8_t *data = malloc(file_size); //иниц под размер данных
    fread(data, 1, file_size, fin);
    fclose(fin);

    CodeEntry table[256]; //строим таблицу ШФ
    int num_syms = build_shannon_table(data, file_size, table);

    CodeEntry *lookup[256] = {NULL}; //таблица для быстрого поиска
    for (int i = 0; i < num_syms; i++) lookup[table[i].symbol] = &table[i];

    size_t total_bits = 0; //расчет бит в сжатом потоке
    for (long i = 0; i < file_size; i++) total_bits += lookup[data[i]]->code_len; //для каждого b в data

    BitBuffer buf; //битовый буфер с емкостью
    buf_init(&buf, (total_bits + 7) / 8 + 16);
    size_t bit_offset = 0;
    for (long i = 0; i < file_size; i++) { //кодирование сообщения для каждого b
        CodeEntry *e = lookup[data[i]];
        buf_ensure(&buf, (bit_offset + e->code_len + 7) / 8);
        setbits(buf.data, bit_offset, e->code_val, e->code_len);
        bit_offset += e->code_len;
    }

    FILE *fout = fopen(out_path, "wb"); //записываем сжатый файл
    if (!fout) { perror("Нет out файла"); buf_free(&buf); free(data); return; }

    uint32_t n = num_syms; //колво уник символов
    uint64_t orig_len = file_size; //исходный размер файла
    fwrite(&n, sizeof(n), 1, fout); //заголовок (4байта) 
    fwrite(&orig_len, sizeof(orig_len), 1, fout); //8байт 

    fwrite(table, sizeof(CodeEntry), num_syms, fout); //таблица CodeEntry*num_syms
    fwrite(buf.data, 1, (bit_offset + 7) / 8, fout); //бит поток
    fclose(fout);

    //размеры компонентов
    size_t bitstream_bytes = (bit_offset + 7) / 8;
    if (bitstream_bytes == 0) bitstream_bytes = 1; //защита от 0
    size_t header_bytes = sizeof(n) + sizeof(orig_len) + (size_t)num_syms * sizeof(CodeEntry);
    size_t total_bytes = header_bytes + bitstream_bytes;

    //кэффы
    double k_raw   = (double)file_size / bitstream_bytes; //теоретический (только данные)
    double k_total = (double)file_size / total_bytes;     //фактический (файл целиком)

    printf("Сжатие завершено:\n");
    printf("\tИсходный файл: %ld байт\n", file_size);
    printf("\tБитовый поток (без заголовка): %zu байт (k=%.2f)\n", bitstream_bytes, k_raw);
    printf("\tПолный архив (с заголовком):   %zu байт (k=%.2f)\n", total_bytes, k_total);

    buf_free(&buf);
    free(data);
}

void decompress(const char *in_path, const char *out_path) { //распаковка
    FILE *fin = fopen(in_path, "rb"); //открываем и читаем заголовок
    if (!fin) { perror("Input file"); return; }

    uint32_t num_syms;
    uint64_t orig_len;
    if (fread(&num_syms, sizeof(num_syms), 1, fin) != 1) { fprintf(stderr, "Чтение заголовка\n"); fclose(fin); return; }
    if (fread(&orig_len, sizeof(orig_len), 1, fin) != 1) { fprintf(stderr, "Чтение длины\n"); fclose(fin); return; }

    CodeEntry *table = malloc(num_syms * sizeof(CodeEntry));
    fread(table, sizeof(CodeEntry), num_syms, fin);

    long data_start = ftell(fin); //чтение бит потока
    fseek(fin, 0, SEEK_END);
    long total_size = ftell(fin);
    long bitstream_size = total_size - data_start;
    fseek(fin, data_start, SEEK_SET);

    uint8_t *bitstream = malloc(bitstream_size); //память под битпоток
    fread(bitstream, 1, bitstream_size, fin);
    fclose(fin);

    FILE *fout = fopen(out_path, "wb"); //декод файл
    if (!fout) { perror("Нет out файла"); free(bitstream); free(table); return; }

    uint32_t acc = 0; //для битов
    int acc_len = 0;
    size_t bit_offset = 0; 
    size_t total_bits = bitstream_size * 8;
    uint64_t decoded = 0; //восст символы

    while (decoded < orig_len && bit_offset < total_bits) {
        acc = (acc << 1) | getbits(bitstream, bit_offset, 1); //читаем биты из потока и добавляем в аккум
        acc_len++; //длину и смещение +1
        bit_offset++;

        for (uint32_t i = 0; i < num_syms; i++) { //для совпадений в табл кодов
            if (table[i].code_len == acc_len && table[i].code_val == acc) {
                fputc(table[i].symbol, fout);
                decoded++;
                acc = 0;
                acc_len = 0;
                break;
            }
        }
    }

    fclose(fout);
    free(bitstream);
    free(table);
    printf("Распаковка завершена: %lu символов восстановлено\n", (unsigned long)decoded);
}