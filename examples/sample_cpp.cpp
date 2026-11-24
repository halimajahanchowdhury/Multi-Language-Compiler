#include <iostream>
using namespace std;

int main() {
    int a;
    int b;
    bool isGreater;

    a = 15;
    b = 7;
    isGreater = (a > b);

    if (isGreater == true) {
        cout << "a is greater than b" << endl;
    } else {
        cout << "b is greater or equal" << endl;
    }

    return 0;
}
