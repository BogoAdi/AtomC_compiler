#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include "add.h"

enum
{
    /* IDENTIFCATORI & CONSTANTE */
    ID, CT_INT, CT_REAL, CT_CHAR, CT_STRING,
    /*CUVINTE CHEIE */
    BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE,
    /*DELIMITATORI */
    COMMA, SEMICOLON, LPAR, RPAR, LBRACKET, RBRACKET,LACC, RACC, END,
    /*OPERATORI */
    ADD, SUB, MUL, DIV, DOT, AND, OR, NOT, ASSIGN, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ
}; /* codurile AL*/


typedef struct _Token
{
    int code;   /*codul(nume) */
    union
    {
        char *text;
        int i;
        double r;
    };
    int line;
    struct _Token *next;
} Token;
char bufin[30001];
char * pCrtCh;
Token *tokens, *lastToken;
int line=1;
void EliberareMemorie()
{
    Token *p;
    while(tokens!=NULL)
    {
        p=tokens;
        tokens=tokens->next;
        free(p);
    }
}
char *createString(const char *pStartCh,char *pCrtCh)
{
    char *fin;
    int n=pCrtCh-pStartCh;
    if((fin=(char*)malloc((n+1)*sizeof(char)))==NULL)
    {
        printf("Eroare la alocarea dinamica");
        exit(EXIT_FAILURE);
    }
    strncpy(fin,pStartCh,n);
    fin[n]='\0';
//    printf("%s\n",fin);
    return fin;
}
void err(const char *fmt,...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr,"error: ");
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    EliberareMemorie();
    exit(-1);
}

void tkerr(const Token *tk, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error in line %d: ",tk->line-1);
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    EliberareMemorie();
    va_end(va);
    exit(-1);
}

#define SAFEALLOC(var, Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");
Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk,Token);

    tk->code=code;
    tk->line=line;
    tk->next=NULL;

    if(lastToken)
    {
        lastToken->next=tk;
    }
    else
    {
        tokens=tk;
    }
    lastToken=tk;
    // printf("%d linie    %d\n",tk->line, tk->code);
    return tk;

}

