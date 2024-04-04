#include <stdio.h>
#include <stdlib.h>


int main() {
    int gIndex = 9; // The index of the child
    int val;

    // The read and write-end of the pipe
    int in, out;

    while(1) {
        read(in, &val, sizeof(int));
        printf("[Child] [%d] Child received %d!\n", gIndex, getpid(), val);
        val--;
        sleep(10);
        write(out, &val, sizeof(int));
        printf("[Child %d] [%d] Child Finished hard work, writing back %d\n", gIndex, getpid(), val);
    }
}
