/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <assert.h>
#include "globals.h"
#include "util.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYSE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE TRUE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYSE
#include "analyse.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE *source;
FILE *listing;
FILE *code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyse = TRUE;
int TraceCode = FALSE;

int Error = FALSE;

int main(int argc, char *argv[]) {
    TreeNode *syntaxTree;
    char pgm[120]; /* source code file name */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    strcpy(pgm, argv[1]);
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".tny");
    source = fopen(pgm, "r");
    if (source == NULL) {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }
    listing = stdout; /* send listing to screen */
    fprintf(listing, "\nCMINUS COMPILATION: %s\n", pgm);
#if NO_PARSE
    while (getToken() != ENDOFFILE)
        ;
#else
    syntaxTree = parse();
    if (TraceParse) {
        fprintf(listing, "\nSyntax tree:\n");
        printTree(syntaxTree);
    }
#if !NO_ANALYZE
    if (!Error) {
        if (TraceAnalyse)
            fprintf(listing, "\nBuilding Symbol Table...\n");
        buildsymtab(syntaxTree);
        assert(Error == FALSE);
        if (TraceAnalyse)
            fprintf(listing, "\nChecking Types...\n");
        typeCheck(syntaxTree);
        if (TraceAnalyse)
            fprintf(listing, "\nType Checking Finished\n");
    }
#if !NO_CODE
    if (!Error) {
        char *codefile;
        int fnlen = strcspn(pgm, ".");
        codefile = (char *)calloc(fnlen + 4, sizeof(char));
        strncpy(codefile, pgm, fnlen);
        strcat(codefile, ".tm");
        code = fopen(codefile, "w");
        if (code == NULL) {
            printf("Unable to open %s\n", codefile);
            exit(1);
        }
        codeGen(syntaxTree, codefile);
        fclose(code);
    }
#endif
#endif
#endif
    fclose(source);
    return 0;
}
