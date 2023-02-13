/*
MIT License

Copyright (c) 2023 ad

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "objectc.h"

#if __DEBUG_OBJECTC == 1
#include <stdio.h>
#endif

// THIS IS NOT YOUR CONVENTIONAL CLASS IMPLEMENTATION!
// DO NOT USE AS EXAMPLE

#define $b(CAST) ((BaseObject*)(CAST))
#define $o(CAST) $b(CAST)->object

void ReferenceList_add(void* instance) {
    $instance(ReferenceList);

    size_t found = -1;
    if(this->instances != 0) {
        for(size_t i = 0; i < this->size; i++) {
            if(this->instances[i] == 0) found = i;
        }
        if(found == -1) {
            this->instances = realloc(this->instances, sizeof(BaseObject*) * (++this->size));
            found = this->size - 1;
        }
    }
    else {
        this->instances = malloc(sizeof(BaseObject*));
        this->size = 1;
        found = 0;
    }
    this->instances[found] = instance;


    $o(instance)->__parentRefList = this;
    $o(instance)->__parentRefIndex = found;
#if __DEBUG_OBJECTC == 1
    printf("ObjectC: Added reference {%s} for {%s} at index %d\n", $o(instance)->name, this->parentInstance->object->name, found);
#endif

}

void ReferenceList_free() {
    $instance(ReferenceList);
#if __DEBUG_OBJECTC == 1
    printf("ObjectC: Freeing {%s}'s %d items\n", this->parentInstance->object->name, this->size);
#endif
    for(size_t i = 0; i < this->size; i++) {
        
        if(this->instances[i] != 0) {
#if __DEBUG_OBJECTC == 1
            printf("    ObjectC: Freeing {%s} at [%p]\n", this->instances[i]->object->name, this->instances[i]);
#endif
            if(this->instances[i]->object->destruct) 
                $(this->instances[i])->object->destruct();
            this->instances[i] = 0;
        }
    }
    if(this->parentInstance->object->__parentRefList) $(this->parentInstance->object->__parentRefList)->freeOne($o(this->parentInstance)->__parentRefIndex);
    this->size = 0;
    free(this->instances);
    free(this);
}

void ReferenceList_freeOne(size_t index) {
    $instance(ReferenceList);
    

    if(index < this->size) {
        this->instances[index] = 0;
    }
}

$constructor(ReferenceList, void* parentInstance) {
    ReferenceList* this = (ReferenceList*)malloc(sizeof(ReferenceList)); 
    memset(this, 0, sizeof(ReferenceList));

    this->size = 0;
    this->instances = 0;
    this->parentInstance = parentInstance;
    $func(ReferenceList, free);
    $func(ReferenceList, add);
    $func(ReferenceList, freeOne);

    $return;
}
$destructor(ManagedAlloc) {
    $instance(ManagedAlloc);

    free(this->mem);

    $free;
}
$constructor(ManagedAlloc, size_t size){
    $create(ManagedAlloc);
    this->mem = malloc(size);
    $return;
}


void* __objectc_pop(void** arr) {

    int last = 0;
    for(int i = 0; i < INSTANCE_STACK_SIZE; i++) {
        if(arr[i] != 0) last = i;
    }
    void* lastptr = arr[last];
#ifdef __DEBUG_OBJECTC
    //printf("            Popping %d from %s stack\n", last, arr == __objectc_prevref ? "reference" : "instance");
#endif
    arr[last] = 0;
    return lastptr;
}
void __objectc_push(void** arr, void* elem) {

    int last = 0;
    for(int i = 0; i < INSTANCE_STACK_SIZE; i++) {
        if(arr[i] != 0) last = i;
    }
    if(last == INSTANCE_STACK_SIZE - 1) {
        for(int i = 0; i < INSTANCE_STACK_SIZE; i++) {
            arr[i] = 0;
        }
        arr[0] = elem;
    } else {
        arr[last+1] = elem;
    }
#ifdef __DEBUG_OBJECTC
   // printf("            Pushing %d to %s stack\n", last, arr == __objectc_prevref ? "reference" : "instance");
#endif
}
