TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

HEADERS += \
    department.h \
    employee.h \
    heap/binomial_heap.h \
    heap/comparator.h \
    heap/exception.h \
    heap/fibonacci_heap.h \
    heap/heap.h \
    heap/heap_exception.h \
    logger.h \
    model.h \
    request.h \
    time_model.h
