/***********************************************************************
*
*  FILE        : SampleEMIF.c
*  DATE        : 2020-06-15
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include <stdio.h>
#include <string.h>
#include "serial/serial.h"
#include "cli/cli.h"

void main(void);

cli_status_t help_func(int argc, char **argv);

cmd_t cmd_tbl[] = {
    {.cmd = "help",.func = help_func,.man = "Help e.g help -cmdlist"},
#if CLI_USE_PASSWORD_EN == 1
    {.cmd = "login",.func = NULL,.man = "To Login follow e.g login"}, /* Built-in Functions */
#endif
    {.cmd = "cls",.func = NULL,.man = "Clear Screen e.g cls -10"},  /* Built-in Functions */
    {.cmd = "about",.func = NULL,.man = "Simple Lightweight Command Interpreter"} /* Built-in Functions ends*/
};

cli_t cli;
serial_t SerialComm1; ///< Serial library object instance for Comm1

void main(void)
{
    myComm1Init();///< user platform function to initialize serial comm port
    SerialComm1.sendchar = sendchar; ///< port function to write byte to uart tx register
    SerialComm1.enable_tx = enable_tx; ///< port function to enable/disable uart transmit
    SerialComm1.enable_rx = enable_rx; ///< port function to enable/disable uart receive
    serial_init (&SerialComm1);

    cli.cli_state = CLI_INIT;
    cli.cmd_tbl = cmd_tbl;
    cli.cmd_cnt = sizeof(cmd_tbl)/sizeof(cmd_t);
    cli_init (&cli);

    while (1)
    {
        // Check if input character available over UART

        if(0<serial_rxbuflen(&SerialComm1))
        {
            uint8_t c;
            c = getchar();
            cli_put(&cli, c);
            if (CLI_PROCESS == cli.cli_state)
            {
                cli_process (&cli);
            }
        }
    }
    return;
}


cli_status_t help_func(int argc, char **argv)
{
    cli_status_t Ret=CLI_E_INVALID_ARGS;
    switch(argc)
    {
        case 1:
            if(strcmp(argv[1], "-cmdlist") == 0)
            {
                uint32_t Temp;
                Temp = (uint32_t) (sizeof(cmd_tbl)/sizeof(cmd_t));
                printf( "Supported Commands list");
                for(uint32_t dwCount=0;dwCount<Temp;dwCount++)
                {
                    printf("\n%2d. ",dwCount+1);
                    printf(( char *) cmd_tbl[dwCount].cmd);
                    printf(" - ");
                    printf(( char *) cmd_tbl[dwCount].man);
                }
                Ret = CLI_OK;
            }
            break;
        default:
            break;
    }
    return Ret;
}

