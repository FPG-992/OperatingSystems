#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// Function to generate a random number between min and max (inclusive)
int random_between(int min, int max) {
    if (min >= max) {
        printf("Error: min must be less than max\n");
        exit(EXIT_FAILURE);
    }
    return min + rand() % (max - min + 1);
}


int main() {
    // Seed the random number generator
    srand(time(NULL));

    int min = 1;
    int max = 10;

    // Generate and print a random number between min and max
    int random_num = random_between(min, max);
    printf("Random number between %d and %d: %d\n", min, max, random_num);

    return 0;
}
