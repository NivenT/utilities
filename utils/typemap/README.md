# TypeMap

This provides a map from types to values of the type. In particular, this map let's you store one value of each type (including custom types and references). 

## Example Usage
```C++
#include <TypeMap.h>

int main() {
    int b = 8;

    TypeMap map;
    map.insert<int>(5);
    map.insert<char>('a');
    map.insert<int&>(b);

    assert(map.find<int>() == 5);
    assert(map.find<int&>() == 8);
    assert(map.find<char>() == 'a');

    map.get<int>()++;
    assert(map.get<int>() == 6);

    // Inserts a float into the map
    map.get<float>() = 3.5;
    assert(map.find<float>() == 3.5);

    // Crashes at runtime because no double has been asserted
    map.find<double>() = 4;
}
```

## Details
* Internally data is stored in a `std::unordered_map<utils::TypeInfo, void*>`.
* The TypeMap calls all necessary destructors when elements are removed just like a `std::vector` or `std::map` would even though it stores `void*`.
* You can access the raw `void*` pointers if you really want to via `TypeMap::find(TypeInfo)`. If you do, you should know that the semantics of the pointer depends on the type it "points" to. Suppose we wrote `void* val = TypeMap::find(TypeInfo::get<T>);`. Then,
    * For references (i.e. `T=U&` for some type `U`), `val` is a pointer to the value being referenced; the "true" type of `val` is `T*`.
    * For small types (i.e. `sizeof(T) <= sizeof(void*)` so this includes all pointers), `val` is literally the value being stored; the "true" type of `val` is `T`
    * For all other types, `val` is a pointer to the value which is stored on the heap; the "true" type of `val` is `T` and `val` points to a dynamically allocated heap address that the `TypeMap` is responsible for freeing.
* `TypeMap::find` crashes when you try to find a type that hasn't been inserted yet while `TypeMap::get` just inserts a new value on the fly.
