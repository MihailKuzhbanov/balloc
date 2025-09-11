Модуль блочного аллокатора памяти для C/C++

Библиотеки для использования аллокатора в проекте

    С - bins/libba_c.so
    C++ - bins/libba_cpp.so
Для использования подключить заголовочники

    C - include/b_allocator.h
    C++ - include/block_allocator.hpp
Для сборки в составе проекта добавить файлы реализации

    C - src/b_allocator.c
    C++ - src/block_allocator.cpp


Сборка проекта: mkdir build ; cd build ; cmake .. ; make
Проход тестов: make test
Генерация документации: make docs
