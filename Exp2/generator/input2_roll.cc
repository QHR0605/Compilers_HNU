/* (ε|b|bb)(a|ab|abb)* */
#include <iostream>
#include <random>
#include <time.h>
using namespace std;

int main() {
    srand(time(0));
    freopen("../input_file/input2_", "w", stdout);
    char const *s[2] = {"b\0", "bb\0"};
    char const *s_[3] = {"a\0", "ab\0", "abb\0"};
    for (int i = 0; i <= 4; i++) {
        for (int times = 0; times < 3; times++) {
            int choice = rand() % 3;
            if (choice != 2)
                printf("%s", s[choice]);
            for (int j = 0; j < i; j++) {
                int choice = rand() % 3;
                printf("%s", s_[choice]);
            }
            cout << endl;
        }
    }
    for (int num = 0; num < 8; num++) {
        int times = rand() % 7 + 1;
        for (int i = 0; i < times; i++) {
            int choice = rand() % 6;
            if (choice >= 3) {
                printf("%s", s_[choice - 3]);
            }
            else if (choice == 2) {
                continue;
            }
            else printf("%s", s[choice]);
        }
        cout << endl;
    }
}