#include <stdio.h>

int main() {
    int marks = 75;
    if (marks >= 80) {
        printf("Grade: A+\n");
    } else if (marks >= 70) {
        printf("Grade: A\n");
    } else {
        printf("Grade: B\n");
    }

    int count = 5;
    while (count > 0) {
        printf("%d\n", count);
        count--;
    }
    return 0;
}