int getNextToken()
{
    int state=0,nCh;
    char ch;
    const char *pStartCh;
    Token *tk;

    for(;;)
    {
        ch=*pCrtCh;
        switch(state)
        {
        case 0:
            if(isalpha(ch)||ch=='_')
            {
                pStartCh=pCrtCh;
                pCrtCh++;
                state=1;
            }
            else if(isdigit(ch))
            {
                pStartCh=pCrtCh;
                pCrtCh++;
                state=3;
            }
            else if( ch == '\'' )
            {
                pStartCh=pCrtCh;
                pCrtCh++;
                state=11;
            }
            else if(ch == '\"' )
            {
                pStartCh=pCrtCh;
                pCrtCh++;
                state=14;
            }
            else if( ch == ',')
            {
                pCrtCh++;
                state=16;
            }
            else if(ch == ';')
            {
                pCrtCh++;
                state=17;
            }
            else if(ch == '(')
            {
                pCrtCh++;
                state=18;
            }
            else if(ch == ')')
            {
                pCrtCh++;
                state=19;
            }
            else if(ch == '[')
            {
                pCrtCh++;
                state=20;
            }
            else if(ch == ']')
            {
                pCrtCh++;
                state=21;
            }
            else if( ch == '{')
            {
                pCrtCh++;
                state=22;
            }
            else if( ch == '}')
            {
                pCrtCh++;
                state=23;
            }
            else if( ch == '+' )
            {
                pCrtCh++;
                state=25;
            }
            else if( ch == '-')
            {
                pCrtCh++;
                state=26;
            }
            else if( ch == '*')
            {
                pCrtCh++;
                state=27;
            }
            else if( ch == '/')
            {
                pCrtCh++;
                state=28;
            }
            else if(ch == '.')
            {
                pCrtCh++;
                state=32;
            }
            else if(ch == '&')
            {
                pCrtCh++;
                state=33;
            }
            else if( ch =='|')
            {
                pCrtCh++;
                state=35;
            }
            else if( ch == '!')
            {
                pCrtCh++;
                state=37;
            }
            else if( ch == '=')
            {
                pCrtCh++;
                state=40;
            }
            else if( ch == '<')
            {
                pCrtCh++;
                state=43;
            }
            else if( ch == '>')
            {
                pCrtCh++;
                state=46;
            }
            else if(ch == '\0'|| ch == EOF )
            {
                pCrtCh++;
                state=24;
            }
            else if(ch =='\n')
            {
                line++;
                pCrtCh++;
            }
            else if(ch =='\t'||ch =='\r'|| ch==' ')
                pCrtCh++;
            else
                tkerr(addTk(END),"caracter invalid");
            break;
        case 1:
            if(isalpha(ch)||isdigit(ch)|| ch == '_')
                pCrtCh++;
            else
                state=2;
            break;
        case 2:
            nCh=pCrtCh-pStartCh;
            if(nCh==5&&!memcmp(pStartCh,"break",5))
                tk=addTk(BREAK);
            else if(nCh==4&&!memcmp(pStartCh,"char",4))
                tk=addTk(CHAR);
            else if(nCh==6&&!memcmp(pStartCh,"double",6))
                tk=addTk(DOUBLE);
            else if(nCh==4&&!memcmp(pStartCh,"else",4))
                tk=addTk(ELSE);
            else if(nCh==3&&!memcmp(pStartCh,"for",3))
                tk=addTk(FOR);
            else if(nCh==2&&!memcmp(pStartCh,"if",2))
                tk=addTk(IF);
            else if(nCh==3&&!memcmp(pStartCh,"int",3))
                tk=addTk(INT);
            else if(nCh==6&&!memcmp(pStartCh,"return",6))
                tk=addTk(RETURN);
            else if(nCh==6&&!memcmp(pStartCh,"struct",6))
                tk=addTk(STRUCT);
            else if(nCh==4&&!memcmp(pStartCh,"void",4))
                tk=addTk(VOID);
            else if(nCh==5&&!memcmp(pStartCh,"while",5))
                tk=addTk(WHILE);

            // teste cuvinte cheie
            else  // daca nu este un cuvant cheie, atunci e un ID
            {
                tk=addTk(ID);
                tk->text=createString(pStartCh,pCrtCh);
            }
            return tk->code;

        case 3:
            if(isdigit(ch))
            {
                pCrtCh++;
                break;
            }
            if(ch == '.')
            {
                pCrtCh++;
                state=5;
                break;
            }
            if(ch == 'e' || ch =='E')
            {
                pCrtCh++;
                state=7;
                break;
            }
            state=4;
            break;

        case 4:
            tk=addTk(CT_INT);
            tk->i=atoi(pStartCh);
            //printf("%d",atoi(createString(pStartCh,pCrtCh)));
            return tk->code;

        case 5:
            if(isdigit(ch))
            {
                pCrtCh++;
                state=6;
            }
            else
                tkerr(addTk(END),"caracter invalid");
            break;
        case 6:
            if(isdigit(ch))
            {
                pCrtCh++;
            }
            else if(ch =='E'|| ch =='e')
            {
                pCrtCh++;
                state=7;
            }
            else
                state=10;
            break;
        case 7:
            if(ch == '-'|| ch == '+')
            {
                pCrtCh++;
                state=8;
            }
            state=9;
            break;
        case 8:
            if( isdigit(ch) )
            {
                pCrtCh++;
                state=9;
            }
            else
                tkerr(addTk(END),"caracter invalid");
            break;
        case 9:
            if(isdigit(ch))
                pCrtCh++;
            else
                state=10;
            break;
        case 10:
            tk=addTk(CT_REAL);
            tk->r=atof(createString(pStartCh,pCrtCh));
            return tk->code;
        case 11:
            if(ch == '\'')
                tkerr(addTk(END),"caracter invalid");
            state=12;
            pCrtCh++;
            break;
        case 12:
            if(ch =='\'')
            {
                state=13;
                pCrtCh++;
            }
            else
                tkerr(addTk(END),"caracter invalid");
            break;
        case 13:
            tk=addTk(CT_CHAR);
            tk->i=*(pStartCh+1);
            return tk->code;
        case 14:
            if(ch != '\"')
            {
                state=14;
                pCrtCh++;
            }
            else
            {
                state=15;
                pCrtCh++;
            }
            break;
        case 15:
            tk=addTk(CT_STRING);
            tk->text=createString(pStartCh+1,pCrtCh-1);
            return tk->code;//discutabil
        case 16:
            addTk(COMMA);
            return COMMA;
        case 17:
            addTk(SEMICOLON);
            return SEMICOLON;
        case 18:
            addTk(LPAR);
            return LPAR;
        case 19:
            addTk(RPAR);
            return RPAR;
        case 20:
            addTk(LBRACKET);
            return LBRACKET;
        case 21:
            addTk(RBRACKET);
            return RBRACKET;
        case 22:
            addTk(LACC);
            return LACC;
        case 23:
            addTk(RACC);
            return RACC;
        case 24:
            addTk(END);
            return END;
        case 25:
            addTk(ADD);
            return ADD;
        case 26:
            addTk(SUB);
            return SUB;
        case 27:
            addTk(MUL);
            return MUL;
        case 28:
            if(ch =='/')
            {
                state=30;
                pCrtCh++;
            }
            else
                state=29;
            break;
        case 29:
            addTk(DIV);
            return DIV;
        case 30:
            if(!(ch == '\n'||ch =='\r'|| ch == '\0'))
                pCrtCh++;
            else
            {
                state=31;
                pCrtCh++;
            }
            break;
        case 31:
            state=0;
            break;
        case 32:
            addTk(DOT);
            return DOT;
        case 33:
            if(ch =='&')
            {
                state=34;
                pCrtCh++;
            }
            else
                tkerr(addTk(END),"caracter invalid");
            break;
        case 34:
            addTk(AND);
            return AND;
        case 35:
            if( ch =='|')
            {
                state=36;
                pCrtCh++;
            }
            else
                tkerr(addTk(END),"caracter invalid");
        case 36:
            addTk(OR);
            return OR;
        case 37:
            if( ch == '=' )
            {
                state=39;
                pCrtCh++;
            }
            else
                state=38;
            break;
        case 38:
            addTk(NOT);
            return NOT;
        case 39:
            addTk(NOTEQ);
            return NOTEQ;
        case 40:
            if( ch == '=')
            {
                state=42;
                pCrtCh++;
            }
            else
                state=41;
            break;
        case 41:
            addTk(ASSIGN);
            return ASSIGN;
        case 42:
            addTk(EQUAL);
            return EQUAL;
        case 43:
            if( ch== '=')
            {
                state=45;
                pCrtCh++;
            }
            else
                state=44;
            break;
        case 44:
            addTk(LESS);
            return LESS;
        case 45:
            addTk(LESSEQ);
            return LESSEQ;
        case 46:
            if( ch == '=')
            {
                state=48;
                pCrtCh++;
            }
            else
                state=47;
            break;
        case 47:
            addTk(GREATER);
            return GREATER;
        case 48:
            addTk(GREATEREQ);
            return GREATEREQ;
        default:
            printf("Stare necunoscuta!\n");
            break;
        }

    }

}

