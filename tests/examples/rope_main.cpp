#include "rope.hpp"
#include <iostream>

int main()
{
    const char* str = "Hello, World!!";
    const char* other = "Lewin";

    examples::rope r(str);
    r.insert(5,other, 5);
    r.insert(6,other, 5);
    r.insert(7,other, 5);
    r.insert(13,other, 5);
    r.append(str, 14);
    r.debug();

    char* buf = new char[128];
    r.copy(buf,127);

    std::cout << buf << std::endl << std::endl;

    for ( unsigned int i = 0; i < r.size(); i++ )
        std::cout << r.at(i);
    std::cout << std::endl << std::endl;
    delete[] buf;

    examples::rope r2(20,'a');
    r2.debug();

    return 0;
}