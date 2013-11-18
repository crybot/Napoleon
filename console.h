#ifndef CONSOLE_H
#define CONSOLE_H
#include <string>

// deprecated system-dependent console output colors
namespace Console
{
    const std::string Green = "\e[32m";
    const std::string Red = "\e[31m";
    const std::string Yellow = "\e[33m";
    const std::string Reset = "\e[m";
}

#endif // CONSOLE_H
