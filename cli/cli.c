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
 * Note:- this CLI framework assumes stdio library integrated and working
 *        on platform
 * Features 1. Added NULL checks for carriage return ('\r') by user
 *          2. Added cls (clear screen) function as built-in api
 *          3. Added short .man pages like string to basic Command structure
 *          4. .man pages simulate per command help string to user
 *          5. Added User login feature enclosed by #define CLI_USE_PASSWORD_EN
 *          6. User login function is added as built-in api
 *          7. Cleaned up user_uart_println, instead using stdio
 *          8. Added about to the list of built-in apis, just the .man string
 */

/*! @file cli.c
 * @brief Implementation of command-line interface.
 */
#include "cli.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static uint8_t buf[MAX_BUF_SIZE];      /* CLI Rx byte-buffer */
static uint8_t *buf_ptr;               /* Pointer to Rx byte-buffer */

static uint8_t cmd_buf[MAX_BUF_SIZE];  /* CLI command buffer */

char cli_prompt[] = "\n>> ";       /* CLI prompt displayed to the user */
const char cli_unrecog[] = "CMD: Command not recognized";
const char cli_invarg[] = "Invalid arguments";

#if CLI_USE_PASSWORD_EN == 1
const char cli_user_unauthorized[] = "Access unauthorized, please login";
const char cli_user_logoff[] = "User successfully logged off";
const char cli_user_detail[] = "Enter user name and password";
const char cli_user_invalid[] = "Wrong user name or password";
const char cli_user_name[] = "Shailesh";
const char cli_user_pwd[] = "Shailesh";
const char cli_user_welcome[] = "Welcome ";
#endif

/*!
 * @brief This internal API prints a message to the user on the CLI.
 */
#if CLI_USE_PASSWORD_EN == 1
#define MAX_USER_NAME_PWD_LEN   0x10u
#define MAX_USER_NAME_LEN       MAX_USER_NAME_PWD_LEN
#define MAX_USER_PWD_LEN        MAX_USER_NAME_PWD_LEN

typedef enum
{
    LOGIN_NONE,
    LOGIN_USR,
    LOGIN_PWD,
    LOGIN_ERR,
    LOGIN_DONE
} sys_login_state_t;

sys_login_state_t loginState=LOGIN_NONE;

static cli_status_t sudo(int argc, char **argv);
#endif
static void cli_cls(int argc, char **argv);

/*!
 * @brief This API initializes the command-line interface.
 */
cli_status_t cli_init(cli_t *cli)
{
    /* Set buffer ptr to beginning of buf */
    buf_ptr = buf;

    /* Print the CLI prompt. */
    printf(cli_prompt);

    return CLI_OK;
}

/*!
 * @brief This API deinitialises the command-line interface.
 */
cli_status_t cli_deinit(cli_t *cli)
{
    return CLI_OK;
}

/*! @brief This API must be periodically called by the user to process and execute
 *         any commands received.
 */
cli_status_t cli_process(cli_t *cli)
{
    uint8_t argc = 0;
    char *argv[30];
    cli_status_t Ret=CLI_E_CMD_NOT_FOUND;

    /* Get the first token (cmd name) */
    argv[argc] = strtok ((char *) cmd_buf, " ");

    if (NULL != argv[argc])
    {
        /* Walk through the other tokens (parameters) */
        while ((argv[argc] != NULL) && (argc < 30))
        {
            argv[++argc] = strtok (NULL, " ");
        }
#if CLI_USE_PASSWORD_EN == 1
        if ((LOGIN_USR == loginState) || (LOGIN_PWD == loginState))
        {
            --argc;
            sudo (argc, argv);
            Ret = CLI_OK;
        }
        else
        {
#endif
            /* Search the command table for a matching command, using argv[0]
             * which is the command name. */
            for (size_t i = 0; i < cli->cmd_cnt; i++)
            {
                if (strcmp (argv[0], (const char *) cli->cmd_tbl[i].cmd) == 0)
                {
                    Ret = CLI_OK;
                    --argc;
#if CLI_USE_PASSWORD_EN == 1
                    switch (loginState)
                    {
                        case LOGIN_DONE:
#endif
                            if ((0 == (strcmp (argv[1], "?"))) || (0 == (strcmp (argv[1], "-?")))
                                    || (0 == strcmp ((const char *) cli->cmd_tbl[i].cmd, "about")))
                            { /* Check if cmd help is requested */
                                printf ((char *) cli->cmd_tbl[i].man);
                            }
                            else if (0 == strcmp ((const char *) cli->cmd_tbl[i].cmd, "cls"))
                            {/* Invoke clear screen api */
                                // TODO implementation pending
                                cli_cls (argc, argv);
                            }
#if CLI_USE_PASSWORD_EN == 1
                            else if (0 == strcmp ((const char *) cli->cmd_tbl[i].cmd, "login"))
                            //                        || (0 == strcmp ((const char *) cli->cmd_tbl[i].cmd, "logoff")))
                            {/* Already logged in, Invoke login screen api to logoff user */
                                sudo (argc, argv);
                            }
#endif
                            else if (NULL != cli->cmd_tbl[i].func)
                            {/* Found a match, execute the associated function. */
                                Ret = cli->cmd_tbl[i].func (argc, argv);
                                if(CLI_E_INVALID_ARGS == Ret)
                                {
                                    printf(cli_invarg);
                                }
                            }
#if CLI_USE_PASSWORD_EN == 1
                        break;
                        default:
                            if (0 == strcmp ((const char *) cli->cmd_tbl[i].cmd, "login"))
                            {/* Invoke login screen api */
                                sudo (argc, argv);
                            }
                            else
                            {
                                // message to logon
                                printf((char *)cli_user_unauthorized);
                            }

                        break;
                    }
#endif
                    break;
                }
            }
#if CLI_USE_PASSWORD_EN == 1
        }
#endif
    }
    else
    {
        cli->cli_state = CLI_READY;
        return CLI_E_NULL_PTR;
    }

    switch (Ret)
    {
        case CLI_E_CMD_NOT_FOUND:
            /* Command not found */
            printf (cli_unrecog);
        break;
        default:
        break;
    }
    /* Clear cmd_buf */
    memset (cmd_buf, 0, MAX_BUF_SIZE);
#if CLI_USE_PASSWORD_EN == 1
    if (LOGIN_PWD != loginState)
    {
#endif
        printf (cli_prompt);
#if CLI_USE_PASSWORD_EN == 1
    }
#endif
    cli->cli_state = CLI_READY;
    return Ret;
}

