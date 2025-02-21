#include "logging.hpp"
#include "base.hpp"
#include "utils.hpp"
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

char *random_strc(int n){
    int urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd == -1) return nullptr;
    char *str = (char*)malloc(sizeof(char)*(n+1));
    if (str == nullptr) {
        close(urandom_fd);
        return nullptr;
    }
    for (int i=0;i<n;i++){
        char x = 0;
        read(urandom_fd, &x, 1);
        str[i] = 'a' + (x % ('z'-'a'+1));
    }
    str[n] = '\0';
    close(urandom_fd);
    return str;
}

bool starts_with(const char *s, const char *ss) {
    const char *str = strstr(s,ss);
    return str != nullptr && str == s;
}

bool fexist(const char *path) {
    struct stat st;
    return lstat(path, &st) == 0;
}
    
bool is_dir(const char *path) {
    struct stat st;
    return lstat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

bool is_lnk(const char *path) {
    struct stat st;
    return lstat(path, &st) == 0 && S_ISLNK(st.st_mode);
}

bool mkdir_ensure(const char *path, int mode) {
    mkdir(path, mode);
    return is_dir(path);
}

int mkdirs(const char *path, int mode) {
    std::vector<char> s(strlen(path) + 1);
    strcpy(s.data(), path);
    
    char *ss = s.data();
    while (ss[0] == '/')
        ss++;
    if (ss[0] == '\0')
        return 0;
    while ((ss = strchr(ss, '/')) != nullptr) {
        ss[0] = '\0';
        mkdir(s.data(), mode);
        ss[0] = '/';
        ss++;
    }
    int ret = mkdir(s.data(), mode);
    return ret;
}

char *dirname2(const char *path) {
    std::vector<char> s(strlen(path) + 1);
    std::vector<char> p(strlen(path) + 1);
    strcpy(s.data(), path);
    
    char *ss = s.data();
    while (ss[0] == '/')
        ss++;
    if (ss[0] == '\0')
        return nullptr;
    while ((ss = strchr(ss, '/')) != nullptr) {
        ss[0] = '\0';
        strcpy(p.data(), s.data());
        ss[0] = '/';
        ss++;
    }
    return strdup(p.data());
}

// Eksik fonksiyonları ekleyelim
const char *xgetenv(const char *name) {
    return getenv(name);
}

bool str_empty(const char *str) {
    return str == nullptr || str[0] == '\0';
}

std::vector<std::string> split_ro(const std::string& str, const char delimiter) {
    std::vector<std::string> result;
    std::string current = "";
    
    for (char c : str) {
        if (c == delimiter) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

int getenforce() {
    return -1;
}

int setenforce(bool mode) {
    return -1;
}

int getfilecon(const char *path, char **con) {
    *con = strdup("unlabeled");
    return 0;
}

int setfilecon(const char *path, const char *con) {
    return 0;
}

void freecon(char *con) {
    free(con);
}
