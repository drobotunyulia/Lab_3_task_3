#ifndef REQUEST_H
#define REQUEST_H

//здесь все что касается генерации заявок

#include <iostream>
#include <ctime>
#include <cstdlib>
#include "time_model.h"

namespace request {

    //возможные значения приоритета заявки
    enum emergency {
        emergency_0,
        emergency_1,
        emergency_2,
        emergency_3,
        emergency_4,
        emergency_5,
        emergency_6,
        emergency_7,
        emergency_9
    };

    //возможные значения сложности заявки
    enum complexity {
        complexity_0,
        complexity_1,
        complexity_2,
        complexity_3,
        complexity_4,
        complexity_5
    };

    using emergency_t = emergency;
    using complexity_t = complexity;

    emergency_t get_emergency()
    //генерация случайного значения приотритета
    {
        return static_cast<emergency_t>(get_random_value(emergency::emergency_0, emergency::emergency_9));
    }

    complexity_t get_complexity()
    //генерация случайного значения сложности
    {
        return static_cast<complexity_t>(get_random_value(complexity_0, complexity_5));
    }

    std::string get_request_text()
    //генерация случайного значения текста заявки
    //(из пяти возможных вариантов)
    {
        std::string result;
        switch (get_random_value(1, 5)){
        case 1:
            result = "Credit request";
            break;
        case 2:
            result = "Transaction request";
            break;
        case 3:
            result = "Cash request";
            break;
        case 4:
            result = "Deposit request";
            break;
        default:
            result = "Currency request";
            break;
        }
        return result;
    }

    struct request_base
    //базовый класс заявки
    {
        //сложность заявки
        emergency_t emergency;
        //время создания заявки
        tm* time_created;
        //отделение, для которого предназначения заявка
        size_t department;
        //сложность заявки
        complexity_t complexity;
        request_base()
        {}
        request_base(emergency_t emergency,
                     tm*time_created,
                     size_t department,
                     complexity_t complexity) :
            emergency(emergency),
            time_created(time_created),
            department(department),
            complexity(complexity)
        {}
        virtual ~request_base()
        {}
    };

    struct request : public request_base
    //основной класс заявки (наследуется от базового)
    {
        //текст заявки
        std::string request_text;
        request(emergency_t emergency,
                tm* time_created,
                size_t department,
                complexity_t complexity,
                const std::string &request_text) :
            request_base(emergency, time_created, department, complexity),
            request_text(request_text)
        {}
        request(request &request_obj) : request_base()
        //конструктор копирования
        {
            this->emergency = request_obj.emergency;
            this->time_created = request_obj.time_created;
            this->department = request_obj.department;
            this->complexity = request_obj.complexity;
            this->request_text = request_obj.request_text;
        }
        ~request()
        {}
    };

    using request_t = request;

    class request_generator_base
    //абстрактный класс генератора заявок
    //(для реализации шаблона "абстрактная фабрика")
    {
    public:
        virtual request_base *create_request(tm* time_created, size_t department) = 0;
    };

    class request_generator : public request_generator_base
    //класс генератора заявок (наследуется от абстрактного базового класса)
    {
    public:
        request_t *create_request(tm* time_created, size_t department) override
        //метод создания заявки (поля "emergency", "complexity"
        //и "request_text" заполняются случайными значениями)
        {
            return new request(get_emergency(), time_created, department, get_complexity(), get_request_text());
        }
    };

    using request_generator_t = request_generator;
}

#endif // REQUEST_H
