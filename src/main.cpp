#include <cstddef>
#include <cstdlib>
#include <string>
#include <chrono>
#include <iostream>

#include "TreeMap.h"
#include "HashMap.h"

#define REPEAT_COUNT 10000

namespace
{

template <typename K, typename V>
using Map = aisdi::TreeMap<K, V>;

void performTreeMapAdditionTest(unsigned int repeatCount) {
    aisdi::TreeMap<int, int> collection;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    for (std::size_t i = 0; i < repeatCount; ++i)
        collection[i] = repeatCount - i;

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout<<"[ TreeMapAddition ]\t\t" << duration <<" [ms]"<<std::endl;
}

void performHashMapAdditionTest(unsigned int repeatCount) {
    aisdi::HashMap<int, int> collection;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    for (std::size_t i = 0; i < repeatCount; ++i)
        collection[i] = repeatCount - i;

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout<<"[ HashMapAddition ]\t\t" << duration <<" [ms]"<<std::endl;
}

void performTreeMapIterationTest(unsigned int repeatCount) {
    aisdi::TreeMap<int, int> collection;
    for (std::size_t i = 0; i < repeatCount; ++i)
        collection[i] = repeatCount - i;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    for(auto it = collection.cbegin(); it != collection.cend(); ++it) {}

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout<<"[ TreeMapIteration ]\t\t" << duration <<" [ms]"<<std::endl;
}

void performHashMapIterationTest(unsigned int repeatCount) {
    aisdi::HashMap<int, int> collection;
    for (std::size_t i = 0; i < repeatCount; ++i)
        collection[i] = repeatCount - i;

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    for(auto it = collection.begin(); it != collection.end(); ++it) {}

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout<<"[ HashMapIteration ]\t\t" << duration <<" [ms]"<<std::endl;
}


} // namespace

int main(int argc, char** argv)
{
  const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : REPEAT_COUNT;

    performTreeMapAdditionTest(repeatCount);
    performHashMapAdditionTest(repeatCount);

    performTreeMapIterationTest(repeatCount);
    performHashMapIterationTest(repeatCount);

  return 0;
}
