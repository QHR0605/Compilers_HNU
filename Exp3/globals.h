#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include <stdio.h>
#include <iomanip>
#include <stdlib.h>
#include <ctype.h>
#include <stack>
#include <queue>
#include <string.h>

#define MAX_BUF_SIZE 256
#define MAX_STR_SIZE 50

typedef enum {
    ENDFILE,
    ERROR,
    IF,
    THEN,
    ELSE,
    END,
    REPEAT,
    UNTIL,
    READ,
    WRITE,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    EQULE,
    LE,
    LB,
    RB,
    SEMI,
    ASSIGN,
    NUM,
    ID
} TokenType;
typedef enum { START, INNUM, INID, INASSIGN, DONE, INCOMMENT } StateType;

struct {
    std::string str;
    TokenType token;
} reserved[8] = {{"if", IF},         {"then", THEN},   {"else", ELSE}, {"end", END},
                 {"repeat", REPEAT}, {"until", UNTIL}, {"read", READ}, {"write", WRITE}};

typedef enum { StmtK, ExpK } NodeKind;
typedef enum { IfK, RepeatK, AssignK, ReadK, WriteK } StmtKind;
typedef enum { OpK, ConstK, IdK } ExpKind;

/* ExpType is used for type checking */
typedef enum { Void, Integer, Boolean } ExpType;

#define MAXCHILDREN 7

typedef struct treeNode {
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *father;
    int child_no;
    int max_child;
    int lineno;
    std::string name;
    NodeKind nodekind;
    union {
        StmtKind stmt;
        ExpKind exp;
    } kind;
    union {
        TokenType op;
        int val;
    } attr;
    ExpType type; /* for type checking of exps */

} TreeNode;

int buf_size = 0, buf_pos = 0;
char buffer[MAX_BUF_SIZE];
int name_pos = 0;
int line_no = 0;
char currString[MAX_STR_SIZE];

TokenType token; 
#endif