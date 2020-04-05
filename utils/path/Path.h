#ifndef UTILS_PATH_H_INCLUDED
#define UTILS_PATH_H_INCLUDED

#include <cassert>
#include <ostream>
#ifdef _UTILS_UNIX
	#include <dirent.h>
#elif _UTILS_WINDOWS
	#include <windows.h>
#endif

namespace utils {
	class Path;
	extern std::ostream& operator<<(std::ostream& out, const Path& path);
	/// A (case-sensitive) Path in a file system
	class Path {
	public:
		/// Key unlocking the set_{begin,end}() "private" functions of class Path::iterator
	    class SetBeginEndKey {
	        friend class Path;
	        SetBeginEndKey() {}
	        SetBeginEndKey(const SetBeginEndKey&);
	        SetBeginEndKey& operator=(const SetBeginEndKey&);
	    };
	    /// Custom iterator for looping over files in a path
	    ///
	    /// For Directories: loops over the files (including . and ..)
	    ///
	    /// For Files: one iteration returning the name of the file itself
		class iterator {
		public:
			using value_type = const Path;
			using difference_type = std::ptrdiff_t;
			using pointer = const Path*;
			using reference = const Path;
			using iterator_category = std::forward_iterator_tag;
			#ifdef _UTILS_UNIX
				using node = struct{ DIR* d; struct dirent* e; };
			#elif _UTILS_WINDOWS
				using node = struct{ HANDLE h; WIN32_FIND_DATA d; };
			#endif
		private:
			pointer m_path;
			node m_node;
		public:
			iterator() {}
			iterator(pointer path) : m_path(path) {
				assert(path != nullptr);
			}
			reference operator*() const {
				if (m_path->is_file()) return *m_path;
				#ifdef _UTILS_UNIX
					return Path(m_node.e->d_name);
				#elif _UTILS_WINDOWS
					return Path(m_node.d.cFileName);
				#endif
			}
			reference operator->() const {
				return operator*();
			}
			iterator& operator++() {
				if (m_path->is_file()) {
					m_path = nullptr;
				} else {
					#ifdef _UTILS_UNIX
						m_node.e = readdir(m_node.d);
						if (!m_node.e) closedir(m_node.d);
					#elif _UTILS_WINDOWS
						if (FindNextFile(m_node.h, &m_node.d) == 0) {
							FindClose(m_node.h);
							m_path = nullptr;
						}
					#endif
				}
				return *this;
			}
			iterator operator++(int) {
				auto ret = *this;
				operator++();
				return ret;
			}
			bool operator==(const iterator& rhs) const {
				if (m_path != rhs.m_path) return false;
				if (!m_path || m_path->is_file()) return true;
				#ifdef _UTILS_UNIX
					return m_node.e == rhs.m_node.e;
				#elif _UTILS_WINDOWS
					return strcmp(m_node.d.cFileName, rhs.m_node.d.cFileName) == 0;
				#endif
			}
			bool operator!=(const iterator& rhs) const {
				return !operator==(rhs);
			}

			void set_begin(SetBeginEndKey _) {
				if (!m_path->exists()) {
					m_path = nullptr;
				} else if (m_path->is_directory()) {
					#ifdef _UTILS_UNIX
						m_node.d = opendir(m_path->m_path.c_str());
						m_node.e = readdir(m_node.d);
					#elif _UTILS_WINDOWS
						std::string pat = m_path->m_path + "/*";
						m_node.h = FindFirstFile(pat.c_str(), &m_node.d);
					#endif
				}
			}
			void set_end(SetBeginEndKey _) {
				if (m_path->is_file() || !m_path->exists()) {
					m_path = nullptr;
				} else {
					#ifdef _UTILS_UNIX
						m_node.e = nullptr;
					#elif _UTILS_WINDOWS
						m_path = nullptr;
					#endif
				}
			}
		};
	private:
		std::string m_path;
	public:
		Path() {}
		Path(const std::string& path);
		Path(const char* p) : Path(std::string(p)) {}

		/// Returns Path representing the current working directory
		static Path cwd();

		/// Returns an absolute path equivalent to this
		Path resolve() const;
		/// Returns the folder containing this path
		Path parent() const;
		/// Returns the last component (file or directory) name of the path
		///
		/// When extension=false, the extension is dropped from the returned string
		std::string file_name(bool extension = true) const;

		bool exists() const;
		bool empty() const { return is_empty(); }
		bool is_empty() const { return m_path.empty(); }
		bool is_file() const;
		bool is_directory() const;
		bool is_folder() const { return is_directory(); }
		bool is_relative() const { return !is_absolute(); }
		bool is_absolute() const;

		const std::string& to_string() const { return m_path; }

		Path& operator=(const char* p) { m_path = p; return *this; }
		Path& operator=(const std::string& p) { return operator=(p.c_str()); }
		bool operator==(const char* p) const;
		bool operator==(const Path& p) const { return *this == p.m_path; }
		bool operator==(const std::string& p) const { return operator==(p.c_str()); }
		bool operator!=(const char* p) const { return !operator==(p); }
		bool operator!=(const Path& p) const { return !operator==(p); }
		bool operator!=(const std::string& p) const { return !operator==(p); }

		bool operator<(const Path& p) const { return resolve().to_string() < p.resolve().to_string(); }
		bool operator<=(const Path& p) const { return operator==(p) || operator<(p); }

		Path operator+(const char* p) const;
		Path& operator+=(const char* p) { return *this = *this + p; }
		Path operator+(const std::string& p) const { return operator+(p.c_str()); }
		Path& operator+=(const std::string& p) { return operator+=(p.c_str()); }
		Path operator+(const Path& p) const { return operator+(p.m_path); }
		Path& operator+=(const Path& p) { return operator+=(p.m_path); }

		Path::iterator begin() const;
		Path::iterator end() const;
	};
}

namespace std {
    template<>
    struct hash<utils::Path> {
        std::size_t operator()(const utils::Path& path) const {
            return std::hash<std::string>()(path.resolve().to_string());
        }
    };
}

#endif // UTILS_PATH_H_INCLUDED