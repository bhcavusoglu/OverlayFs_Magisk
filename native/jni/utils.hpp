#ifndef UTILS_HPP
#define UTILS_HPP

#include "base.hpp"
#include <vector>
#include <string>

char *random_strc(int n);
bool starts_with(const char *s, const char *ss);
bool is_dir(const char *path);
bool is_lnk(const char *path);
bool fexist(const char *path);
bool mkdir_ensure(const char *path, int mode);
int mkdirs(const char *path, int mode);
char *dirname2(const char *path);
int getmod(const char *file);
int getuidof(const char *file);
int getgidof(const char *file);
int dump_file(const char *src, const char *dest);
int verbose_mount(const char *a, const char *b, const char *c, int d, const char *e);
int verbose_umount(const char *a, int b);
const char *xgetenv(const char *name);
bool str_empty(const char *str);
std::vector<std::string> split_ro(const std::string& str, const char delimiter);
int getenforce();
int setenforce(bool mode);
int getfilecon(const char *path, char **con);
int setfilecon(const char *path, const char *con);
void freecon(char *con);

// Template for reversed iteration
template <typename T>
class reversed_container {
public:
    reversed_container(T &base) : base(base) {}
    auto begin() { return base.rbegin(); }
    auto begin() const { return base.crbegin(); }
    auto cbegin() const { return base.crbegin(); }
    auto end() { return base.rend(); }
    auto end() const { return base.crend(); }
    auto cend() const { return base.crend(); }
private:
    T &base;
};

template <typename T>
reversed_container<T> reversed(T &base) {
    return reversed_container<T>(base);
}

#endif // UTILS_HPP
