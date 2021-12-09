#ifndef HEAP_H
#define HEAP_H
#include <sstream>
#include <iostream>
#include <string>
#include <set>
#include "comparator.h"
#include "heap_exception.h"

template<class TKey, class TValue>
struct Node
{
    TKey key;
    TValue value;
    int degree;//количество дочерних узлов
    Node* parent;//указатель на родительский узел
    Node* child;//указатель на крайний левый дочерний узел
    Node* right;//указатель на правый сестринский узел
    Node* left;//указатель на левый сестринский узел
    bool marked;//перемещались ли ранее потомки этого узла
};

template <class TKey, class TValue>
class heap
{
protected:
    comparator<TKey> *key_comparator;
    class insert_error_exception : public heap_exception
    {
    public:
        insert_error_exception(TKey key);
    };
    class get_minimum_error_exception : public heap_exception
    {
    public:
        get_minimum_error_exception();
    };
    class remove_minimum_error_exception : public heap_exception
    {
    public:
        remove_minimum_error_exception();
    };
    class union_heap_error_exception : public heap_exception
    {
    public:
        union_heap_error_exception(TKey key);
        union_heap_error_exception();
    };
public:
    std::set<TKey> keys;
    Node<TKey, TValue> *heap_ptr = nullptr;
    int heap_size = 0;
    virtual void make_heap(comparator<TKey> *key_comparator) = 0;
    virtual void insert(const TKey key, const TValue value) = 0;
    virtual TValue get_minimum() = 0;
    virtual void remove_minimum() = 0;
    virtual void union_heap(heap<TKey, TValue>& H_2) = 0;
    virtual ~heap()
    {

    };
};
template <class TKey, class TValue>
heap<TKey, TValue>::insert_error_exception::insert_error_exception(TKey key)
{
    std::stringstream key_string;
    key_string << key;
    std::string exception_message = "Insert error. Element with key \"" + key_string.str() + "\" already exists.";
    set_exception_message(exception_message);
}
template <class TKey, class TValue>
heap<TKey, TValue>::get_minimum_error_exception::get_minimum_error_exception()
{
    std::string exception_message = "Get error. You cannot get element from empty heap.";
    set_exception_message(exception_message);
}
template <class TKey, class TValue>
heap<TKey, TValue>::remove_minimum_error_exception::remove_minimum_error_exception()
{
    std::string exception_message = "Remove error. You cannot remove element with key from empty heap.";
    set_exception_message(exception_message);
}
template <class TKey, class TValue>
heap<TKey, TValue>::union_heap_error_exception::union_heap_error_exception(TKey key)
{
    std::stringstream key_string;
    key_string << key;
    std::string exception_message = "Union error. You cannot union heap with key \"" + key_string.str() + "\" .";
    set_exception_message(exception_message);
}
template <class TKey, class TValue>
heap<TKey, TValue>::union_heap_error_exception::union_heap_error_exception()
{
    std::string exception_message = "Union error. You cannot union heap with same keys.";
    set_exception_message(exception_message);
}

template <class TKey, class TValue>
class heap_creator
{
public:
    virtual heap<TKey, TValue>* create(comparator<TKey> *key_comparator) = 0;
    virtual ~heap_creator()
    {}
};
#endif // HEAP_H
