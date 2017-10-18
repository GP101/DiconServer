#include <stdio.h>
#include <iostream>
#include <set>
#include <string>


void main()
{
    unsigned long a = 0x12345678;
    char* pc = (char*)&a;
    std::cout << std::hex << (int)pc[0] << std::endl;
}
