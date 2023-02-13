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

#include <stdlib.h>
#include <string.h>

#define __DEBUG_OBJECTC 0
#define INSTANCE_STACK_SIZE 16

#ifndef __objectc_this
void* __objectc_this[INSTANCE_STACK_SIZE];
#endif



void* __objectc_pop(void** arr);
void __objectc_push(void** arr, void* elem);



#define _OBJ_STR(x) #x
#define _OBJ_STRINGIFY(x) _OBJ_STR(x)


#define $(s) (__objectc_push((void*)__objectc_this, s),s)
#define $instance(CLASSNAME) CLASSNAME* this = (CLASSNAME*)__objectc_pop((void*)__objectc_this); __objectc_pop((void*)__objectc_prevref)

#define $typedef(CLASSNAME) typedef struct CLASSNAME CLASSNAME
$typedef(ReferenceList);
#define $class(CLASSNAME) $typedef(CLASSNAME); struct CLASSNAME


#define $interface(CLASSNAME) $class(CLASSNAME)
$interface(ObjectC){
    char* name;
    void (*destruct)();
    size_t __parentRefIndex;
    ReferenceList* __refList;
    ReferenceList* __parentRefList;
};

$interface(BaseObject) {
    ObjectC* object;
};


 
#define $new(CLASSNAME) (this != 0 ? (__objectc_push((void*)__objectc_prevref, this->object->__refList)) : (0), CLASSNAME ## __const)
#define $$const(CLASSNAME) CLASSNAME ## __const

#define $ref(CLASSNAME) struct CLASSNAME *

#define $constructor(CLASSNAME, ...) CLASSNAME* CLASSNAME ## __const (__VA_ARGS__)

#define $$create_base(CLASSNAME) CLASSNAME* this = (CLASSNAME*)malloc(sizeof(CLASSNAME)); \
memset(this, 0, sizeof(CLASSNAME)); \
this->object = (ObjectC*)malloc(sizeof(ObjectC)); \


#define $create(CLASSNAME) $$create_base(CLASSNAME); \
this->object->name = _OBJ_STRINGIFY(CLASSNAME); \
this->object->destruct = CLASSNAME ## _destruct; \
ReferenceList* __prevref = __objectc_pop((void*)__objectc_prevref); \
if(__prevref != 0) $(__prevref)->add(this); \
this->object->__refList = $new(ReferenceList)(this);


#define $unmanaged BaseObject* this = 0;
#define $managed $$create_base(BaseObject); \
this->object->name = __FILE__ ":" _OBJ_STRINGIFY(__LINE__) ; \
this->object->__refList = $new(ReferenceList)(this);


#define $destructor(CLASSNAME) void CLASSNAME ## _destruct ()

#define $func(CLASSNAME, func) this->func = CLASSNAME ## _ ## func
#define $return return this
#define $delete(INSTANCE) $(INSTANCE)->object->destruct(); 
#define $free $(this->object->__refList)->free(); free(this); this = 0 


#define $default_destructor(CLASSNAME) $destructor(CLASSNAME) { \
    $instance(CLASSNAME); \
    $free; \
}


$class(ReferenceList) {
    BaseObject* parentInstance;
    BaseObject** instances;
    size_t size;

    void (*add)(void* instance);
    void (*free)();
    void (*freeOne)(size_t index);
    void (*destruct)();
};
$constructor(ReferenceList);
$destructor(ReferenceList);



$class(ManagedAlloc) {
    ObjectC* object;
    void* mem;
};

$destructor(ManagedAlloc);
$constructor(ManagedAlloc, size_t size);
#define $malloc(SIZE) $new(ManagedAlloc)(SIZE)
#define $realloc(WHAT,SIZE) realloc(WHAT,SIZE)

#ifndef __objectc_prevref
ReferenceList* __objectc_prevref[INSTANCE_STACK_SIZE];
#endif