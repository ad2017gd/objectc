#include <stdlib.h>
#include <string.h>

#define __DEBUG_OBJECTC 1

#ifndef __objectc_this
void* __objectc_this;
#endif

#define _OBJ_STR(x) #x
#define _OBJ_STRINGIFY(x) _OBJ_STR(x)


#define $(s) ((__objectc_this = s),s)
#define $instance(CLASSNAME) CLASSNAME* this = (CLASSNAME*)__objectc_this

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

 
#define $new(CLASSNAME) (this != 0 ? (__objectc_prevref = $(this)->object->__refList) : (__objectc_prevref = 0), CLASSNAME ## __const)
#define $$const(CLASSNAME) CLASSNAME ## __const

#define $ref(CLASSNAME) struct CLASSNAME *

#define $constructor(CLASSNAME, ...) CLASSNAME* CLASSNAME ## __const (__VA_ARGS__)

#define $$createBase(CLASSNAME) CLASSNAME* this = (CLASSNAME*)malloc(sizeof(CLASSNAME)); \
memset(this, 0, sizeof(CLASSNAME)); \
this->object = (ObjectC*)malloc(sizeof(ObjectC)); \


#define $create(CLASSNAME) $$createBase(CLASSNAME); \
this->object->name = # CLASSNAME; \
this->object->destruct = CLASSNAME ## _destruct; \
if(__objectc_prevref != 0) $(__objectc_prevref)->add(this); \
this->object->__refList = $new(ReferenceList)(this);


#define $unmanaged BaseObject* this = 0;
#define $managed $$createBase(BaseObject); \
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



// RUNTIME REFERENCE COUNTER

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


#ifndef __objectc_prevref
ReferenceList* __objectc_prevref;
#endif
