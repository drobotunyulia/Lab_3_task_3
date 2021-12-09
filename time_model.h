#ifndef TIME_MODEL_H
#define TIME_MODEL_H

//модель, реализующая ход времени

#include <iostream>
#include <ctime>

size_t get_random_value(size_t min_value, size_t max_value)
//функция генерации равномерно распределенного
//случайного числа в заданном диапазоне
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    return static_cast<size_t>(rand() * fraction * (max_value - min_value + 1) + min_value);
}

class time_obj
//класс, моделирующий процессы хода времени
{
private:
    //текущее время (в виде стандартной структуры из модуля "ctime")
    tm *tm_struct;
    //текущее время (в виде количества секунд)
    time_t current_time;
public:
    time_obj()
    //устанавливаем время в 8.00 текущего дня
    {
        time_t time_init = time(0);
        this->tm_struct = localtime(&time_init);
        this->tm_struct->tm_hour = 8;
        this->tm_struct->tm_min = 0;
        this->tm_struct->tm_sec = 0;
        current_time = mktime(tm_struct);
    }
    std::string get_date_time_str()
    //получаем текущее время в виде строки
    {
        std::string result;
        result = asctime(this->tm_struct);
        result = result.erase(result.size() - 1, result.size());
        return result;
    }
    tm *get_date_time()
    //получаем текущее время в виде стандартной структуры
    {
        return this->tm_struct;
    }
    void execute()
    //одна итерация рабочего цикла
    //(увеличение текущего времени на 1 минуту)
    {
        current_time = current_time + 60;
        tm_struct = localtime(&current_time);
    }
};

using time_obj_t = time_obj;

#endif // TIME_MODEL_H
