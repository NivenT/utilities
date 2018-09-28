#include <iostream>

#include <Path.h>

using namespace std;
using namespace utils;

int main(int argc, char* argv[]) {
    cout<<"Running Path tests..."<<endl;

    Path p("path/to/a/file.txt");
    assert(p.to_string() == "path/to/a/file.txt");

    Path p2 = std::move(p);
    assert(p.to_string() == "");
    assert(p.is_empty());
    assert(p2 == "path/to/a/file.txt");

    Path q("folder/file.txt");
    assert(q.exists());
    assert(q.is_file());
    assert(!q.is_directory());
    assert(q.parent() == "folder");
    assert(q.parent() == "folder/");
    assert(q.parent() == "folder/./");
    assert(q.parent().is_directory());	

    assert(Path("folder") + "file.txt" == q);
    assert(q.is_relative());

    Path r = "/home";
    assert(r.is_absolute());
    assert(Path("C:\\").is_absolute());

    Path cwd = Path::cwd();
    assert(cwd.is_directory() && cwd.is_absolute());
    assert(cwd + "folder" == Path("folder/..///folder/./"));
    assert(Path("folder") == cwd + "folder");
    assert(Path("folder").to_string() != (cwd + "folder").to_string());
    assert(Path("folder").resolve().to_string() == (cwd + "folder").to_string());

    assert(Path("Desktop\\Stuff\\And\\Things") == Path("Desktop/Stuff/And/Things"));

    Path s("fake");
    assert(!s.exists());
    for (const Path& p : s) {
    	assert(false);
    }

    for (const auto& p : q) {
    	assert(p == q);
    }

    Path t = "folder";
    auto it = t.begin();
    assert(*(it++) == ".");
    assert(*(it++) == "..");
    assert(*(it++) == "file");
    assert(*(it++) == "file.txt");
    assert(it == t.end());

    assert(Path("folder/fake/").parent() == "folder");
    assert(Path("folder").parent() == ".");

    assert(Path("folder/fake.txt").file_name() == "fake.txt");
    assert(Path("folder/fake.txt").file_name(false) == "fake");
    assert(Path("folder/fake").file_name() == "fake");
    assert(Path("/").file_name() == "");

    assert(Path(".") == cwd);
    assert(Path("..") == cwd.parent());

    assert(Path(".").is_relative());
    assert(Path(".").resolve().is_absolute());

    cout<<"Tests passed"<<endl;
    return 0;
}
