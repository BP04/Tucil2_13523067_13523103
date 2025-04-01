#ifndef IMAGE_LOAD_EXCEPTION_HPP
#define IMAGE_LOAD_EXCEPTION_HPP

#include <stdexcept>
#include <string>

class ImageLoadException : public std::runtime_error
{
public:
    explicit ImageLoadException(const std::string &msg)
        : std::runtime_error(msg) {}
};

#endif