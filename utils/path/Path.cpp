#include <fstream>

#ifdef _UTILS_UNIX
	#include <sys/stat.h>
	#include <unistd.h>
#elif _UTILS_WINDOWS
	#include <direct.h>
#endif

#include "Path.h"
#include "common.h"

using namespace std;

namespace utils {
	Path::Path(const string& path) {
		m_path = replace_all(path, "\\", "/");
	}
	Path Path::cwd() {
		char buf[FILENAME_MAX];
		#ifdef _UTILS_UNIX
			getcwd(buf, FILENAME_MAX);
		#elif _UTILS_WINDOWS
			_getcwd(buf, FILENAME_MAX);
		#endif
		return Path(buf);
	}
	Path Path::resolve() const {
		if (is_empty()) return *this;

		Path ret;
		if (is_relative()) {
			ret = Path::cwd() + *this;
		} else {
			ret = *this;
		}

		auto parts = split(ret.m_path, '/');
		for (int i = parts.size()-1; i >= 0; i--) {
			auto& part = parts[i];
			if ((part.empty() || part == ".") && i != 0) {
				parts.erase(parts.begin() + i);
			} else if (part == ".." && i >= 1) {
				parts.erase(parts.begin() + i);
				parts.erase(parts.begin() + i - 1);
			}
		}
		ret.m_path = "";
		for (auto& part : parts) ret.m_path += part + "/";
		ret.m_path.resize(ret.m_path.size()-1);

		return ret;
	}
	Path Path::parent() const {
		Path ret = resolve();
		do {
			int pos = ret.m_path.rfind("/", ends_with(ret.m_path, "/") ? ret.m_path.size()-2 : string::npos);
			ret.m_path.resize(pos);
		} while (ret.m_path.back() == '/');
		return ret;
	}
	string Path::file_name(bool extension) const {
		int pos = m_path.rfind('/');
		string end = m_path.substr(pos+1);
		if (extension) return end;
		pos = end.rfind('.');
		return end.substr(0, pos);
	}
	bool Path::exists() const {
		#ifdef _UTILS_UNIX
			struct stat s;
			return stat(m_path.c_str(), &s) == 0;
		#elif _UTILS_WINDOWS
			return GetFileAttributes(m_path.c_str()) != INVALID_FILE_ATTRIBUTES;
		#endif
	}
	bool Path::is_file() const {
		// Technically wrong because of symlinks and whatnot
		return exists() && !is_directory();
	}
	bool Path::is_directory() const {
		#ifdef _UTILS_UNIX
			struct stat s;
			if (stat(m_path.c_str(), &s) == 0) {
				return s.st_mode & S_IFDIR;
			} else {
				return false;
			}
		#elif _UTILS_WINDOWS
			return GetFileAttributes(m_path.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
		#endif
	}
	bool Path::is_absolute() const {
		if (m_path.empty()) return false;
		int pos = m_path.find('/');
		string root = m_path.substr(0, pos);
		return root.empty() || (root.size() == 2 && root[1] == ':');
	}
	bool Path::operator==(const char* p) const {
		return trim(m_path, "/\\") == trim(p, "/\\") ? true : resolve().m_path == Path(p).resolve().m_path;
	}
	Path Path::operator+(const char* p) const {
		Path p2(p);
		return ends_with(m_path, "/") ? Path(m_path + p2.m_path) : 
										Path(m_path + "/" + p2.m_path);
	}
	Path::iterator Path::begin() const {
		iterator ret(this);
		ret.set_begin(SetBeginEndKey());
		return ret;
	}
	Path::iterator Path::end() const {
		iterator ret(this);
		ret.set_end(SetBeginEndKey());
		return ret;
	}

	ostream& operator<<(ostream& out, const Path& path) {
		return out<<path.to_string();
	}
}
