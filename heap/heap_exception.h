#ifndef HEAP_EXCEPTION_H
#define HEAP_EXCEPTION_H
#include <iostream>


class heap_exception : public std::exception
{
private:
    std::string exception_message;

public:
    heap_exception();
    void set_exception_message(std::string exception_message);
    const char *what() const noexcept;
};

heap_exception::heap_exception()
{
}

void heap_exception::set_exception_message(std::string exception_message)
{
    this->exception_message = exception_message;
}

const char* heap_exception::what() const noexcept
{
    return exception_message.c_str();
}

#endif // HEAP_EXCEPTION_H
