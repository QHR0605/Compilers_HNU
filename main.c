#include "globals.h"
#include <string.h>
int line_no = 1;
char buffer[50];
int main() {
    FILE *o = fopen("test", "w");
    FILE *f;
    f = fopen("sample.tny", "r");
    while(fgets(buffer, 49, f)) {
        fprintf(o, "%s %d\n", buffer, strlen(buffer));
    }
}