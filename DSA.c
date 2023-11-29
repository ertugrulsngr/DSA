#include <stdlib.h>
#include <stdio.h>
#include "DSA.h"

#define DSA_SIZE_COEFFICIENT 2
#define DSA_USED_SIZE(dsa) ((dsa->length) * (dsa->elementSize))
#define DSA_INDEX_TO_P(dsa, index) (((char*)(dsa->data)) + ((dsa->elementSize) * (index)))


void _dsa_memcpy(void *dest, const void *src, size_t size)
{
    char *d = dest;
    const char *s = src;
    while (size--)
    {
        *d++ = *s++;
    }
}

void _dsa_memcpy_reversed(void *dest, const void *src, size_t size)
{
    char *d = dest;
    const char *s = src;
    while (size--)
    {
        *d-- = *s--;
    }
}

DSA *dsa_create(size_t elementSize)
{
    if (elementSize == 0){
        perror("dsa_create: element size can not be zero");
        return NULL;
    }
    DSA *dsa = (DSA*)malloc(sizeof(DSA));
    if (dsa == NULL){
        perror("dsa_create:");
        return NULL;
    }
    dsa->elementSize = elementSize;
    dsa->allocatedSize = dsa->elementSize * DSA_SIZE_COEFFICIENT;
    dsa->length = 0;
    dsa->data = malloc(dsa->allocatedSize);
    if (dsa->data == NULL)
    {
        free(dsa);
        perror("dsa_create: ");
        return NULL;
    }
    return dsa;
}

int _dsa_add_size_handle(DSA *dsa, int addSize)
{
    if (dsa->allocatedSize >= DSA_USED_SIZE(dsa)+addSize)
    {
        return 1;
    }
    printf("add_size_handle_realloc\n");

    size_t newSize = DSA_SIZE_COEFFICIENT * (DSA_USED_SIZE(dsa) + addSize);
    void *temp = realloc(dsa->data, newSize);
    if (temp == NULL)
    {
        perror("_dsa_add_size_handle: ");
        return 0;
    }
    dsa->data = temp;
    dsa->allocatedSize = newSize;
    return 1;
}

int _dsa_remove_size_handle(DSA *dsa, int removeSize){
    if (dsa->allocatedSize <= DSA_SIZE_COEFFICIENT * (DSA_USED_SIZE(dsa) - removeSize)){
        return 1;
    }
    size_t newSize = DSA_SIZE_COEFFICIENT * (DSA_USED_SIZE(dsa) - removeSize);
    void *temp = realloc(dsa->data, newSize);
    if (temp == NULL){
        perror("_dsa_remove_size_handle: ");
        return 0;
    }
    dsa->data = temp;
    dsa->allocatedSize = newSize;
    return 1;
}

void _dsa_slide_left(DSA *dsa, size_t startIndex)
{
    // startIndex-1 will be overwrited
    char *next = DSA_INDEX_TO_P(dsa, startIndex);
    char *prev = next - dsa->elementSize;
    size_t size = (dsa->length - startIndex) * dsa->elementSize;
    _dsa_memcpy(prev, next, size);
}

void _dsa_slide_right(DSA *dsa, size_t startIndex)
{
    // startIndex will be empty
    char *next = DSA_INDEX_TO_P(dsa, dsa->length);
    char *prev = next-dsa->elementSize;
    size_t size = (dsa->length-startIndex) * dsa->elementSize;
    _dsa_memcpy_reversed(next, prev, size);
}

int dsa_add(DSA *dsa, const void *element)
{
    if (!_dsa_add_size_handle(dsa, dsa->elementSize))
    {
        printf("dsa_add: size could not handled.");
        return 0;
    }
    char *p = DSA_INDEX_TO_P(dsa, dsa->length);
    _dsa_memcpy((void*)p, element, dsa->elementSize);
    dsa->length++;
    return 1;
}

int dsa_remove(DSA *dsa, size_t index)
{
    if (index >= dsa->length)
    {
        printf("dsa_remove: index can not be greater than length.\n");
        return 0;
    }
    if (index != dsa->length-1)
    {
        _dsa_slide_left(dsa, index+1);
    }
    if(!_dsa_remove_size_handle(dsa, dsa->elementSize))
    {
        printf("dsa_remove: size could not handled.\n");
        return 0;
    }
    dsa->length--;
    return 1;
}

int dsa_insert(DSA *dsa, size_t index, const void *element)
{
    if (index >= dsa->length)
    {
        printf("dsa_remove: index can not be greater than length.");
        return 0;
    }
    if (index == dsa->length-1 || dsa->length == 0)
    {
        return dsa_add(dsa, element);
    }
    
    _dsa_slide_right(dsa, index);
    char *p = DSA_INDEX_TO_P(dsa, index);
    _dsa_memcpy(p, element, dsa->elementSize);
    dsa->length++;
    return 1;
}

int dsa_add_multiple(DSA *dsa, const void *arr, size_t arrSize)
{
    if (!_dsa_add_size_handle(dsa, arrSize*dsa->elementSize))
    {
        printf("dsa_add: size could not handled.");
        return 0;
    }
    char *p = DSA_INDEX_TO_P(dsa, dsa->length);
    _dsa_memcpy((void*)p, arr, arrSize*dsa->elementSize);
    dsa->length+=arrSize;
    return 1;
}

int dsa_remove_multiple(DSA *dsa, const size_t *indicesArr, size_t arrSize)
{

}


