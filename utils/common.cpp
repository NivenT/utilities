#include <fstream>

#include "common.h"

using namespace std;

namespace utils {
    bool starts_with(const string& str, const string& prefix) {
        return str.size() >= prefix.size() && str.find(prefix) == 0;
    }
    bool ends_with(const string& str, const string& suffix) {
        return str.size() >= suffix.size() && str.rfind(suffix) == str.size() - suffix.size();
    }
    string replace_all(const string& str, const string& o, const string& n) {
        string ret = str;

        int pos = 0;
        while ((pos = ret.find(o, pos)) != string::npos) {
            ret.replace(pos, o.size(), n);
            pos += n.size();
        }
        return ret;
    }
    string replace_all(const string& str, const vector<vector<string>>& os, 
                       const vector<string>& ns) {
        assert(os.size() == ns.size());

        string ret = str;
        // Could be done in one pass over the string but this is easier to write
        for (int i = 0; i < os.size(); ++i) {
            for (int j = 0; j < os[i].size(); ++j) {
                ret = replace_all(ret, os[i][j], ns[i]);
            }
        }
        return ret;
    }
    string trim(const string& str, const string& back_trash, const string& front_trash) {
        auto begin = str.find_first_not_of(front_trash);
        auto end = str.find_last_not_of(back_trash);

        if (begin == string::npos) return "";
        return str.substr(begin, end-begin+1);
    }
    string read_file(const string_view path) {
        string contents;

        // This call to .data() is kinda sketch
        ifstream file(path.data(), ios::binary);
        if (file.fail()) return "";
        file.seekg(0, ios::end);
        unsigned int file_size = file.tellg();
        file.seekg(0, ios::beg);
        file_size -= file.tellg();

        contents.resize(file_size);
        file.read((char*)&contents[0], file_size);
        file.close();

        return contents;
    }
    vector<string> split(const string& str, char delim) {    
        vector<string> items; 
        stringstream ss(str);  
        string item;   
        while (getline(ss, item, delim)) { 
            items.push_back(item);  
        }   
        return items;   
    }
}
