#include <sstream>
#include "sort.h"

template <typename T, size_t S>
MinHeap<T, S>::MinHeap() : size_(0), pending_(0) {}

template <typename T, size_t S>
void MinHeap<T, S>::push(T item) {
  this->heap[this->size_] = item;
  this->size_ += 1;
  this->heapify();
}

template <typename T, size_t S>
T MinHeap<T, S>::pop() {
  T item = this->heap[0];
  this->heap[0] = this->heap[this->size()-1];
  this->size_ -= 1;
  this->heapify();
  return item;
}

template <typename T, size_t S>
size_t MinHeap<T, S>::pending() {
  return this->pending_;
}

template <typename T, size_t S>
void MinHeap<T, S>::inc_pending() {
  this->pending_ += 1;
}

template <typename T, size_t S>
void MinHeap<T, S>::reset_pending() {
  this->pending_ = 0;
  this->heapify();
}

template <typename T, size_t S>
size_t MinHeap<T, S>::size() {
  return size_ - pending_;
}

template <typename T, size_t S>
size_t MinHeap<T, S>::max_size() {
  return S;
}

template <typename T, size_t S>
void MinHeap<T, S>::heapify() {
  if (this->size() == 2 && heap[1] < heap[0]) {
    T tmp = heap[0];
    heap[0] = heap[1];
    heap[1] = tmp;
  } else if (this->size() >= 3) {
    for (size_t i = this->size()/2; i >= 2; i -= 1) {
      size_t parent_index = i - 1;
      size_t left_index = i * 2 - 1;
      size_t right_index = i * 2;
      T parent = this->heap[parent_index];
      T left = this->heap[left_index];
      T right = this->heap[right_index];

      // std::cout << parent_index << " " << left_index << " " << right_index << std::endl;

      if (right_index < this->size()) {
        if (left < right && left < parent) {
          heap[parent_index] = heap[left_index];
        } else if (right < left && right < parent) {
          heap[parent_index] = heap[right_index];
        }
      } else {
        if (left < parent) {
          heap[parent_index] = heap[left_index];
        }
      }
    }
  }
  // for (size_t i = 0; i < this->max_size(); i += 1) {
  //   std::cout << this->heap[i];
  //   if (i >= this->size()) {
  //     std::cout << " x";
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << std::endl;
}

template<typename T>
bool compare(T a, T b) {
  return a < b;
}

void heapify(int heap[], size_t size) {
  if (size < 1) {
    return;
  }
  if (size < 2 && heap[1] < heap[0]) {
    int temp = heap[0];
    heap[0] = heap[1];
    heap[1] = temp;
    return;
  }
  for (size_t i = size/2; i >= 2; i -=1) {
    int parent = heap[i];
    int left = heap[i * 2 - 1];

    if (i * 2 < size) {
      if (compare<int>(left, parent)) {
        heap[i] = left;
      }
    } else {
      int right = heap[i * 2];
      if (compare<int>(left, right) && compare<int>(left, parent)) {
        heap[parent] = left;
        heap[i * 2 - 1] = parent;
      } else if (compare<int>(right, left) && compare<int>(right, parent)) {
        heap[parent] = right;
        heap[i * 2] = parent;
      }
    }
  }
}

std::vector<int> sort(std::vector<int> vec) {
    const size_t heap_size = 3;
    int heap[heap_size];

    std::vector<int> buffer;
    std::vector<std::pair<size_t, size_t> > offsets;

    {
      size_t size = 0;
      size_t pending = 0;
      for (size_t i = 0; i < vec.size(); i += 1) {
        if (size < heap_size) {
          heap[size] = vec[i];
          heapify(heap, size);
          size += 1;
          continue;
        }

        int item = heap[0];                  // get smallest item in heap
        heap[0] = heap[size - pending - 1];  // move last in heap into beginning
        heap[size - pending - 1] = heap[i];  // move in new item to the end
        heapify(heap, size - pending);       // heapify

        if (item < vec[i]) {
          pending += 1;
        }
        buffer.push_back(item);
      }
    }

    std::vector<int> output;
    output = buffer;
    return output;
}
// std::vector<int> sort(std::vector<int> vec) {
//   const size_t heap_size = 3;
//   std::vector<int> buffer;
//   std::vector<std::pair<size_t, size_t> > offsets;
//   {
//     MinHeap<int, heap_size> heap;
//     int item;
//     size_t offset_ = 0;
//     size_t count = 0;
//     for (size_t i = 0; i < vec.size(); i += 1) {
//       if (heap.size() < heap.max_size()) {
//         heap.push(vec[i]);
//         continue;
//       }

//       item = heap.pop();
//       heap.push(item);

//       buffer.push_back(item);
//       std::cout << item << std::endl;

//       if (vec[i] < item) {
//         heap.inc_pending();
//         if (heap.pending() == heap.max_size()) {
//           offsets.push_back(std::pair<size_t, size_t>(offset_, count));
//           offset_ = i;
//           count = 0;
//           heap.reset_pending();
//         }
//       }

//       heap.push(vec[i]);
//     }

//     count += heap.size();
//     for (size_t i = 0; i < heap.size(); i += 1) {
//       item = heap.pop();
//       std::cout << item << std::endl;
//       buffer.push_back(item);
//     }
//   }

//   std::vector<int> output;
//   output = buffer;
//   return output;
// }
