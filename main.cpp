#include <iostream>
#include <vector>
#include "request.h"
#include "employee.h"
#include "department.h"
#include "time_model.h"
#include "model.h"
#include "heap/fibonacci_heap.h"

using namespace std;

int main()
{
    //получаем экземпляр объекта типа "bank_model"
    //(общее число отделений банка 5, максимальное
    //число отделений, которые могут находится в
    //перегруженном состоянии - 3)
    bank_model::bank_model *bank_model_obj = new bank_model::bank_model(5, 3);
    cout << "The modeling process has begun..." << endl;
    for(size_t i = 0; i < 10080; i++)
    //цикл на 10080 минут работы отделений банка (это семь суток)
    {
        bank_model_obj->execute();
    }
    cout << "The modeling process is over..." << endl;
    return 0;
}
