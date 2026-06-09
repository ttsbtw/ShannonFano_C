#include "archive.h"
#include <stdio.h>
#include <string.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    int c_flag = 0, d_flag = 0;
    char *out_file = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "cdo:")) != -1) { //c и d флаги без аргументов, o: с арг
        switch (opt) { //выборка
            case 'c': c_flag = 1; break;
            case 'd': d_flag = 1; break;
            case 'o': out_file = optarg; break;
            default:
                fprintf(stderr, "Использование: %s {-c|-d} -o output_file input_file\n", argv[0]);
                return 1;
        }
    }
    
    //проверка на вход файл (optindex)
    if (optind >= argc) {
        fprintf(stderr, "Ошибка: не указан входной файл.\n");
        return 1;
    }
    const char *in_file = argv[optind];

    //валидность флагов
    if (!out_file) {
        fprintf(stderr, "Ошибка: необходимо указать выходной файл опцией -o.\n");
        return 1;
    }

    if (!c_flag && !d_flag) {
        fprintf(stderr, "Ошибка: укажите -c или -d.\n");
        return 1;
    }

    if (c_flag && d_flag) {
        fprintf(stderr, "Ошибка: нельзя одновременно -c и -d.\n");
        return 1;
    }

    //выполнение
    if (c_flag) {
        compress(in_file, out_file);
    } else {
        decompress(in_file, out_file);
    }

    return 0;
}
