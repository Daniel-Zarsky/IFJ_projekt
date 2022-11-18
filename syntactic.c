//
// Created by samuel on 19.10.2022.
//


/**
    * Project: Implementace překladače imperativního jazyka IFJ22.
    *
    * @brief Implementation of syntactic analasys.
    *
    * @author Samuel Simun <xsimun04@stud.fit.vutbr.cz>
    */


#include "syntactic.h"
#include "scanner.c"
#include "symtable.c"
#include "scanner.h"
#include "error.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "grammatic_rules.c"

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
    free_table(to_delete->local_var);
    free_table(to_delete->function_var);
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
 * Function allocates data sources and sets to default values
 *
 * @return Syntactic_data_ptr
 */
Syntactic_data_ptr Init_data(){
    Syntactic_data_ptr data_ptr = malloc(sizeof(struct Syntactic_data));
    if (data_ptr == NULL){
        Program_Error(ERR_INTERNAL);
    }

    data_ptr->used_var = NULL;
    data_ptr->main_var = NULL:
    data_ptr->local_var = NULL;
    data_ptr->inside_condition = FALSE;
    data_ptr->inside_function = FALSE;
    data_ptr->inside_loop = FALSE;
    data_ptr->inside_program_closures = FALSE;

    return data_ptr;
}


/**
 * @brief Function get new token from file
 * Function get new token from file and return it for next processing
 *
 * @return token_struct token
 */
token_struct Get_token(){
    token_struct token;
    get_next_token(&token);
    return token;
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
    create_table(1543, data->local_var);
    data->used_var = data->local_var;

    /// Start of grammar check
    if (check_function_definition(data) != TRUE)
        return SYNTAX_ERR;


    /// Delete sources clean up
    free_table(data->local_var);
    data->used_var = data->main_var;
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
    /// Create local sym_table for condition
    create_table(1543, data->local_var);
    data->used_var = data->local_var;

    /// Start of grammar check
    if (check_condition(data) != SYNTAX_OK)
        return SYNTAX_ERR;

    /// Delete sources clean up
    free_table(data->local_var);
    data->used_var = data->main_var;
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
    /// Create local sym_table for while
    create_table(1543, data->local_var);
    data->used_var = data->local_var;

    /// Start of grammar check
    if (check_while(data) != SYNTAX_OK)
        return ERR_SYNTAX;

    /// Delete sources celan up
    free_table(data->local_var);
    data->used_var = data->main_var;
    return SYNTAX_OK;
}


/**
 * @brief Function handles start of command with keyword Int
 * Function handles token Function by calling function from gramatic_rules.c
 *
 * @see gramatic_rules.c
 * @param Syntactic_data_ptr
 * @return void
 * @TODO Write
 */
int Handle_int(Syntactic_data data){

}

/**
 * @brief Function handles start of command with keyword Float
 * Function handles token Function by calling function from gramatic_rules.c
 *
 * @see gramatic_rules.c
 * @param Syntactic_data_ptr
 * @return void
 * @TODO Write
 */
int Handle_float(Syntactic_data data){

}

/**
 * @brief Function handles start of command with keyword String
 * Function handles token Function by calling function from gramatic_rules.c
 *
 * @see gramatic_rules.c
 * @param Syntactic_data_ptr
 * @return void
 * @TODO Write
 */
int Handle_string(Syntactic_data data){

}


int parser(Syntactic_data_ptr data){

    token_struct token = Get_token();

    while(token.type != TYPE_PROLOG_END || token.type != TYPE_EOF) {
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
                if (Handle_int(data)){
                    Program_Error(data->error_status, data);
                }
                break;

            case (KEYWORD_STRING):
                if (Handle_string(data)){
                    Program_Error(data->error_status, data);
                }

            case (KEYWORD_FLOAT):
                if (Handle_float(data)){
                    Program_Error(data->error_status, data);
                }

            case (TYPE_INTEGER_Q):
                if (Handle_int(data)){
                    Program_Error(data->error_status, data);
                }

            case (TYPE_STRING_Q):
                if (Handle_string(data)){
                    Program_Error(data->error_status, data);
                }

            case (TYPE_FLOAT_Q):
                if (Handle_float(data)){
                    Program_Error(data->error_status, data);
                }

            default:
                Program_Error(ERR_SYNTAX, data);
        }

        token = Get_token();
    }

}


int main(){
    token_struct token = get_next_token();

    if (validate_program(token)){
        Program_Error(ERR_SYNTAX, data);;
    }

    Syntactic_data_ptr *data = Init_data();
    create_table(1543, data->main_var);
    create_table(1543, data->function_var);
    parser();

    return 0;
}


