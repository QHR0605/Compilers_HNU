#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    ENDFILE, ERROR,
    IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
    PLUS, MINUS, TIMES, DIVIDE, EQULE, LE, LB, RB, SEMI, ASSIGN,
    NUM, ID
}TokenType;
