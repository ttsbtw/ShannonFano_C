# Программа для кодирования по Шеннону Фано

# Язык реализации - Си

# Работает с данными на **АНГЛИЙСКОМ ЯЗЫКЕ**!

---

# Пример пользования:

- $ sfcompress -c -o file.sfc file.txt (сжатие file.txt в file.sfc)
- $ sfcompress -d -o file1.txt file.sfc (распаковка file.sfc в file1.txt)

Тестовые файлы можете найти в /tests_txt

---

# Быстрые проверки с моими тестами:

- make all - простая компиляция

- make compress - сжатие .txt с содержимым "bbbbmdddyyyyyy" в .sfc

- make decompress - распаковка .sfc в .txt с содержимым "bbbbmdddyyyyyy"

- make test - сравнение данных этих двух файлов (совпадает)

- make test-long - сжатие длинного .txt (800байт) в .sfc

---

PS: В /docs предоставлен отчет о выполненной работе.