void showAtoms()
{
    Token *p;
    char *a[]= {"ID", "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING", "BREAK",
                "CHAR", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN",
                "STRUCT", "VOID", "WHILE", "COMMA", "SEMICOLON", "LPAR",
                "RPAR","LBRACKET","RBRACKET", "LACC", "RACC", "END", "ADD", "SUB", "MUL", "DIV", "DOT", "AND",
                "OR", "NOT", "ASSIGN", "EQUAL","NOTEQ", "LESS",
                "LESSEQ", "GREATER", "GREATEREQ"
               };
    while(tokens!=NULL)
    {
        printf("%d\t %s",tokens->line, a[tokens->code]);
        if(tokens->code==ID || tokens->code==CT_STRING)
            printf(":%s",tokens->text);
        else if ( tokens->code==CT_INT )
            printf(":%d",tokens->i);
        else if(tokens->code == CT_CHAR)
            printf(":%c",tokens->i);
        else if (tokens->code==CT_REAL)
            printf(":%f",tokens->r);
        printf("\n");
        p=tokens;
        tokens=tokens->next;
        free(p);
    }

}

//ASIN

Token *iTk; // iteratorul în lista de atomi. Inițial pointează la primul atom din listă.
Token *consumedTk; // atomul care tocmai a fost consumat. Va fi folosit în etapele următoare ale compilatorului.

