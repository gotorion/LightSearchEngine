#include <iostream>

#include "Time.h"
#include "dictionary.h"
int main() {
    DicProducer m;
    Time t;
    m.buildEnDict();
    m.buildIndexen();
    m.buildCnDict();
    m.buildIndexcn();
    std::cout << "建立词典库用时：";
    t.end();
}