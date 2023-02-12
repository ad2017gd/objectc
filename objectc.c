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

    printf("ObjectC: Added reference {%s} for {%s} at index %d\n", $o(instance)->name, this->parentInstance->object->name, found);

}

void ReferenceList_free() {
    $instance(ReferenceList);
    printf("ObjectC: Freeing {%s}'s %d items\n", this->parentInstance->object->name, this->size);
    for(size_t i = 0; i < this->size; i++) {
        
        if(this->instances[i] != 0) {
            printf("    ObjectC: Freeing {%s} at [%p]\n", this->instances[i]->object->name, this->instances[i]);
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