bool consume(int code)
{
    if(iTk->code==code)   // dacă la poziția curentă avem codul cerut, consumăm atomul
    {
        consumedTk=iTk;
        iTk=iTk->next;
        return true;
    }
    return false; // dacă la poziția curentă se află un atom cu un alt cod decât cel cerut, nu are loc nicio acțiune
}


const char *tkCodeName(int code)
{
    char *a[]= {"ID", "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING", "BREAK",
                "CHAR", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN",
                "STRUCT", "VOID", "WHILE", "COMMA", "SEMICOLON", "LPAR",
                "RPAR","LBRACKET","RBRACKET", "LACC", "RACC", "END", "ADD", "SUB", "MUL", "DIV", "DOT", "AND",
                "OR", "NOT", "ASSIGN", "EQUAL","NOTEQ", "LESS",
                "LESSEQ", "GREATER", "GREATEREQ"
               };
    return a[code];
}
/*
    // const char *tkCodeName(int code) - o funcție care primește ca parametru un cod de atom și îi returnează numele
bool consume(int code){
    printf("consume(%s)",tkCodeName(code));
    if(iTk->code==code){
        consumedTk=iTk;
        iTk=iTk->next;
        printf(" => consumed\n");
        return true;
    }
printf(" => found %s\n",tkCodeName(iTk->code));
return false;
}
*/
Symbol *owner=NULL;
//Lexical Syntax
bool unit();
bool structDef();
bool varDef();
bool typeBase(Type *t);
bool arrayDecl(Type *t);
bool fnDef();
bool fnParam();
bool stm();
bool stmCompound(bool newDomain);
bool expr();
bool exprAssign();
bool exprOR();
bool exprOrPrim();
bool exprAnd();
bool exprAndPrim();
bool exprEq();
bool exprEqPrim();
bool exprRel();
bool exprRelPrim();
bool exprAdd();
bool exprAddPrim();
bool exprMul();
bool exprMulPrim();
bool exprCast();
bool exprUnary();
bool exprPostfix();
bool exprPostfixPrim();
bool exprPrimary();
bool unit()
{
    Token *start=iTk;
    for(;;)
    {
        if(structDef()) {}
        else if(fnDef()) {}
        else if(varDef()) {}
        else
            break;
    }
    if(consume(END))
    {
        return true;
    }
    else
    {
        tkerr(iTk,"eroare de sintaxa");
    }
    iTk=start;
    return false;
}


bool structDef()
{
    Token *start=iTk;
    if(consume(STRUCT))
    {
        if(consume(ID))
        {
            Token *tkName=consumedTk;
            if(consume(LACC))
            {
                Symbol *s=findSymbolInDomain(symTable,tkName->text);
                if(s)
                    tkerr(iTk,"symbol redefinition: %s",tkName->text);
                s=addSymbolToDomain(symTable,newSymbol(tkName->text,SK_STRUCT));
                s->type.tb=TB_STRUCT;
                s->type.s=s;
                s->type.n=-1;
                pushDomain();
                owner = s;
                for(;;)
                {
                    if(varDef()) {}
                    else
                        break;
                }
                if(consume(RACC))
                {
                    if(consume(SEMICOLON))
                    {
                        owner=NULL;
                        dropDomain();
                        return true;
                    }
                    else
                    {
                        tkerr(iTk," Expecting ; after }");
                    }
                }
                else
                {
                    tkerr(iTk," Expecting } at the end of struct");
                }
            }
        }
        else
        {
            tkerr(iTk, " Missing a name for the struct");
        }
    }
    iTk=start;
    return false;
}


