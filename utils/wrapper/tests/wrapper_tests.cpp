#include <iostream>
#include <unordered_set>

#include <Wrapper.h>
#include <common.h>

using namespace std;
using namespace utils;

// StrongWrapper has an explicit constructor
using Inches = StrongWrapper<int, struct InchesTag>;
// Wrappers do not have explicit constructors
using Feet = Wrapper<int, struct FeetTag>;
using Name = StrongWrapper<string, struct NameTag>;

int main(int argc, char* argv[]) {
    cout<<"Running Wrapper tests..."<<endl;

    Inches a(12);
    Feet b;
    Name c("Steve");

    b = 1;

    // You can cout<< a wrapper
    assert(utils::to_string(a) == "12");
    assert(utils::to_string(b) == "1");
    assert(utils::to_string(c) == "Steve");

    // Confirms that you can't check for equality between Inches and Feet even though they wrap the same type
    if ((int)check::EqualsExists<Inches, Feet>::value == 1) {
        assert(false);
    }

    assert(a == 12);
    assert(a + 5 == 17);
    assert((b << 3) == 8);
    assert(a++ == 12);
    assert(a == 13);
    assert(++b == 2);
    assert(a * 9 == 117);
    assert(b == 2);
    assert(b / 2 == Feet(1));
    assert((a & 8) == 8);
    assert((b | 4) == 6);
    assert(~Inches(0) == Inches(~0));
    assert(--a == 12);
    assert((a >> 1) == 6);
    assert(a % 3 == 0);
    assert(b % 3 == 2);

    a *= 5;
    assert(a == 60);
    b /= 2;
    assert(b == 1);
    a -= 15;
    assert(a == 45);
    b += 8;
    assert(b == 9);
    assert(a != 10);

    assert(a > 3);
    assert(b < 10);
    assert(a <= 45);
    assert(b >= 8);
    
    assert(c + string(" Rogers") == Name("Steve Rogers"));
    assert(c < Name("Tony"));

    c += string("n Strange");
    assert(utils::to_string(c) == "Steven Strange");
    assert(c == c.to_inner());

    // Issue with commas in assert's, so this check is written in a roundabout way
    if (!is_same<Name::inner, string>::value) {
        assert(false);
    }

    // Wrappers can be hashed as well
    unordered_set<Inches> set;
    set.insert(Inches(3));
    set.insert(Inches(5));

    assert(set.size() == 2);
    assert(*set.find(Inches(3)) == Inches(3));
    
    cout<<"Tests passed"<<endl;
    return 0;
}

