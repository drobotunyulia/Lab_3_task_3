#ifndef MODEL_H
#define MODEL_H

//Общая модель функционирования всех отделений банка

#include <iostream>
#include <vector>
#include "request.h"
#include "employee.h"
#include "department.h"
#include "time_model.h"
#include "heap/fibonacci_heap.h"

namespace bank_model {

    //число сотрудников в отделении (одинаковое для всех
    //отделений, что не совсем соответствует условию, в
    //случае необходимости можно достаточно просто делать
    //число сотрудников в отделении случайным)
    const size_t employee_num = 10;
    //вероятность перегрузки отделения (по условию это "l")
    const float overhead_probability = 0.8;

    bool is_overhead()
    //параметр определяющий выход отделения на перегрузку
    {
        float inner_probability = 1.0 / 780.0 * overhead_probability * 1000000.0;
        size_t inner_value = get_random_value(0, 1000000);
        if (inner_value >= 0 && inner_value <= static_cast<size_t>(inner_probability))
        {
            return true;
        }
        return false;
    }

    bool is_request()
    //параметр определяющий интенсивность поступления заявок к очередь
    {
        return !static_cast<bool>(get_random_value(0, 10));
    }

    class bank_model {
    private:
        //массив объектов типа "отделение банка"
        std::vector<department::department_t*> departments;
        //массив объектов типа "фибоначиева куча" для реализации очереди заявок
        //для каждого отделения банка своя фибоначиева куча и, соответственно
        //своя очередь заявок
        std::vector<fibonacci_heap<request::emergency_t, request::request_t *>*> request_queues;
        //массив значений загруженности отделений банка (нужен для определения наименее
        //загруженного отделения путем поиска индекса минимального элемента в этом массиве)
        std::vector<float> overheads;
        //указатель на объект типа "генератор заявок" (паттерн "абстрактная фабрика")
        request::request_generator_t *request_generator_obj;
        //указатель на объект типа "модель времени"
        time_obj_t *time_model_obj;
        //число отделений банка
        size_t department_num;
        //максимальное число перегруженных отделений
        size_t max_overhead_department_num;

        size_t get_small_overhead_department()
        //получаем номер наименее загруженного отделения
        {
            return std::distance(this->overheads.begin(), std::min_element(this->overheads.begin(), this->overheads.end()));
        }

