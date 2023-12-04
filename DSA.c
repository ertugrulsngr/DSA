#include <stdlib.h>
#include <stdio.h>
#include "DSA.h"

#define DSA_SIZE_COEFFICIENT 2
#define DSA_USED_SIZE(dsa) ((dsa->length) * (dsa->elementSize))
#define DSA_INDEX_TO_P(dsa, index) (((char*)(dsa->data)) + ((dsa->elementSize) * (index))) 

#define DSA_ERR_INVALID_INDEX "Invalid index."


int dsa_size_t_compare(const void *a, const void *b)
{
    const size_t _a = *(const size_t *)a;
    const size_t _b = *(const size_t *)b;
    if (_a < _b)
    {
        return -1;
    }
    if (_a > _b)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void dsa_memcpy(void *dest, const void *src, size_t size)
{
    char *d = dest;
    const char *s = src;
    while (size--)
    {
        *d++ = *s++;
    }
}

void dsa_memcpy_reverse(void *destEnd, const void *srcEnd, size_t size)
{
    char *d = destEnd;
    const char *s = srcEnd;
    while (size--)
    {
        *d-- = *s--;
    }
}

int dsa_add_size_handle(DSA *dsa, size_t addSize)
{
    if (dsa->allocatedSize >= DSA_USED_SIZE(dsa)+addSize)
    {
        return 1;
    }
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

int dsa_remove_size_handle(DSA *dsa, size_t removeSize){
    if (dsa->allocatedSize <= DSA_SIZE_COEFFICIENT * (DSA_USED_SIZE(dsa) - removeSize)){
        return 1;
    }
    size_t newSize = DSA_SIZE_COEFFICIENT * (DSA_USED_SIZE(dsa) - removeSize);
    if (newSize == 0){
        newSize = DSA_SIZE_COEFFICIENT * dsa->elementSize;
    }
    void *temp = realloc(dsa->data, newSize);
    if (temp == NULL){
        perror("_dsa_remove_size_handle: ");
        return 0;
    }
    dsa->data = temp;
    dsa->allocatedSize = newSize;
    return 1;
}

void dsa_shift_block_right(DSA *dsa, size_t blockStartIndex, size_t blockEndIndex, size_t shiftCount)
{
    char *srcEnd = DSA_INDEX_TO_P(dsa, blockEndIndex) + (dsa->elementSize - 1);
    char *destEnd = srcEnd + (dsa->elementSize * shiftCount);
    size_t shiftedElementCount = blockEndIndex - blockStartIndex + 1;
    dsa_memcpy_reverse(destEnd, srcEnd, shiftedElementCount * dsa->elementSize);
}

void dsa_shift_block_left(DSA *dsa, size_t blockStartIndex, size_t blockEndIndex, size_t shiftCount)
{
    char *src = DSA_INDEX_TO_P(dsa, blockStartIndex);
    char *dest = src - (dsa->elementSize * shiftCount);
    size_t shiftedElementCount = blockEndIndex - blockStartIndex + 1;
    dsa_memcpy(dest, src, shiftedElementCount * dsa->elementSize);
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

int dsa_add(DSA *dsa, const void *element)
{
    if (!dsa_add_size_handle(dsa, dsa->elementSize))
    {
        printf("dsa_add: size could not handled.");
        return 0;
    }
    char *p = DSA_INDEX_TO_P(dsa, dsa->length);
    dsa_memcpy(p, element, dsa->elementSize);
    dsa->length++;
    return 1;
}

int dsa_remove(DSA *dsa, size_t index)
{
    if (index >= dsa->length)
    {
        printf(DSA_ERR_INVALID_INDEX);
        return 0;
    }

    if (index != dsa->length-1)
    {
        dsa_shift_block_left(dsa, index+1, dsa->length-1, 1);
    }
    // TODO: Here dsa shifted left. But if size not handled, length couldn't decreased.
    if(!dsa_remove_size_handle(dsa, dsa->elementSize))
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
        printf(DSA_ERR_INVALID_INDEX);
        return 0;
    }
    if (index == dsa->length-1 || dsa->length == 0)
    {
        return dsa_add(dsa, element);
    }
    dsa_shift_block_right(dsa, index, dsa->length-1, 1);
    char *p = DSA_INDEX_TO_P(dsa, index);
    dsa_memcpy(p, element, dsa->elementSize);
    dsa->length++;
    return 1;
}

int dsa_clear(DSA *dsa)
{
    if (!dsa_remove_size_handle(dsa, DSA_USED_SIZE(dsa))){
        printf("dsa_remove: size could not handled.\n");
        return 0;
    }
    dsa->length = 0;
    return 1;
}

int dsa_add_multiple(DSA *dsa, const void *arr, size_t arrLength)
{
    if (!dsa_add_size_handle(dsa, arrLength*dsa->elementSize))
    {
        printf("dsa_add: size could not handled.");
        return 0;
    }
    char *p = DSA_INDEX_TO_P(dsa, dsa->length);
    dsa_memcpy(p, arr, arrLength*dsa->elementSize);
    dsa->length+=arrLength;
    return 1;
}

void dsa_free(DSA *dsa)
{
    if (dsa)
    {
        free(dsa->data);
        free(dsa);
    }
    
}