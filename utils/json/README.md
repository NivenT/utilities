# Json

This provides a class for storing and manipulating JSON values. The aim of this class is to make working with Json as painless as possible.

## Example Usage
I recommend looking at the tests for example usage. For a quick glance, here are two ways (not the only two) to construct a Json value.
```C++
#include <Json.h>

int main() {
    utils::Json a = Json::parse(R"({
        "five": 5,
        "pi": 3.14159,
        "arr": [8, false, null],
        "arr2": [
            2, true,
            8,
                    "far"
        ],
        "obj": {
            "obj": {
                "huh": null
            },
            "heLLo": "woRld"
        },
        "my": "name",
        "true": false,
    })");
    utils::Json b = {
        {"five", 5},
        {"pi", 3.14159},
        {"arr", {8, false, Json::null()}},
        {"arr2", {2, true, 8, "far"}},
        {"obj", {
            {"obj", {
                {"huh", Json::null()}
            }},
            {"heLLo", "woRld"}
        }},
        {"my", "name"},
        {"true", false},
    };
    assert(a == b);
}
```

## Details
* Use `Json::dump()` to print out the value and `Json::dump(2)` to pretty print it.
