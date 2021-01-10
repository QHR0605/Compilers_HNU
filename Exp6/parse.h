#ifndef _PARSE_H_
#define _PARSE_H_

/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode *parse(void);

/* assistant functions */
int isOp(TokenType token);
int isRelop(TokenType token);
int isAddop(TokenType token);
int isMulop(TokenType token);

#endif
