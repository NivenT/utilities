# Path

This provides a class with some helpful function for dealing with file paths. I'm not gonna provide example usage because I think the tests and the documentation probably say enough here.

## Details
* You can iterate over the files in a folder using something like `for (const Path& p : Path("path/to/a/folder")) {/* stuff */}`
* This class has no concept of symlinks or shortcuts
