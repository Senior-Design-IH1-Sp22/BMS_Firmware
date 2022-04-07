/*
 * bits.c
 *
 *  Created on: Apr 6, 2022
 *      Author: xchen
 */

// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}

// Function to reverse `buffer[i�j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j)
        swap(&buffer[i++], &buffer[j--]);
    return buffer;
}

// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32)
        return buffer;
    // consider the absolute value of the number
    int n = abs(value);
    int i = 0;
    while (n) {
        int r = n % base;
        if (r >= 10)
            buffer[i++] = 65 + (r - 10);
        else
            buffer[i++] = 48 + r;
        n = n / base;
    }
    // if the number is 0
    if (i == 0)
        buffer[i++] = '0';
    buffer[i] = '\0'; // null terminate string
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}


