#include "core/daemon.h"

#include "core/logger.h" 

#include <unistd.h>
#include <fcntl.h>

bool daemonize() {
    int pid = fork();

    if (pid == -1) {
        Logger::get()->error("Fork error. Pid = -1.");
        exit(1);
    }

    if (pid > 0) {
        exit(0);
    }

    if (setsid() == -1) {
        Logger::get()->error("Setsid error. Return -1.");
        exit(1);
    }

    int fd_dev_null = open("/dev/null", O_RDWR);

    if (fd_dev_null == -1) {
        Logger::get()->error("Error open /dev/null. Return -1.");
        exit(1);   
    }

    dup2(fd_dev_null, STDIN_FILENO);
    dup2(fd_dev_null, STDOUT_FILENO);
    dup2(fd_dev_null, STDERR_FILENO);

    close(fd_dev_null);

    chdir("/");

    return true;
}