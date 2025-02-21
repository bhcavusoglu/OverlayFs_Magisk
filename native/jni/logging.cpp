#include <stdio.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <time.h>
#include <string>
#include <unistd.h>
#include <vector>

void log_to_file(int fd, int prio, const char *log) {
    if (fd < 0) {
        printf("%s", log);
        return;
    }
    char prio_c = 'I';
    switch (prio) {
        case 1:
            prio_c = 'D';
            break;
        case 2:
            prio_c = 'W';
            break;
        case 3:
            prio_c = 'E';
            break;
        case 4:
            prio_c = 'V';
            break;
        case 5:
            prio_c = 'F';
            break;
        default:
            prio_c = 'I';
            break;
    }
    std::vector<char> buf(4098);
    timeval tv;
    tm tm;
    gettimeofday(&tv, nullptr);
    localtime_r(&tv.tv_sec, &tm);
    long ms = tv.tv_usec / 1000;
    size_t off = strftime(buf.data(), buf.size(), "%m-%d %T", &tm);
    snprintf(buf.data() + off, buf.size() - off,
        ".%03ld %5d %5d %c : %s", ms, getpid(), gettid(), prio_c, log);
    write(fd, buf.data(), strlen(buf.data()));
}
