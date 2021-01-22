#ifndef SMPLC_Exception_DEF
#define SMPLC_Exception_DEF

#include <stdexcept>
#include <string>

class Exception: public std::exception{
private:
    std::string message;
public:
    Exception(std::string message);
    const char* what();
};


#endif