bool varDef()
{
    Token *start=iTk;
    Type t;
    if(typeBase(&t))
    {
        if(consume(ID))
        {
            Token *tkName=consumedTk;
            if(arrayDecl(&t))
            {
                if(t.n==0)
                    tkerr(iTk,"a vector variable must have a specified dimension");
            }
            if(consume(SEMICOLON))
            {
                Symbol *var=findSymbolInDomain(symTable,tkName->text);
                if(var)
                    tkerr(iTk,"symbol redefinition: %s",tkName->text);
                var=newSymbol(tkName->text,SK_VAR);
                var->type=t;
                var->owner=owner;
                addSymbolToDomain(symTable,var);
                if(owner)
                {
                    switch(owner->kind)
                    {
                    case SK_FN:
                        var->varIdx=symbolsLen(owner->fn.locals);
                        addSymbolToList(&owner->fn.locals,dupSymbol(var));
                        break;
                    case SK_STRUCT:
                        var->varIdx=typeSize(&owner->type);
                        addSymbolToList(&owner->structMembers,dupSymbol(var));
                        break;
                    }
                }
                else
                {
                    var->varIdx=allocInGlobalMemory(typeSize(&t));
                }

                return true;
            }
            else
            {
                tkerr(iTk," Expecting ; ");
            }
        }
        else
        {
            tkerr(iTk, " Expecting an identifier after a type");
        }
    }
    iTk=start;
    return false;
}

bool typeBase(Type *t)
{
    t->n = -1;
    Token *start=iTk;
    if(consume(INT))
    {
        t->tb=TB_INT;
        return true;
    }
    if(consume(DOUBLE))
    {
        t->tb=TB_DOUBLE;
        return true;
    }
    if(consume(CHAR))
    {
        t->tb=TB_CHAR;
        return true;
    }
    if(consume(STRUCT))
    {
        if(consume(ID))
        {
            Token *tkName=consumedTk;
            t->tb=TB_STRUCT;
            t->s=findSymbol(tkName->text);
            if(!t-> s)
                tkerr(iTk,"structura nedefinita: %s",tkName->text);
            return true;
        }
        else
        {
            tkerr(iTk," Expecting an identifier after struct");
        }
    }
    iTk=start;
    return false;
}

bool arrayDecl(Type *t)
{
    Token *start=iTk;
    if(consume(LBRACKET))
    {
        if(consume(CT_INT))
        {
            Token *tkSize=consumedTk;
            if(tkSize ->i >0)
                t->n = tkSize ->i;
            else
                t->n=0;

        }
        if(consume(RBRACKET))
        {
            return true;
        }
        else
        {
            tkerr(iTk, " expected a } at the end ");
        }
    }
    iTk=start;
    return false;
}

bool fnDef()
{
    Token *start=iTk;
    Type t;
    if(typeBase(&t)|| consume(VOID))
    {
        if(consumedTk->code == VOID)
        {
            t.tb=TB_VOID;
        }
        if(consume(ID))
        {
            Token *tkName=consumedTk;
            if(consume(LPAR))
            {
                Symbol *fn=findSymbolInDomain(symTable,tkName->text);
                if(fn)
                    tkerr(iTk,"symbol redefinition: %s",tkName->text);
                fn=newSymbol(tkName->text,SK_FN);
                fn->type=t;
                addSymbolToDomain(symTable,fn);
                owner=fn;
                pushDomain();
                if(fnParam())
                {
                    for(;;)
                    {
                        if(consume(COMMA))
                        {
                            if(fnParam()) {}
                            else
                            {
                                tkerr(iTk, " Expected a valid function parameter, the type or the name is missing, or the type is invalid  ");
                                iTk=start;
                                false;
                            }
                        }
                        else
                            break;
                    }
                }
                if(consume(RPAR))
                {
                    if(stmCompound(false))
                    {
                        dropDomain();
                        owner=NULL;
                        return true;
                    }
                    else
                    {
                        tkerr(iTk, " Aspected a valid expresion");
                    }
                }
                else
                {
                    tkerr(iTk, " Expected a ) ");
                }
            }
        }
        else
        {
            tkerr(iTk," Missing identifier or { after a struct");
        }
    }
    iTk=start;
    return false;

}

