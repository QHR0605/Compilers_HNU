#include <iostream>
using namespace std;

int main() {
    freopen("./input_file/input2_", "r", stdin);
    string s;
    int cnt = 0;
    while(getline(cin, s)) {
        if (cnt == 30)
            return 1;
        cout << s << endl;
        cnt++;
    }
}