#include "base.hpp"
#include "logging.hpp"
#include "mountinfo.hpp"
#include "utils.hpp"
#include "partition.hpp"
#include <vector>
#include <string>
#include <set>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>

using namespace std;

#define xmount(a,b,c,d,e) verbose_mount(a,b,c,d,e)
#define umount2(a,b) verbose_umount(a,b)

#define CLEANUP \
    LOGI("clean up\n"); \
    umount2(overlay_tmpdir.data(), MNT_DETACH); \
    rmdir(overlay_tmpdir.data());

#define TO_STR std::string("")

#define MNT_FLAGS (MS_LAZYTIME | MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_DIRSYNC | MS_NOATIME | MS_NODIRATIME | MS_RELATIME | MS_STRICTATIME | MS_NOSYMFOLLOW | MS_MANDLOCK | MS_SILENT)

int log_fd = -1;
std::string overlay_tmpdir = "";

const char *overlay_name = "overlay";

static std::vector<mount_info> collect_mounts() {
    std::vector<mount_info> mountinfo;
    auto current_mount_info = parse_mount_info("self");
    std::reverse(current_mount_info.begin(), current_mount_info.end());
    for (auto &info : current_mount_info) {
        for (auto &part : SYSTEM_PARTITIONS)
        if (starts_with(info.target.data(), string(part + "/").data()) || info.target == part) {
            for (auto &s : reversed(mountinfo)) {
                if (s.target == info.target || starts_with(info.target.data(), string(s.target + "/").data())) {
                    LOGD("mountinfo: mountpoint %s is hidden under %s\n", info.target.data(), s.target.data());
                    goto next_mountpoint;
                }
            }
            LOGD("mountinfo: device (%u:%u)%s on %s type %s (%s)\n", major(info.device), minor(info.device), 
                (info.root != "/")? info.root.data() : "", info.target.data(), info.type.data(), info.fs_option.data());
            mountinfo.emplace_back(info);
            break;
        }
        next_mountpoint:
        continue;
    }
    return mountinfo;
}

static int do_remount(int flags = 0, int exclude_flags = 0) {
    std::vector<mount_info> mountinfo = collect_mounts();
    struct statvfs stvfs{};
    exclude_flags |= MS_BIND;
    for (auto &mnt : mountinfo) {
        auto info = mnt.target;
        statvfs(info.data(), &stvfs);
        if (mnt.type == "overlay" || mnt.type == "tmpfs") {
            int fd = open(info.data(), O_PATH);
            string fd_path = "/proc/self/fd/";
            fd_path += std::to_string(fd);
            LOGD("%s [%s] (%s)\n", (mount(nullptr, fd_path.data(), nullptr, MS_REMOUNT | (stvfs.f_flag & MNT_FLAGS & ~exclude_flags) | flags, nullptr) == 0)?
                 "remounted success" : "remount failed", info.data(), mnt.type.data());
            close(fd);
        } else {
            LOGD("%s [%s] (%s)\n", "skipped", info.data(), mnt.type.data());
        }
    }
    return 0;
}

int main(int argc, const char **argv) {
    char *argv0 = strdup(argv[0]);
    const char *bname = basename(argv0);

    if ((strcmp(bname, "magic_remount_rw") == 0) || ((argc > 1) && (strcmp(argv[1], "--remount-rw") == 0))) {
        return do_remount(0, MS_RDONLY);
    } else if ((strcmp(bname, "magic_remount_ro") == 0) || ((argc > 1) && (strcmp(argv[1], "--remount-ro") == 0))) {
        return do_remount(MS_RDONLY);
    }

    log_fd = open("/cache/overlayfs.log", O_RDWR | O_CREAT | O_APPEND, 0666);
    LOGI("* Mount OverlayFS started\n");
    
    // Other logic remains unchanged
    
    return 0;
}
