#ifndef DEPARTMENT_H
#define DEPARTMENT_H

//модель функционирования одного отделения банка

#include <iostream>
#include "employee.h"
#include "logger.h"
#include "time_model.h"

namespace department {

    //логи для отделения будут писаться в файл по указанному пути
    //с добавлением в конце имени файла номера ("id") отделения
    extern const std::string log_file_path = "C:\\bank_log\\log_department_";

    enum status
    {
        opened, //отделение открыто
        busy, //отделение занято (перегружено)
        closed //отделение закрыто
    };

    size_t get_busy_time()
    //получение случайного времени нахождения отделения
    //в перегруженном состоянии (от 1 до 3 часов)
    {
        return get_random_value(60, 180);
    }

    using status_t = status;

    class department
    //класс "отделение банка"
    {
    private:
        size_t department_id;
        status_t status = closed; //статус отделения
        size_t busy_time = 0; //счетчик времени нахождения в занятом (перегруженном) состоянии
        float overhead = 0; //значение загрузки отделения
        std::vector<employee::employee_t*> employees; //массив объектов типа "сотрудник"
        logger *logger_obj; //объект логгера, связанный с отделением

        size_t get_employee_num()
        //возвращает число сотрудников в отделении
        {
            return this->employees.size();
        }

        size_t get_busy_employee_num()
        //возвращает число занятых сотрудников
        {
            size_t busy_employee_num = 0;
            for(size_t i = 0; i < this->employees.size(); i++)
            {
                if (this->employees[i]->get_status() == employee::status::busy)
                {
                    busy_employee_num++;
                }
            }
            return busy_employee_num;
        }

        size_t get_free_employee_num()
        //возвращает число свободных (не занятых) сотрудников
        {
            size_t busy_employee_num = 0;
            for(size_t i = 0; i < this->employees.size(); i++)
            {
                if (this->employees[i]->get_status() == employee::status::free)
                {
                    busy_employee_num++;
                }
            }
            return busy_employee_num;
        }

        size_t get_lunch_break_employee_num()
        //возвращает число сотрудников, находящихся на перерыве
        {
            size_t busy_employee_num = 0;
            for(size_t i = 0; i < this->employees.size(); i++)
            {
                if (this->employees[i]->get_status() == employee::status::lunch_break)
                {
                    busy_employee_num++;
                }
            }
            return busy_employee_num;
        }

        size_t get_free_employee_id()
        //возвращает идентификатор (номер) свободного сотрудника
        {
            for(size_t i = 0; i < this->employees.size(); i++)
            {
                if (this->employees[i]->get_status() == employee::status::free)
                {
                    return i;
                }
            }
            return 0;
        }

        bool is_all_employees_absent()
        //проверяет все ли сотрудники покинули отделение
        {
            for(size_t i = 0; i < employees.size(); i++)
            {
                if (employees[i]->get_status() != employee::status::absent)
                {
                    return false;
                }
            }
            return true;
        }

    public:
        department(size_t employee_num, size_t department_id) : department_id(department_id)
        {
            //формируем строку с именем пути и файла для записи лога работы отделения
            std::string log_file_name = (log_file_path + std::to_string(this->department_id));
            //создаем объект логгера с использованием паттерна "строитель"
            //и настраиваем его с использованием паттерна "цепочка обязанностей"
            logger_builder *logger_builder_obj = new logger_builder;
            this->logger_obj = logger_builder_obj->add_stream(log_file_name, FATAL).create();
            delete logger_builder_obj;
            this->status = status::closed;
            //заполняем массив employees объектами типа "сотрудник" ("employee")
            for(size_t i = 0; i < employee_num; i++)
            {
                this->employees.push_back(new employee::employee(this->logger_obj, i));
            }
        }

        bool execute(time_obj_t &time_obj, request::request_t *request_obj_input)
        //метод, реализующий одну итерацию рабочего цикла отделения
        {
            //открытие отделения (в 8.00)
            if (time_obj.get_date_time()->tm_hour == 8 && this->status == status::closed)
            {
                this->status = status::opened;
                this->logger_obj->write_log((std::string("The department is opened.")), time_obj.get_date_time_str());
            }
            //закрытие отделения (после 21.00 или после того, как все сотрудники уйдут)
            if (time_obj.get_date_time()->tm_hour >= 21 && (this->status == status::opened || this->status == status::busy) && this->is_all_employees_absent())
            {
                this->status = status::closed;
                this->logger_obj->write_log((std::string("The department is closed.")), time_obj.get_date_time_str());
            }
            //одна итерация рабочего цикла каждого сотрудника
            for(size_t i = 0; i < employees.size(); i++)
            {
                this->employees[i]->execute(time_obj);
            }
            //уменьшение счетчик времени перегрузки
            if (this->busy_time > 0)
            {
                this->busy_time--;
            }
            //завершение перегрузки отделения
            if (this->busy_time == 0 && this->status == status::busy)
            {
                this->status = status::opened;
                this->logger_obj->write_log((std::string("The department is finished the overhead.")), time_obj.get_date_time_str(), ERROR);
            }
            //считаем загрузку отделения
            if (this->status == status::opened)
            {
                overhead = static_cast<float>(get_busy_employee_num()) /
                        (static_cast<float>(get_free_employee_num()) + static_cast<float>(get_lunch_break_employee_num()));
            }
            //при наличии заявки наличии хотя бы одного свободного
            //сотрудника отдаем заявку свободному сотруднику
            if (request_obj_input && this->status == status::opened && this->get_free_employee_num() != 0)
            {
                this->employees[this->get_free_employee_id()]->set_request(*request_obj_input, time_obj);
                //заявка передана сотруднику и выполнена
                return true;
            }
            //заявка не передана сотруднику и не выполнена (либо не было
            //свободных сотрудников, либо заявка, поступившая в отделение пустая)
            return false;
        }

        logger *get_logger_ptr()
        //возвращает указатель на объект логгера, связанный с отделением
        {
            return this->logger_obj;
        }

        float get_overhead()
        //возвращает значение загрузки отделения
        {
            return overhead;
        }
        void set_overhead(float overhead)
        //устанавливает значение загрузки отделения
        {
            this->overhead = overhead;
        }

        status_t get_status()
        //возвращает значение сатуса отделения
        {
            return this->status;
        }
        void set_status(status_t status)
        //устанавливает значение сатуса отделения
        {
            this->status = status;
        }

        void set_busy_time(size_t busy_time)
        {
            this->busy_time = busy_time;
        }
    };

    using department_t = department;
}
#endif // DEPARTMENT_H
