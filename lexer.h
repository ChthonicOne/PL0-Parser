#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

int getNextToken(FILE *inFile, int *ftoken, char *value);    // Gets the next token in the file
int nextState(int state, char next);    // Retrieves next state for analyzeTokens

#endif // LEXER_H_INCLUDED
