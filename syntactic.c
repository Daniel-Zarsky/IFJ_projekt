//
// Created by samuel on 19.10.2022.
//


/**
    * Project: Implementace překladače imperativního jazyka IFJ22.
    *
    * @brief Implementation of syntactic analysand.
    *
    * @author Samuel Simun <xsimun04@stud.fit.vutbr.cz>
    */


#include "gramatic_rules.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>
#include "token_buffer.h"
#include "semantics.h"
//#include "generator.h"


#define FALSE 0
#define TRUE 1


/**
 * @brief Function for deallocating all sources
 * the function handles end of program - free all allocated memory
 *
 * @param Syntactic_data_ptr -  Data set which will be freed
 * @return void
 */
void Destroy_data(Syntactic_data_ptr to_delete) {

    if (to_delete == NULL) {
        return;
    }

    free_table(to_delete->main_var);
    free_ptable(to_delete->function_var);
    free_token_buffer(&to_delete->buffer);
    free(to_delete);
}


/**
 * @brief Function handles unexpected end of parser
 * the function handles program error -> calling Destroy data and exiting with
 * with error code
 *
 * @param int
 * @param Syntactic_data_ptr
 * @return void
 */
void Program_Error(int error, Syntactic_data_ptr data){
    Destroy_data(data);
    exit(error);
}


/**
 * @brief Function of initializing data sources
 * Function allocates data
 * sources and sets to default values
 *
 * @return Syntactic_data_ptr
 */
Syntactic_data_ptr Init_data(void){
    Syntactic_data_ptr data_ptr = malloc(sizeof(Syntactic_data));
    if (data_ptr == NULL){
        exit(ERR_INTERNAL);
    }

    if (init_token_buffer(&data_ptr->buffer))
        Program_Error(ERR_INTERNAL, data_ptr);

    if (create_ptable(1543, &(data_ptr->function_var)))
        Program_Error(ERR_INTERNAL, data_ptr);

    if (create_table(1543, &(data_ptr->main_var)))
        Program_Error(ERR_INTERNAL, data_ptr);

    data_ptr->inside_function = FALSE;
    data_ptr->used_var = NULL;
    data_ptr->local_var = NULL;

    return data_ptr;
}


/**
 * @brief Function get new token from file
 * Function get new token from file and return it for next processing
 *
 * @param Syntactic_data_ptr
 * @return token_struct token
 */
Token_struct Get_token(Syntactic_data_ptr data){
    Token_struct * p_token = init_token();
    if (p_token == NULL)
        Program_Error(ERR_INTERNAL, data);

    switch (get_next_token(p_token)) {
        case ERR_INTERNAL:
            Program_Error(ERR_INTERNAL, data);
            break;

        case ERR_LEX:
            Program_Error(ERR_LEX, data);
            break;

        case TOKEN_OK:
            break;

        default:
            Program_Error(ERR_INTERNAL, data);
    }

    Insert_to_buffer(p_token,data);
    return *p_token;
}

/**
 * @brief Function which add given token to buffer
 * Function handles token, push it into buffer, which is taken by generator
 *
 * @see buffer.c
 * @param token
 * @param Syntactic_data_ptr
 * @return void
 */
void Insert_to_buffer(Token_struct *token, Syntactic_data_ptr data){
    if (add_token_buffer(token, &data->buffer))
        Program_Error(ERR_INTERNAL, data);
}

/**
 * @brief Function handles start of program
 * Function handles start tokens and analyze them, set strict_types
 *
 * @param token start token
 * @param Syntactic_data_ptr
 * @return Error value (SYNTAX_OK or ERR_SYNTAX)
 */
int Validate_program(Token_struct token, Syntactic_data_ptr data){
    /// assert "<?php"
    if (token.type != TYPE_PROLOG_START)
        return ERR_SYNTAX;

    token = Get_token(data);

    /// assert "declare"
    if (token.type != TYPE_FUNCTION_ID || cmp_string_buffer("declare", token.buf))
        return ERR_SYNTAX;

    token = Get_token(data);


    /// assert "("
    if (token.type != TYPE_PAR_LEFT)
        return ERR_SYNTAX;

    token = Get_token(data);


    /// assert "strict_types"
    if (token.type != TYPE_FUNCTION_ID || cmp_string_buffer("strict_types", token.buf))
        return ERR_SYNTAX;

    token = Get_token(data);


    /// assert "="
    if (token.type != TYPE_ASSIGN)
        return ERR_SYNTAX;

    token = Get_token(data);


    /// assert "1-9"
    if (token.type != TYPE_INTEGER)
        return ERR_SYNTAX;

    /// assert "1"
    if (cmp_string_buffer( "1", token.buf))
        return ERR_SYNTAX;

    token = Get_token(data);


    /// assert ")"
    if (token.type != TYPE_PAR_RIGHT)
        return ERR_SYNTAX;

    token = Get_token(data);


    /// assert ";"
    if (token.type != TYPE_SEMICOLON)
        return ERR_SYNTAX;

    return SYNTAX_OK;

}

