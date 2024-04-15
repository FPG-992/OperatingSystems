#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>



int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("You must pass the id of the child.\nUsage: ./child 0\n");
        return 1;
    }

    pid_t pid = getpid();
    pid_t ppid = getppid();

    printf("[CHILD: %s] getpid() = %d, getppid() = %d\n", argv[1], pid, ppid);

    return 0;
}
