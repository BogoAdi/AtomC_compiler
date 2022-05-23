#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include "add.h"
#include "at.h"
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
    fprintf(stderr, "error in line %d: ",tk->line);
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
bool expr(Ret *r);
bool exprAssign(Ret *r);
bool exprOR(Ret *r);
bool exprOrPrim(Ret *r);
bool exprAnd(Ret *r);
bool exprAndPrim(Ret *r);
bool exprEq(Ret *r);
bool exprEqPrim(Ret *r);
bool exprRel(Ret *r);
bool exprRelPrim(Ret *r);
bool exprAdd(Ret *r);
bool exprAddPrim(Ret *r);
bool exprMul(Ret *r);
bool exprMulPrim(Ret *r);
bool exprCast(Ret *r);
bool exprUnary(Ret *r);
bool exprPostfix(Ret *r);
bool exprPostfixPrim(Ret *r );
bool exprPrimary(Ret *r);
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
                t->n = tkSize ->i;

        } else
            t->n=0;
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
    Ret rInit, rCond, rStep, rExpr;
    if(stmCompound(true))
    {
        return true;
    }
    iTk=start;
    if(consume(IF))
    {
        if(consume(LPAR))
        {
            if(expr(&rCond))
            {
                if(!canBeScalar(&rCond))
                    tkerr(iTk,"the if condition must be a scalar value");
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
            if(expr(&rCond))
            {
                if(!canBeScalar(&rCond))
                    tkerr(iTk,"the while condition must be a scalar value");

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
            if(expr(&rInit)) {}
            if(consume(SEMICOLON))
            {
                if(expr(&rCond))
                {
                    if(!canBeScalar(&rCond))
                        tkerr(iTk,"the for condition must be a scalar value");
                }
                if(consume(SEMICOLON))
                {
                    if(expr(&rStep)) {}
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
        if(expr(&rExpr))
        {
            if(owner->type.tb==TB_VOID)
                tkerr(iTk,"a void function cannot return a value");

            if(!canBeScalar(&rExpr))
                tkerr(iTk,"the return value must be a scalar value");

            if(!convTo(&rExpr.type,&owner->type))
                tkerr(iTk,"cannot convert the return expression type to the function return type");
        }else{

            if(owner->type.tb!=TB_VOID)
            tkerr(iTk,"a non-void function must return a value ");
        }

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
    if(expr(&rExpr)) {}
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

bool expr(Ret *r)
{
    Token *start=iTk;
    if(exprAssign(r))
    {
        return true;
    }
    iTk=start; //can be put as comments
    return false;
}

bool exprAssign(Ret *r)
{
    Token *start=iTk;
    Ret rDst;
    if(exprUnary(&rDst))
    {
        if(consume(ASSIGN))
        {
            if(exprAssign(r))
            {
                if(!rDst.lval)
                    tkerr(iTk,"the assign destination must be a left-value");

                if(rDst.ct)
                    tkerr(iTk,"the assign destination cannot be constant");

                if(!canBeScalar(&rDst))
                    tkerr(iTk,"the assign destination must be scalar");

                if(!canBeScalar(r))
                    tkerr(iTk,"the assign source must be scalar");

                if(!convTo(&r->type,&rDst.type))
                    tkerr(iTk,"the assign source cannot be converted to destination");

                r->lval=false;
                r->ct=true;

                return true;
            }
            else
            {
                tkerr(iTk,"Missing right expression term");
            }

        }
    }
    iTk=start;
    if(exprOR(r))
    {
        return true;
    }
    iTk=start;
    return false;
}

bool exprOR( Ret *r)
{
    Token *start=iTk;
    if(exprAnd(r))
    {
        if(exprOrPrim(r))
        {
            return true;
        }//else {tkerr(iTk,"Missing || ");}
    }
    iTk=start;
    return false;
}
bool exprOrPrim(Ret *r)
{
    Token *start=iTk;
    if(consume(OR))
    {
        Ret right;
        if(exprAnd(&right))
        {
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))
                tkerr(iTk,"invalid operand type for ||");
            *r=(Ret)
            { {TB_INT,NULL,-1},false,true
            };
            if(exprOrPrim(r))
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

bool exprAnd(Ret *r)
{
    Token *start=iTk;
    if(exprEq(r))
    {
        if(exprAndPrim(r))
        {
            return true;
        }//else {tkerr(iTk,"Missing &&");}
    }
    iTk=start;
    return false;
}
bool exprAndPrim(Ret *r)
{
    Token *start=iTk;
    if(consume(AND))
    {
        Ret right;
        if(exprEq(&right))
        {
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))
                tkerr(iTk,"invalid operand type for &&");
            *r=(Ret)
            { {TB_INT,NULL,-1},false,true
            };
            if(exprAndPrim(r))
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

bool exprEq(Ret *r)
{
    Token *start=iTk;
    if(exprRel(r))
    {
        if(exprEqPrim(r))
        {
            return true;
        }//else {tkerr(iTk,"Missing == or != operator");}
    }
    iTk=start;
    return false;
}

bool exprEqPrim(Ret *r)
{
    Token *start=iTk;
    if(consume(EQUAL)|| consume(NOTEQ))
    {
        Ret right;
        if(exprRel(&right))
        {
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))tkerr(iTk,"invalid operand type for == or!=");
                *r=(Ret)
                {{TB_INT,NULL,-1},false,true};
            if(exprEqPrim(r))
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
bool exprRel(Ret *r)
{
    Token *start=iTk;
    if(exprAdd(r))
    {
        if(exprRelPrim(r))
        {
            return true;
        }//else {tkerr(iTk,"Missing < ,> ,<= or >= operator");}
    }
    iTk=start;
    return false;
}
bool exprRelPrim(Ret *r)
{
    Token *start=iTk;
    if(consume(LESS)||consume(LESSEQ)||consume(GREATER)||consume(GREATEREQ))
    {   Ret right;
        if(exprAdd(&right))
        {
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))tkerr(iTk,"invalid operand type for <,<=, >, >=");
            *r=(Ret)
            {{TB_INT,NULL,-1},false,true};
            if(exprRelPrim(r))
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
bool exprAdd(Ret *r)
{
    Token *start=iTk;
    if(exprMul(r))
    {
        if(exprAddPrim(r))
        {
            return true;
        }
    }
    iTk=start;
    return false;
}

bool exprAddPrim(Ret *r)
{
    Token *start=iTk;
    if(consume(ADD)||consume(SUB))
    {
        Ret right;
        if(exprMul(&right))
        {
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))
                tkerr(iTk,"invalid operand type for + or-");

            *r=(Ret){tDst,false,true};

            if(exprAddPrim(r))
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

bool exprMul(Ret *r)
{
    Token *start=iTk;
    if(exprCast(r))
    {
        if(exprMulPrim(r))
        {
            return true;
        }
    }
    iTk=start;
    return false;
}
bool exprMulPrim(Ret *r)
{
    Token *start=iTk;
    if(consume(MUL)||consume(DIV))
    {
        Ret right;
        if(exprCast(&right))
        {   Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))
                tkerr(iTk,"invalid operand type for * or /");
            *r=(Ret){tDst,false,true};
            if(exprMulPrim(r))
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

bool exprCast(Ret *r)
{
    Token *start=iTk;
    if(consume(LPAR))
    {   Type t;
        Ret op;
        if(typeBase(&t))
        {
            if(arrayDecl(&t)) {}
            if(consume(RPAR))
            {
                if(exprCast(&op))
                {
                    if(t.tb==TB_STRUCT)
                        tkerr(iTk,"cannot convert to a struct type");
                    if(op.type.tb==TB_STRUCT)
                        tkerr(iTk,"cannot convert a struct");
                    if(op.type.n>=0&&t.n<0)
                        tkerr(iTk,"an array can be converted only to another array");
                    if(op.type.n<0&&t.n>=0)
                        tkerr(iTk,"a scalar can be converted only to another scalar");
                    *r=(Ret){t,false,true};
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
    if(exprUnary(r))
    {
        return true;
    }
    iTk=start;
    return false;
}

bool exprUnary(Ret *r)
{
    Token *start=iTk;
    if(consume(SUB)||consume(NOT))
    {
        if(exprUnary(r))
        {
            if(!canBeScalar(r))
                tkerr(iTk,"unary - must have a scalar operand");
            r->lval=false;
            r->ct=true;
            return true;
        }
        else
        {
            tkerr(iTk,"Missing expression after  ! or  -");
        }
    }
    iTk=start;

    if(exprPostfix(r))
    {
        return true;
    }
    iTk=start;
    return false;
}

bool exprPostfix(Ret *r)
{
    Token *start=iTk;
    if(exprPrimary(r))
    {
        if(exprPostfixPrim(r))
        {
            return true;
        }
    }
    iTk=start;
    return false;
}
bool exprPostfixPrim(Ret *r)
{
    Token *start=iTk;
    if(consume(LBRACKET))
    {
        Ret idx;
        if(expr(&idx))
        {
            if(consume(RBRACKET))
            {
                if(r->type.n<0)
                    tkerr(iTk,"only an array can be indexed");
                Type tInt={TB_INT,NULL,-1};

                if(!convTo(&idx.type,&tInt))
                    tkerr(iTk,"the index is not convertible to int");
                r->type.n=-1;
                r->lval=true;
                r->ct=false;
                if(exprPostfixPrim(r))
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
            Token *tkName=consumedTk;
            if(r->type.tb!=TB_STRUCT)
                tkerr(iTk,"a field can only be selected from a struct");
            Symbol *s=findSymbolInList(r->type.s->structMembers,tkName->text);
            if(!s)
                tkerr(iTk,"the structure %s does not have a field %s",r->type.s->name,tkName->text);
            *r=(Ret){s->type,true,s->type.n>=0};
            if(exprPostfixPrim(r))
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
bool exprPrimary( Ret *r)
{
    Token *start=iTk;
    if(consume(ID))
    {   ////???
        Token *tkName=consumedTk;

        Symbol *s=findSymbol(tkName->text);
        if(!s)
            tkerr(iTk,"undefined id: %s",tkName->text);

        if(consume(LPAR))
        {
            if(s->kind!=SK_FN)
                tkerr(iTk,"only a function can be called");
            Ret rArg;
            Symbol *param=s->fn.params;

            if(expr(&rArg))
            {
                if(!param)
                    tkerr(iTk,"too many arguments in function call");
                if(!convTo(&rArg.type,&param->type))
                    tkerr(iTk,"in call, cannot convert the argument type to the parameter type");
                param=param->next;

                for(;;)
                {
                    if(consume(COMMA))
                    {
                        if(expr(&rArg))
                            {
                                if(!param)
                                    tkerr(iTk,"too many arguments in function call");
                                if(!convTo(&rArg.type,&param->type))
                                    tkerr(iTk,"in call, cannot convert the argument type to the parameter type");
                                param=param->next;
                            }
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
                if(param)
                    tkerr(iTk,"too few arguments in function call");
                *r=(Ret){s->type,false,true};
                return true;
            }
            else
            {
                tkerr(iTk,"Missing ) ");
                //iTk=start;
                //return false;
            }
        }
        //????
        if(s->kind==SK_FN)
            tkerr(iTk,"a function can only be called");
        *r=(Ret){s->type,true,s->type.n>=0};
        return true;
    }
    iTk=start;
    if(consume(CT_INT))
    {
        *r=(Ret){{TB_INT,NULL,-1},false,true};
        return true;
    }
    if(consume(CT_REAL))
    {
        *r=(Ret){{TB_DOUBLE,NULL,-1},false,true};
        return true;
    }
    if(consume(CT_CHAR))
    {
        *r=(Ret){{TB_CHAR,NULL,-1},false,true};
        return true;
    }
    if(consume(CT_STRING))
    {
        *r=(Ret){{TB_CHAR,NULL,0},false,true};
        return true;
    }
    if(consume(LPAR))
    {
        if(expr(r))
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
