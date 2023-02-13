<div align="center">
<h1> objectc - Macro and runtime library for Object Oriented Programming in C</h1>
<img height=200px src="https://user-images.githubusercontent.com/39013925/218322848-fd48ca27-692d-4ba6-91cc-368ec484fbd1.png">

</div>

# Dependencies
This library assumes you have a `malloc` and a `printf` (for __DEBUG_OBJECTC=1 only) implementation.

# Case style
All macros implemented by this library are prefixed with `$` and are snake_case.

Function implementations for classes take the format `ClassName_functionName`

# Classes
Classes/interfaces are all structs behind the scenes and are heap-allocated.

## Instantiating a class
```c
// Heap-allocated, so it's a pointer!
RGBA* rgb = $new(RGBA)(0,255,0,0.1);
```

## Defining a class
Class definitions look as follows:
```c
$class(ClassName) {
  ObjectC* object; // <---- Must be first in the struct!
  
  int a;
  void (*imAFunction)();
}; // <---- Semicolon required!
```

## Function implementations
To implement a function `add` for the class `Calculator`:
```c
double Calculator_add(double val/*, ...*/) {
  $instance(Calculator); // Now you can access class instance "this"
  this->total += val;
  // ...
}
```

## Using functions
Your average OOP implementation in C uses an argument to pass the instance to the function. This library uses a macro
that assigns the instance to a global variable and is later retrieved in the function.

**This makes this library thread unsafe!**

```c
// $(...) is very important when calling functions!
// If your program is faulting, double check if you have missed this.
$(calculator)->add(100.0);
```

## Constructors
Constructors are required to create a class instance.
```c
$constructor(Calculator, double initialValue /*, ...*/) {
  $create(Calculator); // Allocates a new class and sets up reference counting
  $func(Calculator,add); // <-- VERY IMPORTANT! Must manually assign function implementations this way.
  this->initialValue = initialValue;
  $return; // <=> return this
}
```

## Destructors
Destructors are also required to free the class instances created.
If not manually allocating memory, just use `$default_destructor(Calculator)`.
```c
$destructor(String) {
  $instance(String);
  
  free(this->cstring);
  
  $free; // <-- Frees memory and instances created in the scope of the same class (functions, constructor)
}
```

## Freeing an instance
Freeing an instance calls the destructor and frees all references inside recursively.
```c
$delete(calculator);
```


## Builtin classes
Currently, objectc implements the `BaseObject` and `ManagedAlloc` classes.
### BaseObject
BaseObject is similar to the "object" in JavaScript. It is the most basic class and by definition every other class extends it
(that's why you have to start every class definition with `ObjectC* object`);
```c
$interface(BaseObject) {
  ObjectC* object;
}
```
### ManagedAlloc
ManagedAlloc allows you to use malloc safely by automatically freeing the allocated memory using the reference counter.

Use ManagedAlloc with the `$malloc(size)` and `$realloc(managedAllocInstance, newSize)` macros. Allocated memory pointer is available in the "mem" member of this class.
```c
int main() { $managed
    char* str = $malloc(3);
    strcpy(str->mem, "hi");

    $free; // <--- Now "str" is freed.
    return 0;
}
```

# Managed and unmanaged functions
All non-class (or static) functions **MUST** be prepended with `$managed` or `$unmanaged` to access ObjectC functionality.

Managed functions keep track of created instances and can be freed at the end.

```c
void managedFunc() { $managed
  Obj* object = $new(Obj)();
  $free; // free all references in this function
}
Obj* global;
void unmanagedFunc() { $unmanaged
  global = $new(Obj)();
}
```


# Example

<details>

<summary>Expand code</summary>

```c
#include "objectc.h"

$class(String) {
    ObjectC* object;
    char* string;
};
$class(Test) {
    ObjectC* object;
    void (*print)(String* str);
};

$destructor(String) {
    $instance(String);

    free(this->string);

    $free;
}

$constructor(String, char* str) {
    $create(String);

    if(str) {
        this->string = malloc(strlen(str)*sizeof(char)+1);
        strcpy(this->string, str);
    } else {
        this->string = malloc(1);
        this->string[1] = 0;
    }

    $return;
}

void Test_print(String* str) {
    $instance(Test);
    String* cat = $new(String)("\n\nTest concat string: \n\n");
    printf("%s%s", str->string, cat->string);
}

$class(TestSub) {
    ObjectC* object;
};

$default_destructor(TestSub);
$constructor(TestSub) {
    $create(TestSub);
    $return;
};

$default_destructor(Test);
$constructor(Test) {
    $create(Test);

    Test* obj = $new(TestSub)();
    $func(Test,print);

    $return;
};

int main() { $managed

    Test* obj = $new(Test)();
    Test* obj2 = $new(Test)();
    String* str = $new(String)("String str test");

    $(obj)->print(str); // creates new String "\n\nTest concat string: \n\n" and concats with {str}

    $free;
    return 0;
}
```

</details>

# License
This program is licensed under the MIT license.
