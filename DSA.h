#ifndef DYNAMICALLY_SCALED_ARRAY_H_
#define DYNAMICALLY_SCALED_ARRAY_H_

#include <stdlib.h>

typedef struct DSA {
    void *data;
    size_t length;
    size_t allocatedSize;
    size_t elementSize;
}DSA;

DSA *dsa_create(size_t elementSize);
int dsa_add(DSA *dsa, const void *element);
int dsa_remove(DSA *dsa, size_t index);
int dsa_insert(DSA *dsa, size_t index, const void *element);
int dsa_add_multiple(DSA *dsa, const void *arr, size_t arrSize);
#endif