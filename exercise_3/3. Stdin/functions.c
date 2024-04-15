#include <stdlib.h>


int is_digit(const char* input) {
    int i = 0;
    while (i < strlen(input)) {
        if ((int) input[i] < 48 || (int) input[i] > 57) {
            return 0;
        }
        ++i;
    }

    return 1;
}


int string_to_int(const char *str) {
    int result = 0;
    int sign = 1; // Sign of the number, initialized as positive
    
    // Handle leading whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n')
        str++;
    
    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convert each character to integer
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    // Apply sign
    result *= sign;
    
    return result;
}


// Function to get the number of digits in an integer
int num_digits(int num) {
    int count = 0;
    if (num == 0)
        return 1;
    while (num != 0) {
        num /= 10;
        count++;
    }
    return count;
}


// Function to convert an integer to a string
char *int_to_string(int num) {
    int sign = 1;
    if (num < 0) {
        sign = -1;
        num *= sign;
    }

    int n = num_digits(num);
    int i = n - 1; // Start from the end of the string
    char *str = (char*) malloc((n + 1 + (sign == -1 ? 1 : 0)) * sizeof(char)); // +1 for '\0', +1 for negative sign if needed
    
    if (str == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    // Handle negative numbers
    if (sign == -1) {
        str[0] = '-';
        i ++;
    }
    
    // Convert each digit to character
    while (num != 0) {
        str[i] = (num % 10) + '0';
        num /= 10;
        i--;
    }

    // Add null terminator
    str[n + (sign == -1 ? 1 : 0)] = '\0';
    
    return str;
}
