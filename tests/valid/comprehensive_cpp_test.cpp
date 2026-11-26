#include <iostream>
using namespace std;

int main() {

    // 1. Variable declaration & assignment
    int a = 10;
    int b = 3;
    int c = 7;
    float h = 5.7;
    float d = 2.5;
    bool ok = true;

    cout << "--- 1. Variables ---" << endl;
    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    cout << h << endl;
    cout << d << endl;
    cout << ok << endl;

    // 2. Arithmetic
    cout << "--- 2. Arithmetic ---" << endl;
    cout << a + b << endl;
    cout << a - b << endl;
    cout << a * b << endl;
    cout << a / b << endl;
    cout << a % b << endl;
    cout << h * 2.0 << endl;
    cout << d * d << endl;

    // 3. Relational
    cout << "--- 3. Relational ---" << endl;
    cout << (a > b) << endl;
    cout << (a < b) << endl;
    cout << (a >= 10) << endl;
    cout << (b <= 2) << endl;
    cout << (a == b) << endl;
    cout << (a != b) << endl;

    // 4. Logical
    cout << "--- 4. Logical ---" << endl;
    cout << ((a > 0) && (b > 0)) << endl;
    cout << ((a > 0) || (b < 0)) << endl;
    cout << (!ok) << endl;
    cout << ((a > b) && (b > c)) << endl;
    cout << (!(a < b) && ok) << endl;

    // 5. Simple if
    cout << "--- 5. if ---" << endl;
    if (a > 0) {
        cout << 1 << endl;
    }
    if (a > 100) {
        cout << 2 << endl;
    }
    cout << 3 << endl;

    // 6. if-else (even / odd)
    cout << "--- 6. if-else ---" << endl;
    if (a % 2 == 0) {
        cout << 100 << endl;
    } else {
        cout << 200 << endl;
    }
    if (c % 2 == 0) {
        cout << 100 << endl;
    } else {
        cout << 200 << endl;
    }

    // 7. else-if ladder
    cout << "--- 7. else-if ladder ---" << endl;
    int marks = 75;
    if (marks >= 80) {
        cout << 4 << endl;
    } else if (marks >= 70) {
        cout << 3 << endl;
    } else if (marks >= 60) {
        cout << 2 << endl;
    } else {
        cout << 0 << endl;
    }

    // 8. Nested if
    cout << "--- 8. Nested if ---" << endl;
    int x = 5;
    int y = -2;
    if (x > 0) {
        if (y > 0) {
            cout << 11 << endl;
        } else {
            cout << 10 << endl;
        }
    } else {
        cout << 0 << endl;
    }

    // 9. Largest & smallest of three
    cout << "--- 9. Largest / Smallest ---" << endl;
    int largest = a;
    int smallest = a;
    if (b > largest) {
        largest = b;
    }
    if (c > largest) {
        largest = c;
    }
    if (b < smallest) {
        smallest = b;
    }
    if (c < smallest) {
        smallest = c;
    }
    cout << largest << endl;
    cout << smallest << endl;

    // 10. Leap year
    cout << "--- 10. Leap year ---" << endl;
    int year = 2024;
    bool leap;
    if (year % 400 == 0) {
        leap = true;
    } else if (year % 100 == 0) {
        leap = false;
    } else if (year % 4 == 0) {
        leap = true;
    } else {
        leap = false;
    }
    cout << leap << endl;

    // 11. Combined
    cout << "--- 11. Combined ---" << endl;
    int n = -4;
    if (n > 0) {
        cout << 1 << endl;
    } else if (n < 0) {
        cout << -1 << endl;
    } else {
        cout << 0 << endl;
    }
    if (n % 2 == 0 && n != 0) {
        cout << 100 << endl;
    } else {
        cout << 200 << endl;
    }

    return 0;
}
