# Wrapper

This provides a templated class for creating generic wrappers. The wrappers created this way automatically inherit all operations available to the wrapped (i.e. "inner") type.

## Example Usuage
```C++
#include <cassert>
#include <Wrapper.h>

using namespace std;
using Feet = utils::Wrapper<float, struct FTag>;
using Yards = utils::Wrapper<float, struct MTag>;
using Seconds = utils::Wrapper<float, struct STag>;
using YPS = utils::Wrapper<float, struct MPSTag>; // Yards per second

Yards to_yards(Feet f) {
    return Yards(f.to_inner() / 3);
}

YPS get_speed(Yards y, Seconds s) {
    return YPS(y.to_inner()/s.to_inner());
}

YPS get_speed(Feet f, Seconds s) {
    return get_speed(to_yards(f), s);
}

int main() {
    Feet f(15);
    Yards y(15);

    assert(get_speed(f, Seconds(5)) == get_speed(y, Seconds(15)));
}
```

## Details
* Wrappers must be constructed explicitly so `Feet f = 10` will cause the compiler to yell at you.
* `Wrapper<T, TAG>::inner` will return the type being wrapped
* At the moment, the inner type cannot be a reference.
