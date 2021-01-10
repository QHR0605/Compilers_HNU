/****************************************************/
/* File: globals.h                                  */
/* Yacc/Bison Version                               */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8


typedef enum 
{
    /* Bookkeeping tokens */
    ENDOFFILE, ERROR,
    /* Reserved words */
    IF, ELSE, INT, RETURN, VOID, WHILE,
    /* More complex tokens */
    ID, NUM,
    /* Various operators */
    PLUS, MINUS, TIMES, DIVIDE, LT, GT, ASSIGN, NE, SEMI, COMMA,
    LPAREN, RPAREN, LSQUARE, RSQUARE, LBRACE, RBRACE, LTE, GTE, EQ
}TokenType;

extern FILE *source;  /* source code text file */
extern FILE *listing; /* listing output text file */
extern FILE *code;    /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum { StmtK, ExpK, DecK } NodeKind;
typedef enum { IfK, WhileK, ReturnK, CallK, CompoundK } StmtKind;
typedef enum { OpK, IdK, ConstK, AssignK } ExpKind;
typedef enum { IdDecK, ArrayDecK, FuncDecK } DecKind;

/* Used in the type checker */
typedef enum { Void, Integer, Boolean, Array, Function } ExpType;

#define MAXCHILDREN 3

typedef struct treeNode 
{
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *sibling;
    int lineno;

    NodeKind nodekind;  /* the type of the AST node */

    union {
        StmtKind stmt;
        ExpKind exp;
        DecKind dec;
    } kind;

    TokenType op;
    int val;
    char *name;
    
    ExpType functionReturnType;
    ExpType variableDataType;
    ExpType expressionType;

    int isParameter;
    int isGlobal;
    struct treeNode *declaration;
    int assemblyAreaSize;
    int localSize;
    int offset;
    
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyse;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;

extern char preTokenString[40 + 1];
#endif