/**
 * @brief Function handles default
 * Function handles default functions which are added to function hash table
 *
 * @param Syntactic_data_ptr
 * @return Error_code
 */
void add_default_functions(Syntactic_data_ptr data){
    ///Write
    if (pinsert(&data->function_var, "write", "", KEYWORD_VOID, -1))
        Program_Error(ERR_INTERNAL, data);

    /// Read string
    if (pinsert(&data->function_var, "reads", "", KEYWORD_STRING_Q, -2))
        Program_Error(ERR_INTERNAL, data);

    /// Read integer
    if (pinsert(&data->function_var, "readi", "", KEYWORD_INT_Q, -2 ))
        Program_Error(ERR_INTERNAL, data);

    /// Read float
    if (pinsert(&data->function_var, "readf", "", KEYWORD_FLOAT_Q, -2))
        Program_Error(ERR_INTERNAL, data);

    /// Floatval
    if (pinsert(&data->function_var, "floatval", "", KEYWORD_FLOAT, -3))
        Program_Error(ERR_INTERNAL, data);

    /// Intval
    if (pinsert(&data->function_var, "intval", "", KEYWORD_INT, -3))
        Program_Error(ERR_INTERNAL, data);

    /// Strval
    if (pinsert(&data->function_var, "strval", "", KEYWORD_STRING, -3))
        Program_Error(ERR_INTERNAL, data);

    /// Strlen
    if (pinsert(&data->function_var, "strlen", "s", KEYWORD_INT, KEYWORD_STRING))
        Program_Error(ERR_INTERNAL, data);

    /// Substing
    if (pinsert(&data->function_var, "substring", "s", KEYWORD_STRING_Q, KEYWORD_STRING))
        Program_Error(ERR_INTERNAL, data);

    if (pinsert(&data->function_var, "substring", "i", KEYWORD_STRING_Q, KEYWORD_INT))
        Program_Error(ERR_INTERNAL, data);

    if (pinsert(&data->function_var, "substring", "j", KEYWORD_STRING_Q, KEYWORD_INT))
        Program_Error(ERR_INTERNAL, data);

    /// Ord
    if (pinsert(&data->function_var, "ord", "c", KEYWORD_INT, KEYWORD_STRING))
        Program_Error(ERR_INTERNAL, data);

    /// Chr
    if (pinsert(&data->function_var, "chr", "i", KEYWORD_STRING, KEYWORD_INT))
        Program_Error(ERR_INTERNAL, data);



}

/**
 * @brief Function handles start of command with keyword function
 * Function handles token Function by calling function from gramatic_rules.c
 *
 * @see gramatic_rules.c
 * @param Syntactic_data_ptr
 * @return void
 */
int Handle_function_dec(Syntactic_data_ptr data){
    /// Create local sym_table for function
    create_table(1543, &data->local_var);
    data->used_var = data->local_var;
    data->inside_function = TRUE;

    /// Start of grammar check
    if (check_function_definition(data) != TRUE)
        return ERR_SYNTAX;

    
    check_function_definition(data);
    if(data->error_status != 0){
        return data->error_status;
    }

    /// Delete sources clean up
    free_table(data->local_var);
    data->used_var = data->main_var;
    data->inside_function = FALSE;
    return SYNTAX_OK;

}


/**
 * @brief Function handles start of command with keyword if
 * Function handles token Function by calling function from gramatic_rules.c
 *
 * @see gramatic_rules.c
 * @param Syntactic_data_ptr
 * @return void
 */
int Handle_if(Syntactic_data_ptr data){

    data->used_var = data->main_var;

    /// Start of grammar check
    if (check_condition(data) != SYNTAX_OK)
        return ERR_SYNTAX;

    int i = 0;
    sem_check_if(&data, i, &i);
    if(data->error_status != 0){
        return data->error_status;
    }

    return SYNTAX_OK;
}


/**
 * @brief Function handles start of command with keyword while
 * Function handles token Function by calling function from gramatic_rules.c
 *
 * @see gramatic_rules.c
 * @param Syntactic_data_ptr
 * @return void
 */