bool fnParam()
{
    Token *start=iTk;
    Type t;
    if(typeBase(&t))
    {
        if(consume(ID))
        {
            Token *tkName=consumedTk;
            if(arrayDecl(&t))
            {
                t.n=0;
            }
            Symbol *param=findSymbolInDomain(symTable,tkName->text);

            if(param)
                tkerr(iTk,"symbol redefinition: %s",tkName->text);

            param=newSymbol(tkName->text,SK_PARAM);
            param->type=t;
            param->paramIdx=symbolsLen(owner->fn.params);

            // parametrul este adaugat atat la domeniul curent, cat si la parametrii fn
            addSymbolToDomain(symTable,param);
            addSymbolToList(&owner->fn.params,dupSymbol(param));


            return true;
        }
        else
        {
            tkerr(iTk, " Expected a name after a type ");
        }
    }
    iTk=start;
    return false;
}

bool stm()
{
    Token *start=iTk;
    if(stmCompound(true))
    {
        return true;
    }
    iTk=start;
    if(consume(IF))
    {
        if(consume(LPAR))
        {
            if(expr())
            {
                if(consume(RPAR))
                {
                    if(stm())
                    {
                        if(consume(ELSE))
                        {
                            if(stm())
                            {
                                return true;
                            }
                            {
                                tkerr(iTk,"Missing else branch code block");
                            }
                            iTk=start;
                            return false;
                        }
                        return true;
                    }
                    else
                    {
                        tkerr(iTk,"Missing  if branch code block ");
                    }
                }
                else
                {
                    tkerr(iTk, " Expected   ) to close a if statement ");
                }
            }
            else
            {
                tkerr(iTk,"  Missing if condition " );
            }
        }
        else
        {
            tkerr(iTk, " Expected a ( after the if condition ");
        }
    }
    iTk=start;
    if(consume(WHILE))
    {
        if(consume(LPAR))
        {
            if(expr())
            {
                if(consume(RPAR))
                {
                    if(stm())
                    {
                        return true;
                    }
                    else
                    {
                        tkerr(iTk, " Missing while branch code block");
                    }
                }
                else
                {
                    tkerr(iTk, " Expected   ) to close a while statement ");
                }
            }
            else
            {
                tkerr(iTk,"  Missing  while condition " );
            }
        }
        else
        {
            tkerr(iTk, " Expected a ( after the while condition");
        }
    }
    iTk=start;
    if(consume(FOR))
    {
        if(consume(LPAR))
        {
            if(expr()) {}
            if(consume(SEMICOLON))
            {
                if(expr()) {}
                if(consume(SEMICOLON))
                {
                    if(expr()) {}
                    if(consume(RPAR))
                    {
                        if(stm())
                        {
                            return true;
                        }
                        else
                        {
                            tkerr(iTk,"Missing for branch code block");
                        }
                    }
                    else
                    {
                        tkerr(iTk," Missing )");
                    }
                }
                else
                {
                    tkerr(iTk," Missing ; ");
                }
            }
            else
            {
                tkerr(iTk," Missing ;");
            }
        }
        else
        {
            tkerr(iTk,"Missing (");
        }
    }
    iTk=start;
    if(consume(BREAK))
    {
        if(consume(SEMICOLON))
        {
            return true;
        }
        else
        {
            tkerr(iTk,"Missing ;");
        }
    }
    iTk=start;
    if(consume(RETURN))
    {
        if(expr()) {}
        if(consume(SEMICOLON))
        {
            return true;
        }
        else
        {
            tkerr(iTk,"Missing ;");
        }
    }
    iTk=start;
    if(expr()) {}
    if(consume(SEMICOLON))
    {
        return true;
    }
    iTk=start;
    return false;
}

