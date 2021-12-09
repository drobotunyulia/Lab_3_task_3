#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>

enum severity_t
{
    UNKNOWN,
    TRACE,
    DEBUG,
    INFORMATION,
    WARNING,
    ERROR,
    FATAL
};

struct stream
{
    std::string name;
    severity_t severity;
};

std::string get_date_time()
{
    std::string result;
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    result = asctime(timeinfo);
    result = result.erase(result.size() - 1, result.size());
    return result;
}

std::string get_severity_name(severity_t severity)
{
    std::string result;
    switch (severity) {
    case TRACE:
        result = "Trace";
        break;
    case DEBUG:
        result = "Debug";
        break;
    case INFORMATION:
        result = "Information";
        break;
    case WARNING:
        result = "Warning";
        break;
    case ERROR:
        result = "Error";
        break;
    case FATAL:
        result = "Fatal";
        break;
    default:
        result = "Unknown";
        break;
    }
    return result;
}

class stream_node
{
public:
    std::string stream_name;
    std::ofstream stream;
    severity_t severity;
    stream_node *next;
    stream_node(std::string stream_name, severity_t severity) : stream_name(stream_name), severity(severity), next(nullptr)
    {
        if (stream_name != "stdout")
        {
            this->stream.open(this->stream_name, std::ios_base::app);
        }
    }
    ~stream_node()
    {
        this->stream.close();
    }
};

class streams
{
private:
    stream_node *head;
public:
    stream_node *get_head()
    {
        return head;
    }
    streams() : head(nullptr)
    {}
    ~streams()
    {
        while (this->head)
        {
            this->remove();
        }
    }
    void append(std::string stream_name, severity_t severity)
    {
        stream_node *new_node = new stream_node(stream_name, severity);
        new_node->next = this->head;
        this->head = new_node;
    }
    void remove()
    {
        if (this->head)
        {

            stream_node *new_head = head->next;
            delete head;
            head = new_head;
        }
    }
};

class logger
{
private:
    streams *streams_ptr = nullptr;
public:
    logger(std::vector<stream> stream_list)
    {
        this->streams_ptr = new streams();
        for (unsigned int i = 0; i < stream_list.size(); i++)
        {
            streams_ptr->append(stream_list[i].name, stream_list[i].severity);
        }

    }
    ~logger()
    {
        delete streams_ptr;
    }
    void write_log(std::string log_string, std::string log_date_time, severity_t severity = INFORMATION)
    {
        stream_node *current_node = streams_ptr->get_head();
        while (current_node)
        {
            if (severity <= current_node->severity && current_node->stream_name != "stdout")
            {
                current_node->stream << "[" << log_date_time << "][" << get_severity_name(severity) << "]:" << log_string << std::endl;
            }
            if (severity <= current_node->severity && current_node->stream_name == "stdout")
            {
                std::cout << "[" << log_date_time << "][" << get_severity_name(severity) << "]:" << log_string << std::endl;
            }
            current_node = current_node->next;
        }
    }
};

class logger_builder
{
public:
    std::vector<stream> stream_list;
    logger *create()
    {
        logger *result_logger = new logger(stream_list);
        stream_list.clear();
        return result_logger;
    }
    logger_builder &add_stream(const std::string& stream_name, severity_t severity)
    {
        stream stream_pair;
        stream_pair.name = stream_name;
        stream_pair.severity = severity;
        if (!stream_name.empty() && severity != UNKNOWN)
        {
            stream_list.push_back(stream_pair);
        }
        return *this;
    }
};

#endif // LOGGER_H