int Handle_while(Syntactic_data_ptr data){

    data->used_var = data->main_var;

    /// Start of grammar check
    if (check_while(data) != SYNTAX_OK)
        return ERR_SYNTAX;

    int i = 0;
    sem_check_while(&data, i, &i);
    if(data->error_status != 0){
        return data->error_status;
    }

    return SYNTAX_OK;
}


/**
 * @brief Function handles start of command with keyword Int
 * Function handles token Function by calling function from gramatic_rules.c
 *
 * @see gramatic_rules.c
 * @param Syntactic_data_ptr
 * @return void
 */
int Handle_expression(Token_struct token, Syntactic_data_ptr data){

    token = Get_token(data);

    if (token.type == TYPE_ASSIGN) {
        check_after_equal(data);
    }
    else{
        if (check_expression(token, data, 0)) {
            data->error_status = ERR_SYNTAX;
            return ERR_SYNTAX;
        }
    }

    printf("IDEM K MART");

    int i = 0;
    if(sem_check_expression(&data, i, &i,0) == -1){    // TODO Incompatible pointer to integer conversion passing 'int *' to parameter of type 'int'; remove &
        return data->error_status;
    }
    return SYNTAX_OK;
}



/**
 * @brief Function handles start of calling function
 * Function handle calling function
 *
 * @param Syntactic_data_ptr
 * @return int Error status
 */
int Handle_function(Syntactic_data_ptr data){

    check_function_call(&data);
    if(data->error_status != 0){
        return data->error_status;
    }


    return SYNTAX_OK;
}




/**
 * @brief Main function for Syntactic / Semantic analyze
 *
 * @param Syntactic_data_ptr
 * @return int Error status
 */
int parser(Syntactic_data_ptr data){
    Token_struct token = Get_token(data);

    while(token.type != TYPE_PROLOG_END && token.type != TYPE_EOF) {
        switch (token.type) {
            case (KEYWORD_FUNCTION):

                if (Handle_function_dec(data)) {
                    Program_Error(data->error_status, data);
                }
                break;
            case (KEYWORD_IF):

                if (Handle_if(data)) {
                    Program_Error(data->error_status, data);
                }
                break;

            case (KEYWORD_WHILE):

                if (Handle_while(data)) {
                    Program_Error(data->error_status, data);
                }
                break;

            case (KEYWORD_INT):
                token = Get_token(data);

                if (Handle_expression(token, data))
                    Program_Error(data->error_status, data);

                break;

            case (KEYWORD_STRING):
                token = Get_token(data);

                if (Handle_expression(token, data))
                    Program_Error(data->error_status, data);

                break;

            case (KEYWORD_FLOAT):
                token = Get_token(data);

                if (Handle_expression(token, data))
                    Program_Error(data->error_status, data);

                break;

            case (KEYWORD_INT_Q):
                token = Get_token(data);

                if (Handle_expression(token, data))
                    Program_Error(data->error_status, data);

                break;

            case (KEYWORD_STRING_Q):
                token = Get_token(data);

                if (Handle_expression(token, data))
                    Program_Error(data->error_status, data);

                break;

            case (KEYWORD_FLOAT_Q):
                token = Get_token(data);

                if (Handle_expression(token, data))
                    Program_Error(data->error_status, data);

                break;

            case (TYPE_VARIABLE_ID):
                token = Get_token(data);

                if (Handle_expression(token, data)){
                    Program_Error(data->error_status, data);
                }
                break;

            case (TYPE_FUNCTION_ID):

                if (Handle_function(data)){
                    Program_Error(data->error_status, data);
                }
                break;

            case (TYPE_INTEGER):

                if (Handle_expression(token, data)){
                    Program_Error(data->error_status, data);
                }
                break;

            case (TYPE_FLOAT):

                if (Handle_expression(token, data)){
                    Program_Error(data->error_status, data);
                }
                break;

            case (TYPE_STRING):

                if (Handle_expression(token, data)){
                    Program_Error(data->error_status, data);
                }
                break;


            default:
                Program_Error(ERR_SYNTAX, data);
        }
        //generator(&data->buffer);
        token = Get_token(data);
    }

    return SYNTAX_OK;
}


int main(void){
    Syntactic_data_ptr data = Init_data();
    add_default_functions(data);

    Token_struct token = Get_token(data);

    if (Validate_program(token, data)){
        Program_Error(ERR_SYNTAX, data);
    }
    free_token_buffer(&data->buffer);
    init_token_buffer(&data->buffer);

    parser(data);
    Destroy_data(data);
    return 0;
}