bool stmCompound(bool newDomain)
{
    Token *start=iTk;
    if(consume(LACC))
    {
        if(newDomain)
            pushDomain();
        for(;;)
        {
            if(varDef()) {}
            else if(stm()) {}
            else
                break;
        }
        if(consume(RACC))
        {
            if(newDomain)
                dropDomain();
            return true;
        }
        else
        {
            tkerr(iTk,"Missing }");
        }
    }
    iTk=start;
    return false;
}

bool expr()
{
    Token *start=iTk;
    if(exprAssign())
    {
        return true;
    }
    iTk=start; //can be put as comments
    return false;
}

bool exprAssign()
{
    Token *start=iTk;
    if(exprUnary())
    {
        if(consume(ASSIGN))
        {
            if(exprAssign())
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Missing right expression term");
            }

        }
    }
    iTk=start;
    if(exprOR())
    {
        return true;
    }
    iTk=start;
    return false;
}

bool exprOR()
{
    Token *start=iTk;
    if(exprAnd())
    {
        if(exprOrPrim())
        {
            return true;
        }//else {tkerr(iTk,"Missing || ");}
    }
    iTk=start;
    return false;
}
bool exprOrPrim()
{
    Token *start=iTk;
    if(consume(OR))
    {
        if(exprAnd())
        {
            if(exprOrPrim())
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Missing ||  ");
            }
        }
        else
        {
            tkerr(iTk,"Missing  expression after || ");
        }
    }
    iTk=start;
    return true;
}

bool exprAnd()
{
    Token *start=iTk;
    if(exprEq())
    {
        if(exprAndPrim())
        {
            return true;
        }//else {tkerr(iTk,"Missing &&");}
    }
    iTk=start;
    return false;
}
bool exprAndPrim()
{
    Token *start=iTk;
    if(consume(AND))
    {
        if(exprEq())
        {
            if(exprAndPrim())
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Missing  &&");
            }
        }
        else
        {
            tkerr(iTk,"Missing expression after &&");
        }
    }
    iTk=start;
    return true;
}

bool exprEq()
{
    Token *start=iTk;
    if(exprRel())
    {
        if(exprEqPrim())
        {
            return true;
        }//else {tkerr(iTk,"Missing == or != operator");}
    }
    iTk=start;
    return false;
}

bool exprEqPrim()
{
    Token *start=iTk;
    if(consume(EQUAL)|| consume(NOTEQ))
    {
        if(exprRel())
        {
            if(exprEqPrim())
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Missing == or != operator");
            }
        }
        else
        {
            tkerr(iTk, "Missing expression after == or != ");
        }
    }
    iTk=start;
    return true;
}
bool exprRel()
{
    Token *start=iTk;
    if(exprAdd())
    {
        if(exprRelPrim())
        {
            return true;
        }//else {tkerr(iTk,"Missing < ,> ,<= or >= operator");}
    }
    iTk=start;
    return false;
}
bool exprRelPrim()
{
    Token *start=iTk;
    if(consume(LESS)||consume(LESSEQ)||consume(GREATER)||consume(GREATEREQ))
    {
        if(exprAdd())
        {
            if(exprRelPrim())
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Missing < ,> ,<= or >= operator");
            }
        }
        else
        {
            tkerr(iTk,"Missing expression after < ,> ,<= or >= operator");
        }
    }
    iTk=start;
    return true;
}
bool exprAdd()
{
    Token *start=iTk;
    if(exprMul())
    {
        if(exprAddPrim())
        {
            return true;
        }
    }
    iTk=start;
    return false;
}

bool exprAddPrim()
{
    Token *start=iTk;
    if(consume(ADD)||consume(SUB))
    {
        if(exprMul())
        {
            if(exprAddPrim())
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Missing + or - operator");
            }
        }
        else
        {
            tkerr(iTk,"Missing expression + or - operator");
        }
    }
    iTk=start;
    return true;
}

