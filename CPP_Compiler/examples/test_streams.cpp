#include <iostream>
using namespace std;

int main() {
    int a = 10, b = 3, c = 7;
    float h = 5.7;
    bool ok = true;

    cout << "--- Variables ---" << endl;
    cout << a << " " << b << " " << c << endl;
    cout << h << " " << ok << endl;

    if (a > b) {
        cout << "a is greater than b" << endl;
    } else {
        cout << "b is greater or equal" << endl;
    }

    return 0;
}
