#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

#define MSTS 100

typedef struct symbol
{
    int kind;       // const = 1, var = 2, proc = 3
    char name[13];  // name up to 12 chars
    int val;        // number
    int level;      // L level
    int addr;       // M address
} symbol;

typedef struct token
{
    int idNum;
    char ident[13];
    int value;
} token;

enum Token_Name
{
    nulsym = 1,
    identsym = 2,
    numbersym = 3,
    plussym = 4,
    minussym = 5,
    multsym = 6,
    slashsym = 7,
    oddsym = 8,
    eqlsym = 9,
    neqsym = 10,
    lessym = 11,
    leqsym = 12,
    gtrsym = 13,
    geqsym = 14,
    lparentsym = 15,
    rparentsym = 16,
    commasym = 17,
    semicolonsym = 18,
    periodsym = 19,
    becomessym = 20,
    beginsym = 21,
    endsym = 22,
    ifsym = 23,
    thensym = 24,
    whilesym = 25,
    dosym = 26,
    callsym = 27,
    constsym = 28,
    varsym = 29,
    procsym = 30,
    writesym = 31,
    readsym = 32,
    elsesym = 33
};

symbol symbolTable[MSTS];
int pos = 0, frameSize = 4;
token tok;

void program();
void block();
void constDec();
void varDec();
void statement();
void condition();
void expression();
void term();
void factor();

void consume(int last);
void speak(int op, int l, int m);
void ident(int kind);               //Adds ident to symbol table
void getIdent(char * name);         //Finds memory address in symbol table and pushes value to top of stack
void storeIdent(char * name);       //Finds memory address in symbol table and stores top of stack there


int main()
{
    printf("Hello world!\n");
    return 0;
}

void program()
{
    block();
    consume(periodsym);
    speak(9, 0, 2);
}

void block()
{
    constDec();
    varDec();
    statement();
}

void constDec()
{
    if (tok.idNum == constsym)
    {
        consume(constsym);
        ident(1);
        /* consume(eqlsym); // Ident will handle this
        number(); */
        while (tok.idNum == commasym)
        {
            consume(commasym);
            ident(1);
        }
        consume(semicolonsym);
    }
}

void varDec()
{
    if (tok.idNum == varsym)
    {
        consume(varsym);
        ident(2);
        while (tok.idNum == commasym)
        {
            consume(commasym);
            ident(2);
        }
        consume(semicolonsym);
    }
    speak(6, 0, frameSize);         //Set up our stack frame with room for all of our variables
}

void statement()
{
    char id[13];
    switch (tok.idNum)
    {
        case identsym : strcpy(id, tok.ident);
                        consume(identsym);
                        consume(becomessym);
                        expression();
                        storeIdent(id);
                        break;
        case beginsym : consume(beginsym);
                        statement();
                        while (tok.idNum == semicolonsym)
                        {
                            consume(semicolonsym);
                            statement();
                        }
                        consume(endsym);
                        break;
        case ifsym    : consume(ifsym);
                        condition();
                        consume(thensym);
                        statement();
                        break;
        case whilesym : consume(whilesym);
                        condition();
                        consume(dosym);
                        statement();
                        break;
        case readsym  : consume(readsym);
                        speak(9, 0, 1);
                        storeIdent(tok.ident);
                        break;
        case writesym : consume(writesym);
                        getIdent(tok.ident);
                        speak(9, 0, 0);
                        break;
        default       : break;
    }
}

void condition()
{
    if (tok.idNum == oddsym)
    {
        consume(oddsym);
        expression();
    } else
    {
        expression();
        int op = tok.idNum;
        switch(tok.idNum)
        {
            case eqlsym : consume(eqlsym);
                          break;
            case neqsym : consume(neqsym);
                          break;
            case lessym : consume(lessym);
                          break;
            case leqsym : consume(leqsym);
                          break;
            case gtrsym : consume(gtrsym);
                          break;
            case geqsym : consume(geqsym);
                          break;
            default     : consume(neqsym);  // If it's not one of these we need an error of some kind.
        }
        expression();
        switch(op)
        {
            case eqlsym : speak(2, 0, 8);
                          break;
            case neqsym : speak(2, 0, 9);
                          break;
            case lessym : speak(2, 0, 10);
                          break;
            case leqsym : speak(2, 0, 11);
                          break;
            case gtrsym : speak(2, 0, 12);
                          break;
            case geqsym : speak(2, 0, 13);
                          break;
        }
    }
}

