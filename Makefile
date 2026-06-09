CC = gcc
CFLAGS = -Wall -Wextra -Isrc

# --- Директории и файлы ---
BINDIR = bin
SRCDIR = src
TESTDIR = tests_txt

TARGET = $(BINDIR)/sfcompress

SRCS = $(SRCDIR)/main.c \
       $(SRCDIR)/archive.c \
       $(SRCDIR)/bitops.c \
       $(SRCDIR)/shennon.c

TEST_COMPRESSED = $(BINDIR)/test_compressed.sfc
TEST_RESTORED = $(BINDIR)/test_restored.txt

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(BINDIR)
	@echo "Компиляция $(TARGET)..."
	$(CC) $(CFLAGS) $^ -o $@

compress: $(TARGET)
	@echo "Сжимаем $(TESTDIR)/test.txt -> $(TEST_COMPRESSED)..."
	@./$(TARGET) -c -o $(TEST_COMPRESSED) $(TESTDIR)/test.txt

decompress: $(TARGET)
	@echo "Распаковываем $(TEST_COMPRESSED) -> $(TEST_RESTORED)..."
	@./$(TARGET) -d -o $(TEST_RESTORED) $(TEST_COMPRESSED)

#Полный тест: сжатие + распаковка + (опционально) сравнение
test: $(TARGET)
	@echo "Запуск полного цикла тестирования..."
	@echo "Шаг 1: Сжатие..."
	@./$(TARGET) -c -o $(TEST_COMPRESSED) $(TESTDIR)/test.txt
	@echo "Шаг 2: Распаковка..."
	@./$(TARGET) -d -o $(TEST_RESTORED) $(TEST_COMPRESSED)
	@echo "Шаг 3: Сравнение оригинала и восстановленного файла..."
	@diff -q $(TESTDIR)/test.txt $(TEST_RESTORED) && echo "Успех! Файлы идентичны." || echo "Ошибка! Файлы отличаются."

test-long: $(TARGET)
	@echo "Сжатие длинного файла..."
	@./$(TARGET) -c -o $(BINDIR)/test_long_compressed.sfc $(TESTDIR)/test_long.txt

clean:
	@echo "Очистка..."
	@rm -f $(TARGET)
	@rm -f $(TEST_COMPRESSED) $(TEST_RESTORED)
	@rm -f $(BINDIR)/test_long_compressed.sfc

.PHONY: all compress decompress test test-long clean