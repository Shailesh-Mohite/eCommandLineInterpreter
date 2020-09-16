/*
 * MIT License
 *
 * Copyright (c) 2019 Sean Farrelly
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * File        cli.c
 * Created by  Sean Farrelly
 * Version     1.0
 * Revamped by Shailesh Mohite
 * Version     2.0
 */
#ifndef _CLI_DEFS_H_
#define _CLI_DEFS_H_

#include <stddef.h>
#include <stdint.h>

/**
 * Serial library Configuration Options start
 *
*/
/* 1. SPECIFY THE TRANSMIT BUFFER SIZE
 * Serial Transmit buffer size, must be powers of 2 (2,4,8,16..)
 * This has performance impact during heavy usage of logging
 */
#define MAX_BUF_SIZE        128     /* Maximum size of CLI Rx buffer */

/* 2. SPECIFY THE DELIMITING CHARACTER
 */
#define CMD_TERMINATOR      '\r'    /* Delimitor denoting end of cmd */

/* 3. SPECIFY WHETHER TO ENABLE USER LOGIN TO ACCESS CLI
 */
#define CLI_USE_PASSWORD_EN (0)
/**
 * Serial library Configuration Options end
 *
*/



typedef enum
{
    CLI_OK,                 /* API execution successful.                */
    CLI_E_NULL_PTR,         /* Null pointer error.                      */
    CLI_E_CMD_NOT_FOUND,    /* Command name not found in command table. */
    CLI_E_INVALID_ARGS,     /* Invalid function parameters/arguments.   */
    CLI_E_BUF_FULL          /* CLI buffer full.                         */
} cli_status_t;

typedef enum
{
    CLI_INIT,
    CLI_READY,
    CLI_PROCESS,
    CLI_EXIT
} cli_state_t;

/*!
 * @brief Function type declarations.
 */
typedef cli_status_t (*cmd_func_ptr_t)(int argc, char **argv);
typedef void (*println_func_ptr_t)(char *string);

/*!
 * @brief Command structure, consisting of a name and function pointer.
 */ 
typedef struct
{
    uint8_t *cmd;        /* Command name.                            */
    cmd_func_ptr_t func; /* Function pointer to associated function. */
    uint8_t* man;     /* Command manual string                    */
} cmd_t;



/*!
 * @brief Command-line interface handle structure.
 */
typedef struct
{    
    cli_state_t cli_state;
    cmd_t *cmd_tbl;             /* Pointer to series of commands which are to be accepted. */
    size_t cmd_cnt;             /* Number of commands in cmd_tbl.                          */
} cli_t;


#endif
