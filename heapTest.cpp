#include <iostream>
#include <string>
#include <utility>
#include "heap.h"

int main()
{
    BinaryHeap <string, int> sample;
    sample.insert("bigBrother", 4);
    sample.insert("is", 3);
    sample.insert("watching", 2);
    sample.insert("you", 1);
    pair <string, int> min_01 = sample.min();
    sample.popMin();
    pair <string, int> min_02 = sample.min();
    cout << "where tha fuck am I ? ---> " << min_01.first << " " << min_02.first;
    return 0;
}