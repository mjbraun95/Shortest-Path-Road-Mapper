#ifndef _HEAP_H_
#define _HEAP_H_

#include <assert.h>
#include <cmath> // for floor
#include <vector>
#include <utility> // for pair

using namespace std;

// T is the type of the item to be held
// K is the type of the key associated with each item in the heap
// The only requirement is that K is totally ordered and comparable via <
template <class T, class K>
class BinaryHeap {
public:
  // constructor not required because the only "initialization"
  // is through the constructor of the variable "heap" which is called by default

  // return the minimum element in the heap
  std::pair<T, K> min() const {
    return heap[0];
  }

  // insert an item with the given key
  // if the item is already in the heap, will still insert a new copy with this key
  void insert(const T& item, const K& key);

  // pop the minimum item from the heap
  void popMin();

  // returns the number of items held in the heap
  int size() const {
    return heapSize;
  }

private:
  // the array holding the heap
  std::vector< std::pair<T, K> > heap;

  // # of items held in the heap
  int heapSize = 0;

  // the parent of index i
  int parent(int i) {
    return floor(((double) i - 1)/2);
  }

  // the left child of index i
  int lChild(int i) {
    return 2 * i + 1;
  }

  // the right child of index i
  int rChild(int i) {
    return 2 * i + 2;
  }

  // swap the contents between two pairs 
  void swap(pair<T, K>& a, pair<T, K>& b) {
    pair<T, K> temp = a;

    a = b;
    b = temp;
  }
};

// insert an item with the given key
// if the item is already in the heap, will still insert a new copy with this key
template <class T, class K>
void BinaryHeap<T, K>::insert(const T& item, const K& key) {
  // let v be a new vertex with the pair (item, key) in the tree
  pair<T, K> v;
  v.first = item;
  v.second = key;

  // add v into the next new location
  heap.push_back(v);
  heapSize += 1;

  // i: index of v
  int i = heapSize - 1;
  // while v is not the root and key(v) < key(parent(v)) do:
  while (i != 0 && heap[i].second < heap[parent(i)].second ){
    // swap the items and keys between v and parent(v)
    swap(heap[i], heap[parent(i)]);

    // v <- parent(v) (crawl up the tree)
    i = parent(i);
  }
}

// pop the minimum item from the heap
template <class T, class K>
void BinaryHeap<T, K>::popMin() {
  // suppose the heap is not empty
  assert(heapSize > 0);

  // swap the items and keys between the root and the last vertex
  swap(heap[0], heap[heapSize - 1]);

  // pop the last item from the list
  heap.pop_back();
  heapSize -= 1;

  // if # of items <= 2
  if (heapSize <= 2) {
    if (heap[0].second > heap[heapSize - 1].second) {
      swap(heap[0], heap[heapSize - 1]);
    }
    return;
  }

  // general case
  // i: index of v
  int i = 0;
  // while the heap property is violated at v do
  while (heap[i].second > heap[lChild(i)].second || heap[i].second > heap[rChild(i)].second) {
    // swap the item and key between u and child of v with smallest key
    if (heap[lChild(i)].second < heap[rChild(i)].second) {
      swap(heap[i], heap[lChild(i)]);

      // v <- child of v with smallest key
      i = lChild(i);
    }
    else {
      swap(heap[i], heap[rChild(i)]);

      i = rChild(i);
    }

    // if v only has one child
    if (lChild(i) == heapSize - 1) {
      if (heap[i].second > heap[heapSize - 1].second) {
        swap(heap[i], heap[heapSize - 1]);
      }

      break;
    }
    // if v has no child
    else if (lChild(i) > heapSize - 1) {
      break;
    }
  }
}

#endif