bool exprMul()
{
    Token *start=iTk;
    if(exprCast())
    {
        if(exprMulPrim())
        {
            return true;
        }
    }
    iTk=start;
    return false;
}
bool exprMulPrim()
{
    Token *start=iTk;
    if(consume(MUL)||consume(DIV))
    {
        if(exprCast())
        {
            if(exprMulPrim())
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Missing * or / operator");
            }
        }
        else
        {
            tkerr(iTk,"Missing  expression after * or / operator");
        }
    }
    iTk=start;
    return true;
}

bool exprCast()
{
    Token *start=iTk;
    if(consume(LPAR))
    {
        if(typeBase(consumedTk))
        {
            if(arrayDecl(consumedTk)) {}
            if(consume(RPAR))
            {
                if(exprCast())
                {
                    return true;
                }
            }
            else
            {
                tkerr(iTk,"Missing }");
            }
        }//else {tkerr(iTk,"Missing cast type base declaration");}
    }
    iTk=start;
    if(exprUnary())
    {
        return true;
    }
    iTk=start;
    return false;
}

bool exprUnary()
{
    Token *start=iTk;
    if(consume(SUB)||consume(NOT))
    {
        if(exprUnary())
        {
            return true;
        }
        else
        {
            tkerr(iTk,"Missing expression after  ! or  -");
        }
    }
    iTk=start;

    if(exprPostfix())
    {
        return true;
    }
    iTk=start;
    return false;
}

bool exprPostfix()
{
    Token *start=iTk;
    if(exprPrimary())
    {
        if(exprPostfixPrim())
        {
            return true;
        }
    }
    iTk=start;
    return false;
}
bool exprPostfixPrim()
{
    Token *start=iTk;
    if(consume(LBRACKET))
    {
        if(expr())
        {
            if(consume(RBRACKET))
            {
                if(exprPostfixPrim())
                {
                    return true;
                }
            }
            else
            {
                tkerr(iTk,"Missing ]");
            }
        }
        else
        {
            tkerr(iTk,"Missing expression after [");
        }
    }
    iTk=start;
    if(consume(DOT))
    {
        if(consume(ID))
        {
            if(exprPostfixPrim())
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk,"Missing identifier after .");
        }
    }
    iTk=start;
    return true;
}
bool exprPrimary()
{
    Token *start=iTk;
    if(consume(ID))
    {
        if(consume(LPAR))
        {
            if(expr())
            {
                for(;;)
                {
                    if(consume(COMMA))
                    {
                        if(expr()) {}
                        else
                        {
                            tkerr(iTk,"Missing  expression after ,");
                        }
                    }
                    else
                        break;
                }
            }
            if(consume(RPAR))
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Missing ) ");
                //iTk=start;
                //return false;
            }
        }
        return true;
    }
    iTk=start;
    if(consume(CT_INT))
    {
        return true;
    }
    if(consume(CT_REAL))
    {
        return true;
    }
    if(consume(CT_CHAR))
    {
        return true;
    }
    if(consume(CT_STRING))
    {
        return true;
    }
    if(consume(LPAR))
    {
        if(expr())
        {
            if(consume(RPAR))
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Missing ) ");
            }
        }
        else
        {
            tkerr(iTk,"Missing expression after (");
        }
    }
    iTk=start;
    return false;
}

int main()
{
    FILE *fis;
    pushDomain();
    fis=fopen("3c.txt","rb");
    if(fis ==NULL)
    {
        printf(" nu s-a putut deschide fisierul");
        return -1;
    }
    int n=fread(bufin,1,30000,fis);
    bufin[n]='\n';
    fclose(fis);

    pCrtCh=bufin;
    //printf("%c",*pCrtCh);

    while(getNextToken()!=END)
    {

    }
    // showAtoms();
    pushDomain();
    iTk=tokens;
    bool c3=unit();
    printf("%d \n",c3);
    showDomain(symTable,"global");
    // showAtoms();
    EliberareMemorie();
    dropDomain();
    return 0;
}
