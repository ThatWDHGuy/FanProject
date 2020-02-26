#ifndef LimitedSizeList_H
#define LimitedSizeList_H

#include <stdlib.h>
template <class T>
class LimitedSizeList {
  private:
    unsigned int elementCount;
    unsigned int maxElementCount;
    T *cursor;
    T *data;

  public:
    class iterator {
      private:
        LimitedSizeList<T> &list;
        T *ptr;

      public:
        iterator(LimitedSizeList<T> &o): list(o) {
          ptr = list.cursor;

        }

        bool operator==(const iterator &x) {
          return (x.ptr == ptr);
        }

        bool operator!=(const iterator &x) {
          return (x.ptr != ptr);
        }

        iterator &operator++(int) {
          ptr++;
          if (ptr >= list.data + list.maxElementCount)
            ptr = list.data;
          return *this;
        }

        iterator &operator--(int) {
          ptr--;
          if (ptr < list.data)
            ptr = list.data + list.maxElementCount;
          return *this;
        }

        iterator &operator-=(int d) {
          ptr -= d;
          if (ptr < list.data)
            ptr += list.maxElementCount;
          return *this;
        }

        T& operator*() {
          return *ptr;
        }

        T *next() {
          return cursor;
        }
    };

    LimitedSizeList(unsigned int maxSize) : elementCount(0), maxElementCount(maxSize + 1) {
      data = (T*)malloc(maxElementCount * sizeof(T));
      cursor = data;
    };

    unsigned int size() {
      return elementCount;
    };

    unsigned int maxSize() {
      return maxElementCount;
    }

    void push(T &newValue) {
      *(cursor++) = newValue;
      if (cursor >= data + maxElementCount) {
        cursor = data;
      }
      if (elementCount < maxElementCount - 1)
        elementCount++;
    };

    LimitedSizeList<T>::iterator begin() {
      return LimitedSizeList<T>::iterator(*this) -= elementCount;
    };
    LimitedSizeList<T>::iterator end() {
      return LimitedSizeList<T>::iterator(*this);
    };
    T *last() {
      if  (elementCount == 0) {
        return NULL;
      } else {
        if (cursor == data) {
          return cursor + maxElementCount - 1;
        } else {
          return cursor - 1;
        }
      }
    };

};

#endif
