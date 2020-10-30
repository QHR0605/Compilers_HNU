#include <stdio.h>
#include <string.h>
#include "globals.h"

#define MAX_BUF_SIZE 256
#define MAX_STR_SIZE 50
typedef enum {
    START, INNUM, INID, INASSIGN, DONE, INCOMMENT
}StateType;

struct {
    char* str;
    TokenType token;
}reserved[8] = 
{{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
{"repeat",REPEAT},{"until",UNTIL},{"read",READ},{"write",WRITE}};

int buf_size = 0;
char buffer[MAX_BUF_SIZE];
int name_pos = 0;
char currString[MAX_STR_SIZE];
int line_no = 1;
int save;
FILE *fp;

int getNext() {
    int c;
    if (buf_size == 0) {
        c = getc(fp);
    }
    else {
        c = buffer[0];
        int i;
        for (i = 0; i < buf_size; i++) {
            buffer[i] = buffer[i + 1];
        }
        buf_size--;
    }
    return c;
}
void backtrace(char c) {
    buffer[buf_size++] = c;
}
TokenType getRealToken() {
    int i;
    for (i = 0; i < 8; i++) {
        if (strcmp(currString, reserved[i].str) == 0) {
            return reserved[i].token;
        }
    }
    return ID;
}

TokenType getToken() {
    StateType state = START;
    TokenType currToken;
    int save = 0;
    while(state != DONE) {
        int c = getNext();
        switch(state) {
            case START:
            if (c == ' ' || c == '\t' || c == '\n') {
                if (c == '\n') line_no++;
                state = START;
            }
            else if (c == '{') {
                state = INCOMMENT;
            }
            else if (isdigit(c)) {
                currString[name_pos++] = c;
                save = 1;
                state = INNUM;
            }
            else if (isalpha(c)) {
                currString[name_pos++] = c;
                save = 1;
                state = INID;
            }
            else if (c == ':') {
                state = INASSIGN;
            }
            else {
                state = DONE;
                switch(c) {
                    case EOF:
                    currToken = ENDFILE;
                    break;
                    case '+':
                    currToken = PLUS;
                    break;
                    case '-':
                    currToken = MINUS;
                    break;
                    case '*':
                    currToken = TIMES;
                    break;
                    case '/':
                    currToken = DIVIDE;
                    break;
                    case '=':
                    currToken = EQULE;
                    break;
                    case '<':
                    currToken = LE;
                    break;
                    case '(':
                    currToken = LB;
                    break;
                    case ')':
                    currToken = RB;
                    break;
                    case ';':
                    currToken = SEMI;
                    break;
                    default:
                    currToken = ERROR;
                }
            }
            break;
            case INNUM:
            if (!isdigit(c)) {
                backtrace(c);
                state = DONE;
                currToken = NUM;
            }
            else currString[name_pos++] = c;
            break;
            case INID:
            if (!isalpha(c)) {
                backtrace(c);
                state = DONE;
                currToken = ID;
            }
            else currString[name_pos++] = c;
            break;
            case INASSIGN:
            if (c == '=') {
                state = DONE;
                currToken = ASSIGN;
            }
            else {
                backtrace(c);
                state = DONE;
                currToken = ERROR;
            }
            break;
            case INCOMMENT:
            if (c == EOF) {
                state = DONE;
                currToken = ENDFILE;
            }
            else if (c == '}') {
                state = START;
            }
            break;
            case DONE:
            break;
            default:
            currToken = ERROR;
        }
        if (state == DONE) {
            currString[name_pos] = '\0';
            if (currToken == ID) {
                currToken = getRealToken();
            }
            name_pos = 0;
        }
    }
    return currToken;
}

void printResult(TokenType curr) {
    printf("%d\t", line_no);
    switch(curr) {
        case IF:
        printf("reserved word: if\n");
        break;
        case THEN:
        printf("reserved word: then\n");
        break;
        case ELSE:
        printf("reserved word: else\n");
        break;
        case END:
        printf("reserved word: end\n");
        break;
        case REPEAT:
        printf("reserved word: repeat\n");
        break;
        case UNTIL:
        printf("reserved word: until\n");
        break;
        case READ:
        printf("reserved word: read\n");
        break;
        case WRITE:
        printf("reserved word: write\n");
        break;
        case ASSIGN:
        printf(":=\n");
        break;
        case EQULE:
        printf("=\n");
        break;
        case LE:
        printf("<\n");
        break;
        case PLUS:
        printf("+\n");
        break;
        case MINUS:
        printf("-\n");
        break;
        case TIMES:
        printf("*\n");
        break;
        case DIVIDE:
        printf("/\n");
        break;
        case LB:
        printf("(\n");
        break;
        case RB:
        printf(")\n");
        break;
        case SEMI:
        printf(";\n");
        break;
        case NUM:
        printf("NUM, val= ");
        printf("%s\n", currString);
        currString[name_pos] = '\0';
        break;
        case ID:
        printf("ID, name= ");
        printf("%s\n", currString);
        currString[name_pos] = '\0';
        break;
        case ENDFILE:
        printf("EOF\n");
        break;
        case ERROR:
        printf("%s", buffer);
        default:
        printf("Error\n");
    }
}
int main() {
    fp = fopen("sample.tny", "r");
    freopen("out", "w", stdout);
    TokenType curr = ERROR;
    while(curr != ENDFILE) {
        curr = getToken();
        printResult(curr);
    }
}