void expression()
{
    int isNeg=0;
    if (tok.idNum == plussym)
    {
        consume(plussym);
    }
    else if (tok.idNum == minussym)
    {
        consume(minussym);
        isNeg = 1;
    }
    term();
    if (isNeg)
        speak (2, 0, 1);
    while (tok.idNum == plussym || tok.idNum == minussym)
    {
        if (tok.idNum == plussym)
            consume(plussym);
        else
            consume(minussym);
        term();
    }
}

void term()
{
    factor();
    while (tok.idNum == multsym || tok.idNum == slashsym)
    {
        if (tok.idNum == multsym)
            consume(multsym);
        else
            consume(slashsym);
        factor();
    }
}

void factor()
{
    if (tok.idNum == identsym)
    {
        getIdent(tok.ident);
    }else if (tok.idNum == numbersym)
    {
        speak(1, 0, tok.value);
    } else
    {
        consume(lparentsym);
        expression();
        consume(rparentsym);
    }
}

void consume(int last)
{

}

void speak(int op, int l, int m)
{

}

void ident(int kind)
{
    int i;
    for (i=0; i<pos; i++)
    {
        if (strcmp(tok.ident, symbolTable[i].name) == 0)    //If there's already an identifier in our list with that name
        {
            printf("Error, duplicate identifier\n");    //Can't have two identifiers in the list at the same level with the same name
            exit(0);
        }
    }


    if (pos == 100)
    {
        printf("Too many symbols in the symbol table\n");
        exit(0);
    }else if (kind == 1)                                //If our ident is a constant
    {
        symbolTable[pos].kind = 1;                      //Mark the ident as a constant
        strcpy(symbolTable[pos].name, tok.ident);       //Copy the name of the constant into the table
        consume(identsym);                              //Next symbol
        consume(eqlsym);                                //Next symbol
        symbolTable[pos].val = tok.value;               //Save the value of the constant into the table
        consume(numbersym);                             //Next symbol
    }else if (kind == 2)                                //If our ident is a variable
    {
        symbolTable[pos].kind = 2;                      //Mark it as a variable
        strcpy(symbolTable[pos].name, tok.ident);     //Copy the name into the table
        symbolTable[pos].level = 0;                     //We don't have procedures so everything's L level is 0
        symbolTable[pos].addr = frameSize + 1;          //Save the memory position of the variable into the table
        frameSize++;                                    //Increase the frame size
        consume(identsym);                              //Next symbol
    }
    pos++;                                              //Next position in the symbol table
}

void getIdent(char * name)
{
    int i, loc = -1;
    for (i=0; i<pos; i++)                               //Search for the identifier in the table
    {
        if (strcmp(name, symbolTable[i].name) == 0)
        {
            loc = i;
        }
    }
    if (loc == -1)
    {
        printf("Identifier not declared in symbol table\n");
        exit(0);
    }
    if (symbolTable[loc].kind == 1)                     //If it's a constant
    {
        speak(1, 0, symbolTable[loc].val);              //Put the value on the stack
    }else                                               //Otherwise it's a variable
    {
        speak(3, symbolTable[loc].level, symbolTable[loc].addr);    //Load it's value from memory, and put it on the top of the stack
    }
}

void storeIdent(char * name)
{
    int i, loc = -1;
    for (i=0; i<pos; i++)
    {
        if (strcmp(name, symbolTable[i].name) == 0)
        {
            loc = i;
        }
    }
    if (loc == -1)
    {
        printf("Identifier not declared in symbol table\n");
        exit(0);
    }
    if (symbolTable[loc].kind == 1)                     //If it's a constant
    {
        printf("Cannot change the value of a constant\n");  //Can't change a constant
        exit(0);
    } else                                              //Otherwise it's a variable and we can store it
    {
        speak(4, symbolTable[loc].level, symbolTable[loc].addr);
    }
}


