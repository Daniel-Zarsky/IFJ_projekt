/**
    * Project: Implementace překladače imperativního jazyka IFJ22.
    *
    * @brief Implementation of functions providing lexical analysis.
    *
    * @author Daniel Žárský <xzarsk04@stud.fit.vutbr.cz>
    */



#include <ctype.h> // includes functions for determining type of loaded data
#include <stdio.h>
#include "buffer.h" // used for storing the identifier of token
#include "scanner.h" // includes prototypes and list of keywords, states and types of tokens and structure of token itself
#include "error.h"
#include <stdlib.h> // used for string processing


int process_float()
    {
        /**
         * Desetinný literál (rozsah C-double) také vyjadřuje nezáporná čísla v desítkové soustavě, přičemž literál je tvořen celou a desetinnou částí, nebo celou částí a exponentem, nebo celou a desetinnou částí a exponentem. Celá i desetinná část je tvořena
    neprázdnou posloupností číslic. Exponent je celočíselný, začíná znakem ’e’ nebo
    ’E’, následuje nepovinné znaménko ’+’ (plus) nebo ’-’ (mínus) a poslední částí je
    neprázdná posloupnost číslic. Mezi jednotlivými částmi nesmí být jiný znak, celou a
    desetinnou část odděluje znak ’.’ (tečka)5
    .
         */
    }
    int process_int()
    {
        /**
           • Celočíselný literál (rozsah C-int) je tvořen neprázdnou posloupností číslic a vyjadřuje
           hodnotu celého nezáporného čísla v desítkové soustavě.

       */
    }

    int process_string()
    {
        /**
       • Řetězcový literál je oboustranně ohraničen dvojitými uvozovkami (", ASCII hodnota
    34). Tvoří jej libovolný počet znaků zapsaných případně i na více řádcích. Možný
    je i prázdný řetězec (""). Znaky s ASCII hodnotou větší než 31 (mimo ") lze zapisovat přímo. Některé další znaky lze zapisovat pomocí escape sekvence: ’\"’, ’\n’,
    ’\t’, ’\\’. Jejich význam se shoduje s odpovídajícími znakovými konstantami jazyka PHP6
    . Na rozdíl od jazyka C nelze escape sekvencí vytvořit chybu – pakliže
    znaky za zpětným lomítkem neodpovídají žádnému z uvedených vzorů, jsou (včetně
    lomítka) bez jakýchkoli náhrad součástí řetězce. Expanzi (interpolaci) proměnných
    v řetězcích neuvažujte, nicméně znak dolar lze sázet jen pomocí escape sekvence
    ’\$’. Znak v řetězci může být zadán také pomocí escape sekvence ’\xdd’, kde dd je
    dvoumístné hexadecimální číslo od 01 do FF (písmena A-F mohou být malá i velká).
    Znak v řetězci může být zadán také pomocí obecné escape sekvence ’\ddd’, kde ddd
    je právě třímístné oktalové číslo od 001 do 377.
    Délka řetězce není omezena (resp. jen dostupnou velikostí haldy). Například řetězcový literál
         */
    }



    //main just for testing purposes in real implementation will be next token called from syntax analysis
    int main(){

            token token1;
            token *current_token;
            current_token = &token1;
            current_token->type = TYPE_EMPTY;
            current_token->attribute = NULL;
            int result= get_next_token(current_token);
            printf("Typ tokenu:\n");
            printf("%d\n", current_token->type);
            if(current_token->attribute != NULL){
                printf("ma atribut\n");
            }

            return result;
    }



    int get_next_token(token *token)
    {
          char c;
        int current = STATE_START;
          while(1){

              //load char from source file
              c = getc(stdin); //Překladač bude načítat řídicí program v jazyce IFJ22 ze standardního vstupu.

              // reduce states with direct transfer to other states(without condition)
              // only deterministic states
              // check unreachable states
              switch(current){
                  case (STATE_START):

                      if(c == '(' ){token->type = TYPE_PAR_LEFT; token->attribute = NULL; return TOKEN_OK; }

                      if(c == ')'){token->type = TYPE_PAR_RIGHT; token->attribute= NULL; return TOKEN_OK; }

                      if(c== '*'){token->type = TYPE_MUL; token->attribute= NULL; return TOKEN_OK; }

                      if(c == '+'){token->type = TYPE_PLUS; token->attribute= NULL; return TOKEN_OK; }

                      if(c == '-'){token->type = TYPE_MINUS; token->attribute= NULL; return TOKEN_OK; }

                      if(c == '.'){token->type = TYPE_CONCAT; token->attribute= NULL; return TOKEN_OK; }

                      if(c =='!'){current = STATE_EXCLAMATION;}

                      if(c == 92){token->type = TYPE_DIV; token->attribute= NULL; return TOKEN_OK; }// \ = 92 in ascii

                      if(c =='>'){current= STATE_GREATER;}

                      if(c =='<'){current= STATE_LOWER;}

                      break;
                  case(STATE_EXCLAMATION):
                      if(c == '='){current =  STATE_EXCLMATION_EQ;}
                      break;

                  case(STATE_EXCLMATION_EQ):
                      if(c=='='){token->type = TYPE_COMPARE_NEG; token->attribute= NULL; return TOKEN_OK;}
                      else return ERR_INTERNAL;
                      break;

                  case(STATE_LOWER):
                      if(c == '='){token->type = TYPE_LOWER_EQ; token->attribute= NULL; return TOKEN_OK;}
                      else {token->type = TYPE_LOWER; token->attribute= NULL; ungetc(c,stdin); return TOKEN_OK; }
                      break;

                  case(STATE_GREATER):
                      if(c == '='){token->type = TYPE_GREATER_EQ; token->attribute= NULL; return TOKEN_OK;}
                      else {token->type = TYPE_GREATER; token->attribute= NULL; ungetc(c,stdin); return TOKEN_OK; }

                   break;

                  default:
                      return ERR_INTERNAL;

              }


          }


    }