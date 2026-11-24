#include <stdio.h>

int main() {
    int n;
    int sum;
    int i;

    n = 5;
    sum = 0;
    i = 1;

    while (i <= n) {
        sum = sum + i;
        i++;
    }

    if (sum > 10) {
        printf("Sum is greater than 10\n");
    } else {
        printf("Sum is small\n");
    }

    return 0;
}
