#ifndef EMPLOYEE_H
#define EMPLOYEE_H

//модель функционирования одного сотрудника

#include <iostream>
#include "logger.h"
#include "time_model.h"
#include "request.h"

extern const size_t expected_time_request = 2;
extern const size_t max_deviation_time_request = 1;

namespace employee {

    enum status
    {
        free, //сотрудник свободен (на работе и не занят обработкой заявки)
        lunch_break, //сотрудник на перерыве
        busy, //сотрудник занят обработкой заявки
        absent //сотрудник ушел с работы
    };

    using status_t = status;

    size_t get_time_request(request::complexity_t complexity)
    //возвращает время обработки заявки сотрудником
    //с учетом сложности и случайного отклонения
    {
        size_t result;
        size_t derivation_time_request = get_random_value(0, max_deviation_time_request);
        bool sign_derivation = static_cast<bool>(get_random_value(0, 1));
        if (sign_derivation)
        {
            result = expected_time_request + derivation_time_request + (static_cast<size_t>(complexity) * 2);
        }
        else
        {
            result = expected_time_request - derivation_time_request + (static_cast<size_t>(complexity) * 2);
        }
        return result;
    }

    bool is_arrival()
    //обеспечивает "случайность" прибытия сотрудников
    {
        return !static_cast<bool>(get_random_value(0, 10));
    }

    bool is_left()
    //обеспечивает "случайность" убытия сотрудников
    {
        return !static_cast<bool>(get_random_value(0, 30));
    }
    bool is_break_first()
    //обеспечивает "случайность" убытия сотрудников
    //на перерыв в первой половине дня с шансом 0.15
    {
        size_t inner_value = get_random_value(0, 1000000);
        if (inner_value >= 0 && inner_value <= 625)
        {
            return true;
        }
        return false;
    }

    bool is_break_second()
    //обеспечивает "случайность" убытия сотрудников
    //на перерыв во второй половине дня с шансом 0.75
    {
        size_t inner_value = get_random_value(0, 1000000);
        if (inner_value >= 0 && inner_value <= 3125)
        {
            return true;
        }
        return false;
    }

    class employee
    //класс "сотрудник"
    {
    private:
        status_t status = absent; //статус сотрудника
        tm *arrival_time; //время прибытия
        size_t processing_time = 0; //счетчик времени обработки заявки
        size_t break_time = 0; //счетчик времени нахождения на перерыве
        bool is_was_break = false; //признак "был или не был на перерыве"
        logger *logger_obj; //объект логгера, связанный с сотрудником
        size_t id; //идентификатор сотрудника (порядковый номер)
    public:
        employee(logger *logger_obj, size_t id) : logger_obj(logger_obj), id(id)
        {
            arrival_time = new tm;
            status = status::absent;
        }

        ~employee()
        {
            delete arrival_time;
        }

        status_t get_status()
        {
            return this->status;
        }

        void execute(time_obj &time_obj)
        //метод, реализующий одну итерацию рабочего цикла сотрудника
        {
            //прибытие сотрудника на работу с учетом "случайности" в период с 8.00 по 12.00
            if (this->status == status::absent &&
                    time_obj.get_date_time()->tm_hour >= 8 &&
                    time_obj.get_date_time()->tm_hour <= 12 &&
                    is_arrival())
            {
                this->status = status::free;
                this->arrival_time->tm_hour = time_obj.get_date_time()->tm_hour;
                this->arrival_time->tm_min = time_obj.get_date_time()->tm_min;
                this->logger_obj->write_log((std::string("The employee ") +
                                             std::to_string(this->id) +
                                             std::string(" has arrived.")), time_obj.get_date_time_str());
            }
            //убытие сотрудника с работы с учетом "случайности" в период с 17.00 по 21.00
            if (this->status == status::free &&
                    time_obj.get_date_time()->tm_hour >= 17 &&
                    time_obj.get_date_time()->tm_hour <= 21 &&
                    is_left())
            {
                this->status = status::absent;
                this->is_was_break = false;
                this->logger_obj->write_log((std::string("The employee ") +
                                             std::to_string(this->id) +
                                             std::string(" left.")), time_obj.get_date_time_str());
            }
            //убытие сотрудника по окнчании работы отделения (после 21.00)
            if (this->status == status::free && time_obj.get_date_time()->tm_hour >= 21)
            {
                status = status::absent;
                this->is_was_break = false;
                this->logger_obj->write_log((std::string("The employee ") +
                                             std::to_string(this->id) +
                                             std::string(" left.")), time_obj.get_date_time_str());
            }
            //убытие сотрудника на перерыв в первой половине дня
            if (this->status == status::free &&
                    time_obj.get_date_time()->tm_hour >= arrival_time->tm_hour &&
                    time_obj.get_date_time()->tm_hour <= arrival_time->tm_hour + 4 &&
                    is_break_first() &&
                    !is_was_break)
            {
                this->status = status::lunch_break;
                this->break_time = 60;
                this->logger_obj->write_log((std::string("The employee ") +
                                             std::to_string(this->id) +
                                             std::string(" is break.")), time_obj.get_date_time_str());
            }
            //убытие сотрудника на обед во второй половине дня
            if (this->status == status::free &&
                    time_obj.get_date_time()->tm_hour >= arrival_time->tm_hour + 4 &&
                    time_obj.get_date_time()->tm_hour <= arrival_time->tm_hour + 8 &&
                    is_break_second() &&
                    !is_was_break)
            {
                this->status = status::lunch_break;
                this->break_time = 60;
                this->logger_obj->write_log((std::string("The employee ") +
                                             std::to_string(this->id) +
                                             std::string(" is break.")), time_obj.get_date_time_str());
            }
            //уменьшение счетчика времени нахождения на перерыве
            if (this->break_time > 0)
            {
                this->break_time--;
            }
            //завершение перерыва сотрудником
            if (this->break_time == 0 && this->status == status::lunch_break)
            {
                this->status = status::free;
                this->is_was_break = true;
                this->logger_obj->write_log((std::string("The employee ") +
                                             std::to_string(this->id) +
                                             std::string(" is finished the break.")), time_obj.get_date_time_str());
            }
            //уменьшение счетчика времени обработки заявки
            if (processing_time > 0)
            {
                processing_time--;
            }
            //завершение обработки заявки
            if (processing_time == 0 && this->status == status::busy)
            {
                this->status = status::free;
                this->logger_obj->write_log((std::string("The employee ") +
                                             std::to_string(this->id) +
                                             std::string(" has finished processing request.")), time_obj.get_date_time_str(), WARNING);
            }
        }

        void set_request(request::request_t &request_obj, time_obj_t &time_obj)
        //принять заявку на обработку
        {
            this->processing_time = get_time_request(request_obj.complexity);
            this->status = status::busy;
            this->logger_obj->write_log((std::string("The employee ") +
                                         std::to_string(this->id) +
                                         std::string(" has started processing the request \"" + request_obj.request_text + "\".")), time_obj.get_date_time_str(), WARNING);
        }
    };

    using employee_t = employee;
}

#endif // EMPLOYEE_H
