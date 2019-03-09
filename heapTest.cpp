#include <iostream>
#include <string>
#include <utility>
#include "heap.h"

int main()
{
    BinaryHeap <string, int> sample;
    sample.insert("is", 1);
    sample.insert("bigBrother", 0);
    sample.insert("you", 3);
    sample.insert(".", 4);
    sample.insert("watching", 2);
    pair <string, int> min_01 = sample.min();
    sample.popMin();
    pair <string, int> min_02 = sample.min();
    sample.popMin();
    pair <string, int> min_03 = sample.min();
    sample.popMin();
    pair <string, int> min_04 = sample.min();
    sample.popMin();
    pair <string, int> min_05 = sample.min();
    cout << "where tha fuck am I ? ---> " << min_01.first << " " << min_02.first << " " << min_03.first <<
        " " << min_04.first << min_05.first << endl;
    return 0;
}