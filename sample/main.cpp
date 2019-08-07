#include "Application.h"
#include <iostream>

int main()
{
    try {
        Application app("HairGL", 1280, 720);
        app.Run();
    }
    catch (std::exception e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}