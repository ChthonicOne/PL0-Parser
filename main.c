#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int pos = 0;
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
void ident(int kind);
void getIdent(char * name);
void storeIdent(char * name);
void number();

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
        consume(eqlsym);
        number();
        while (tok.idNum == commasym)
        {
            consume(commasym);
            ident(1);
            consume(eqlsym);
            number();
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
        while (tok == commasym)
        {
            consume(commasym);
            ident(2);
        }
        consume(semicolonsym);
    }
}

void statement()
{
    switch (tok.idNum)
    {
        case identsym : char id[13];
                        strcpy(id, tok.ident);
                        consume(identsym);
                        consume(becomessym);
                        expression();
                        storeIdent(id);
                        break;
        case beginsym : consume(beginsym);
                        statement();
                        while (tok == semicolonsym)
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
        default       :
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
            default     : consume(eqlsym);  // If it's not one of these we need an error of some kind.
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
    if (tok.idNum == plussym)
    {
        consume(plussym);
    }
    else if (tok.idNum == minussym)
    {
        consume(minussym);
    }
    term();
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

void consume(int last);
void speak(int op, int l, int m);
void ident(int kind);
void getIdent(char * name);
void storeIdent(char * name);
void number();
