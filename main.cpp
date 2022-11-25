#include <iostream>
#include <chrono>
#include "memmanager.h"
#include "complexpart.h"

static const int testArraySize = 2000;

int main()
{
    ComplexObject* array[testArraySize];

    auto programStart = std::chrono::system_clock::now();
    auto programEnd = std::chrono::system_clock::now();

    for (int i = 0; i < 10000; i++)
    {
        for (int j = 0; j < testArraySize; j++)
        {
            array[j] = new ComplexObject(i, j);
        }
        for (int j = 0; j < testArraySize; j++)
        {
            delete array[j];
        }
    }
    programEnd = std::chrono::system_clock::now();
    std::chrono::duration<double> programDuration = programEnd - programStart;

    std::cout << "Elapsed time: " << programDuration.count() << " seconds\n";
    return 0;
}