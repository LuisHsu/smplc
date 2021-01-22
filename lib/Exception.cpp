#include <Exception.hpp>

Exception::Exception(std::string message):
    message(message)
{
}
const char* Exception::what(){
    return message.c_str();
}