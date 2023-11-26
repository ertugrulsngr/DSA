#include <stdlib.h>
#include <stdio.h>

#define DSA_SIZE_COEFFICIENT 2

typedef struct DSA {
    void *data;
    size_t size;
    size_t allocatedSize;
    size_t elementSize;
}DSA;

void _dsa_memcpy(void *dest, const void *src, size_t len){
    char *d = dest;
    const char *s = src;
    while (len--){
        *d++ = *s++;
    }
}

DSA *dsa_create(size_t elementSize){
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
    dsa->data = (void *)malloc(dsa->elementSize * DSA_SIZE_COEFFICIENT);
    if (dsa->data == NULL){
        free(dsa);
        perror("dsa_create");
        return NULL;
    }
    dsa->size = 0;
    dsa->allocatedSize = dsa->elementSize * DSA_SIZE_COEFFICIENT;
    return dsa;
}
int _dsa_add_size_handle(DSA *dsa, int addSize){
    if (dsa->allocatedSize > dsa->size+addSize){
        return 1;
    }
    size_t new_size = DSA_SIZE_COEFFICIENT * (dsa->size+addSize);
    void *temp = realloc(dsa->data, new_size);
    if (temp == NULL){
        perror("_dsa_add_size_handle: ");
        return 0;
    }
    dsa->data = temp;
    dsa->allocatedSize = new_size;
    return 1;
}

int dsa_add(DSA *dsa, const void *element){
    if (!_dsa_add_size_handle(dsa, dsa->elementSize)){
        perror("dsa_add: size could not handled.");
        return 0;
    }
    char *p = dsa->data;
    p += dsa->size * dsa->elementSize;
    _dsa_memcpy((void*)p, element, dsa->elementSize);
    dsa->size++;
}