        size_t get_overhead_department_num()
        //получаем число перегруженных отделений
        {
            size_t result = 0;
            for (size_t i = 0; i < this->department_num; i++)
            {
                if (this->departments[i]->get_status() == department::status::busy)
                {
                    result++;
                }
            }
            return result;
        }
    public:
        bank_model(size_t department_num, size_t max_overhead_department_num = 0) :
            department_num(department_num),
            max_overhead_department_num(max_overhead_department_num)
        {
            this->time_model_obj = new time_obj;
            //получаем объект генератора заявок
            this->request_generator_obj = new request::request_generator();
            //объект "comparator" для реализации фибоначиевой кучи
            comparator<request::emergency_t> *key_comparator = new comparator<request::emergency_t>;
            for(size_t i = 0; i < this->department_num; i++)
            {
                //помещаем очередной объект типа "отделение банка" в массив departments
                this->departments.push_back(new department::department(employee_num, i));
                //помещаем очередной объект типа "фибоначиева куча", на базе которого
                //реалиуется очередь заявок для каждого отделения в массив очередей (request_queues)
                this->request_queues.push_back(new fibonacci_heap<request::emergency_t, request::request_t *>(key_comparator));
            }
        }
        void execute()
        //одна итерация цикла функционирования отделения банка
        {
            //признак успешного выполнения заявки
            bool is_request_completed = false;
            //указатель на входящую заявку из очереди заявки
            request::request_t *request_obj_input = nullptr;
            //признак "заявка добавлена"
            bool is_add_request = true;
            //
            this->overheads.clear();
            //обновляем массив "overheads" со значениями
            //загруженности каждого отделения
            for(size_t i = 0; i < this->department_num; i++)
            {
                this->overheads.push_back(this->departments[i]->get_overhead());
            }
            //цикл создания заявки, отправки в очередь для каждого отделения
            //и передачи заявки из очереди в отделение с учетом приотритета заявки
            //(чем меньше значение приоритета заявки ("emergency"), тем приоритетнее
            //заявка)
            for(size_t i = 0; i < this->department_num; i++)
            {
                //заявки формируются с учетом интенсивности и рабочего времени отделения
                if (is_request() && this->departments[i]->get_status() != department::status::closed &&
                        time_model_obj->get_date_time()->tm_hour < 21 &&
                        time_model_obj->get_date_time()->tm_hour > 8)
                {
                    //формируем заявку (работает паттерн "абстрактная фабрика")
                    request::request_t *request_obj = this->request_generator_obj->create_request(time_model_obj->get_date_time(), i);
                    try
                    {
                        //помещаем завку в очередь заявок
                        this->request_queues[i]->insert(request_obj->emergency, request_obj);
                    }
                    catch (heap_exception &exception)
                    {
                        //если заявка с таким приоритетом уже существует
                        delete request_obj;
                        request_obj = nullptr;
                        is_add_request = false;
                    }
                    if (is_add_request && this->departments[i]->get_status() != department::status::closed)
                    {
                        //пишем в лог о факте добавления заявки в очередь для соответствующего отделения
                        std::string log_string = ("The \"" + request_obj->request_text +
                                                  "\" request with priority " +
                                                  std::to_string(request_obj->emergency) +
                                                  " has been added to the queue.");
                        this->departments[i]->get_logger_ptr()->write_log(log_string, this->time_model_obj->get_date_time_str(), WARNING);
                    }
                }
                try
                {
                    //достаем из очереди очередную заявку
                    request_obj_input = request_queues[i]->get_minimum();
                }
                catch (heap_exception &exception)
                {
                    //если заявок в очереди нет, то делаем заявку пустой
                    request_obj_input = nullptr;
                }
                if (this->departments[i]->get_status() != department::status::busy)
                //если отделение не перегружено, то делаем
                //итерацию работы отделения с заявкой
                {
                    is_request_completed = this->departments[i]->execute(*time_model_obj, request_obj_input);
                }
                else if (this->departments[i]->get_status() == department::status::busy)
                //если отделение перегружено, то делаем
                //итерацию работы отделения с пустой заявкой
                {
                    is_request_completed = this->departments[i]->execute(*time_model_obj, nullptr);
                }
                //если заявка не пустая и не выполнена, то
                //ее нужно отправить другому отделению
                if (request_obj_input && !is_request_completed && this->departments[i]->get_status() == department::status::busy)
                {
                    try
                    {
                        //помещаем необработанную заявку в очередь
                        //наименее перегруженного отделения
                        this->request_queues[get_small_overhead_department()]->insert(request_obj_input->emergency, request_obj_input);
                        //пишем в лог о перенаправлении заявки
                        std::string log_string = ("The \"" + request_obj_input->request_text +
                                                  "\" request with priority " +
                                                  std::to_string(request_obj_input->emergency) +
                                                  " has been redirect to " +
                                                  std::to_string(get_small_overhead_department()) +
                                                  " departments.");
                        this->departments[i]->get_logger_ptr()->write_log(log_string, this->time_model_obj->get_date_time_str(), FATAL);
                        log_string = ("The \"" + request_obj_input->request_text +
                                      "\" request with priority " +
                                      std::to_string(request_obj_input->emergency) +
                                      " from department " +
                                      std::to_string(i) +
                                      " has been added to the queue.");
                        this->departments[get_small_overhead_department()]->get_logger_ptr()->write_log(log_string, this->time_model_obj->get_date_time_str(), FATAL);
                        is_request_completed = true;
                    }
                    catch (heap_exception &exception)
                    {
                    //если заявка не попала в очередь наименее перегруженного отделения
                    //она будет ожидать до тех пор пока в очереди не освободится место
                    //для этой заявки (из нее будет извлечена заявка с таким же приоритетом)
                    }
                }
                //если заявка отправлена в отделение и выполнена, то ее надо удалить из очереди
                if (request_obj_input && is_request_completed)
                {
                    try
                    {
                        request_queues[i]->remove_minimum();
                    }
                    catch (heap_exception &exception)
                    {
                    //вообще это исключение "на всякий случай" (по логике программы
                    //оно не должно срабатывать
                    }
                }
                //устанавливаем какое либо отделение в перегруженное состояние
                //с учетом "overhead_probability" и "max_overhead_department_num"
                if (is_overhead())
                {
                    //случайно выбираем отделение, для которого будет установлена прегрузка
                    size_t department_overhead_id = get_random_value(0, this->department_num - 1);
                    //время, на которое прекратит работу отделение
                    //(случайное, в пределах от 60 до 180 минут)
                    size_t overhead_time = get_random_value(60, 180);
                    //проверка условий выхода отделения в перегруженное состояние
                    if (this->departments[department_overhead_id]->get_status() == department::status::opened &&
                            this->get_overhead_department_num() <= this->max_overhead_department_num &&
                            time_model_obj->get_date_time()->tm_hour + (overhead_time / 60) < 21 &&
                            time_model_obj->get_date_time()->tm_hour > 8)
                    {
                        this->departments[department_overhead_id]->set_status(department::status::busy);
                        this->departments[department_overhead_id]->set_busy_time(overhead_time);
                        //устанавливаем максимальное значение "overhead" для отделения
                        //(чтобы перегруженное отделение не было выбрано для приема переадресованных заявок)
                        this->departments[department_overhead_id]->set_overhead(1.0);
                        this->departments[department_overhead_id]->get_logger_ptr()->write_log("The department is overhead.", this->time_model_obj->get_date_time_str(), ERROR);
                    }
                }
            }
            //делаем одну итерацию рабочего цикла объекта модели времени
            //(увеличение времени на 1 минуту)
            time_model_obj->execute();
        }
    };
}

#endif // MODEL_H