/*!
 * @brief This API should be called from the devices interrupt handler whenever a
 *        character is received over the input stream.
 */
cli_status_t cli_put(cli_t *cli, char c)
{
    switch (c)
    {
        case '\r':
        case '\n':
            *buf_ptr = '\0'; /* Terminate the msg and reset the msg ptr.      */
            strcpy ((char *) cmd_buf, (char *) buf); /* Copy string to command buffer for processing. */
            buf_ptr = buf; /* Reset buf_ptr to beginning.                   */
            printf (cli_prompt); /* Print the CLI prompt to the user.             */
            cli->cli_state = CLI_PROCESS;
        break;

        case '\b':
            /* Backspace. Delete character. */
            if (buf_ptr > buf)
            {
                buf_ptr--;
                putchar(c);
            }
        break;

        default:
            /* Normal character received, add to buffer. */
            if ((buf_ptr - buf) < MAX_BUF_SIZE)
            {
                *buf_ptr++ = c;
#if CLI_USE_PASSWORD_EN == 1
                if(LOGIN_PWD == loginState)
                {
                    c='*';
                }
#endif
                putchar(c);
            }
            else
            {
                return CLI_E_BUF_FULL;
            }
        break;
    }
    return CLI_OK;
}

/*!
 * @brief Print a message on the command-line interface.
 */
#define CLI_CLEAR_SCREEN "\033[2J\033[;H" // to be tested
#define L2CLEAR 20u
static void cli_cls(int argc, char **argv)
{
    /* No. of Lines to clear */
    uint32_t Lines2Clr=L2CLEAR;
    uint32_t len = 0;
    char String[16]={0};

    if(argc > 0)
    {
        len = strlen(argv[1]);
        /* limit to 999 so 3 + 2 characters */
        if(5 > len)
        {
            strncpy(String, argv[1],len);
            len = atoi((const char *)&String[1]);
            /* Limit to 40 lines */
            if(40 > len)
            {
                Lines2Clr=len;
            }
        }
    }

    for(uint32_t Lines=0;Lines<Lines2Clr;Lines++)
    {
        printf("\n");
    }
}

#if CLI_USE_PASSWORD_EN == 1
static cli_status_t sudo(int argc, char **argv)
{
    static char username[MAX_USER_NAME_LEN];
    static char userpwd[MAX_USER_PWD_LEN];
    uint32_t len;
    cli_status_t Ret=CLI_E_INVALID_ARGS;
    if(argc > 0)
    {
        // print error as parameters not allowed
        loginState=LOGIN_NONE;
    }
    else
    {
        switch (loginState)
        {
            case LOGIN_NONE:
                // print string to enter user name pwd
                printf((char *)cli_user_detail);
                loginState++;
            break;
            case LOGIN_USR:
                // save entered user name
                len = strlen(argv[0]);
                if(MAX_USER_NAME_LEN > len)
                {
                    len++;
                    strncpy(username, argv[0],len);
                }
                loginState++;
            break;
            case LOGIN_PWD:
                // validate entered user name and pwd
                len = strlen(argv[0]);
                if(MAX_USER_PWD_LEN > len)
                {
                    len++;
                    strncpy(userpwd, argv[0],len);

                    if((0 == strcmp ((const char *) cli_user_name, username)) &&
                            (0 == strcmp ((const char *) cli_user_pwd, userpwd)))
                    {
                        loginState=LOGIN_DONE;
                        printf((char *)cli_user_welcome);
                        printf((char *)cli_user_name);
                        printf((char *)"!!!");
                        cli_prompt[1] = '$';///< udpate the promt
                    }
                    else
                    {
                        loginState=LOGIN_NONE;
                        printf((char *)cli_user_invalid);
                    }
                }
                else
                {
                    // print error
                    loginState=LOGIN_NONE;
                    printf((char *)cli_user_invalid);
                }
            break;
            case LOGIN_DONE:
                // Logoff user
                loginState=LOGIN_NONE;
                printf((char *)cli_user_logoff);
                cli_prompt[1] = '>'; ///< restore the prompt
            break;
            default:
                // We should not be here
                // so knockoff user
                loginState=LOGIN_NONE;
            break;
        }
        Ret = CLI_OK;
    }
    return Ret;
}
#endif
