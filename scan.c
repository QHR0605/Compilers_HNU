#include "globals.h"
#include "util.h"
#include "scan.h"

/* state in scanner DFA */
typedef enum {
    START,
    INNUM,
    INID,
    INTOCOMMENT,
    INCOMMENT,
    OUTOFCOMMENT,
    INASSIGN,
    INSE,
    INLE,
    INNEQ,
    DONE
} StateType;

/* lexeme if identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
 * source code lines
 */
#define BUFLEN 256

static char lineBuf[BUFLEN]; // hold the current line
static int linepos = 0;      // current pos in lineBuf
static int bufsize = 0;      // current size of the lineBuf is
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void) {
    if (!(linepos < bufsize)) {
        lineno++;
        if (fgets(lineBuf, BUFLEN - 1, source)) {
            if (EchoSource)
                fprintf(listing, "%4d: %s", lineno, lineBuf);
            bufsize = strlen(lineBuf);
            linepos = 0;
            return lineBuf[linepos++];
        } else {
            EOF_flag = TRUE;
            return EOF;
        }
    } else
        return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void) {
    if (!EOF_flag)
        linepos--;
}

/* lookup table of reserved words */
static struct {
    char *str;
    TokenType tok;
} reservedWords[MAXRESERVED] = {{"else", ELSE},     {"if", IF},     {"int", INT},
                                {"return", RETURN}, {"void", VOID}, {"while", WHILE}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char *s) {
    int i;
    for (i = 0; i < MAXRESERVED; i++)
        if (!strcmp(s, reservedWords[i].str))
            return reservedWords[i].tok;
    return ID;
}

/***************************************/
/* The primary function of the scanner */
/***************************************/
/* function getToken returns the next
 * token in the source file
 */
TokenType getToken(void) { /* index for storing into tokenString */
    int tokenStringIndex = 0;
    /* holds current token to be returned */
    TokenType currentToken;
    /* current state - always begins at START */
    StateType state = START;
    /* flag to indicate save to tokenString */
    int save;

    while (state != DONE) {
        char c = getNextChar();
        save = TRUE;
        switch (state) {
        case START:
            if (isdigit(c))
                state = INNUM;
            else if (isalpha(c))
                state = INID;
            else if (c == '!')
                state = INNEQ;
            else if (c == '=')
                state = INASSIGN;
            else if (c == '/') {
                save = FALSE;
                state = INTOCOMMENT;
            } else if (c == '<')
                state = INSE;
            else if (c == '>')
                state = INLE;
            else if ((c == ' ') || (c == '\t') || (c == '\n'))
                save = FALSE;
            else {
                state = DONE;
                switch (c) {
                case EOF:
                    currentToken = ENDFILE;
                    break;
                case '+':
                    currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case ',':
                    currentToken = COMMA;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case '[':
                    currentToken = SLPAREN;
                    break;
                case ']':
                    currentToken = SRPAREN;
                    break;
                case '{':
                    currentToken = CLPAREN;
                    break;
                case '}':
                    currentToken = CRPAREN;
                    break;
                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break; /* End of case 'state == START' */
        case INNEQ:
            if (c == '=') {
                save = FALSE;
                state = DONE;
                currentToken = NEQ; // Not Equal
            } else {
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break; /* End of case 'state == INNEQ' */
        case INASSIGN:
            if (c == '=') {
                save = FALSE;
                state = DONE;
                currentToken = EQ;
            } else {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ASSIGN;
            }
            break; /* End of case 'state == INASSIGN' */
        case INLE:
            if (c == '=') {
                save = FALSE;
                state = DONE;
                currentToken = LE; // Large or Equal
            } else {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = LG; // Large
            }
            break; /* End of case 'state == INLE' */
        case INSE:
            if (c == '=') {
                save = FALSE;
                state = DONE;
                currentToken = SE; // Small or equal
            } else {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = SM; // Small
            }
            break; /* End of case 'state == INSE' */
        case INTOCOMMENT:
            save = FALSE;
            if (c == '*')
                state = INCOMMENT;
            else {
                state = DONE;
                currentToken = OVER;
                ungetNextChar();
            }
            break; /* End of case 'state == INTOCOMMENT' */
        case INCOMMENT:
            save = FALSE;
            if (c == '*')
                state = OUTOFCOMMENT;
            break; /* End of case 'state == INCOMMENT' */
        case OUTOFCOMMENT:
            save = FALSE;
            if (c == '/')
                state = START;
            else
                state = INCOMMENT;
            break; /* End of case 'state == OUTOFCOMMENT' */
        case INNUM:
            if (!isdigit(c)) {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break; /* End of case 'state == INNUM' */
        case INID:
            if (!isalpha(c)) {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ID;
            }
            break; /* End of case 'state == INID' */
        case DONE: /* should never happen */
            fprintf(listing, "Scanner Bug:state = %d\n", state);
            state = DONE;
            currentToken = ERROR;
            break;
        }
        if ((save) && (tokenStringIndex <= MAXTOKENLEN))
            tokenString[tokenStringIndex++] = (char)c;
        if (state == DONE) {
            tokenString[tokenStringIndex] = '\0';
            if (currentToken == ID)
                currentToken = reservedLookup(tokenString);
        }
    }
    if (TraceScan) {
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
} /* end getToken */
