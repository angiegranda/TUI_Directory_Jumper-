#include "hello_functions.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept>

int main(int argc, char * * argv)
{
    try {
        say_hello();
    }
    catch (const std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown exception." << std::endl;
    }
    return 0;   // return code 0 required for recodex success
}