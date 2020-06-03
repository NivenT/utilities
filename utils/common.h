#ifndef UTILS_COMMON_H_INCLUDED
#define UTILS_COMMON_H_INCLUDED

#include <sstream>
#include <vector>
#include <cassert>

namespace utils {
    /// Checks if str starts with prefix
    extern bool starts_with(const std::string& str, const std::string& prefix);
    /// Checks if str ends with suffix
    extern bool ends_with(const std::string& str, const std::string& suffix);
    /// Replaces all occurrences of o in str with n
    extern std::string replace_all(const std::string& str, const std::string& o, 
                                   const std::string& n);
    /// Replaces all occurrences of any string in the ith element of os with the ith element of ns
    extern std::string replace_all(const std::string& str, 
                                   const std::vector<std::vector<std::string>>& os, 
                                   const std::vector<std::string>& ns);
    /// Removes leading and trailing whitespace
    extern std::string trim(const std::string& str, 
                            const std::string& back_trash = " \t\n\v\f\r\0", 
                            const std::string& front_trash = " \t\n\v\f\r\0");
    /// Splits a string into substrings separated by delim
    extern std::vector<std::string> split(const std::string& str, char delim);
    /// Reads the contents of a file into a string
    extern std::string read_file(const std::string_view path);
    /// Stolen from Boost
    extern std::size_t hash_combine(std::size_t lhs, std::size_t rhs);

    /// converts input to a std::string
    template<class T>
    std::string to_string(const T& input, std::size_t precision = 0) {
        std::ostringstream os;
        if (precision != 0) {
            os.precision(precision);
        }
        os<<input;
        return os.str();
    }
}

#endif // UTILS_COMMON_H_INCLUDED