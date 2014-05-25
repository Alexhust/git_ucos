/*
*********************************************************************************************************
*                                     MICRIUM BOARD SUPPORT SUPPORT
*
*                            (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      MICRIUM BOARD SUPPORT PACKAGE
*                                         SERIAL (UART) INTERFACE
*
*                                        Freescale Kinetis K60
*                                               on the
*
*                                        Freescale TWR-K60N512
*                                          Evaluation Board
*
* Filename      : bsp_ser.c
* Version       : V1.00
* Programmer(s) : EHS
*                 JPB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_SER_MODULE
#include <bsp_ser.h>
#include <k60_tower.h>
#include <uart.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  BSP_OS_SEM   BSP_SerTxWait;
static  BSP_OS_SEM   BSP_SerRxWait;
static  BSP_OS_SEM   BSP_SerLock;
static  CPU_INT08U   BSP_SerRxData;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void        BSP_Ser_WrByteUnlocked  (CPU_INT08U  c);
static  CPU_INT08U  BSP_Ser_RdByteUnlocked  (void);

extern int core_clk_khz;
extern int periph_clk_khz;
/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          BSP_Ser_Init()
*
* Description : Initialize a serial port for communication.
*
* Argument(s) : baud_rate           The desired RS232 baud rate.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_Init ()
{
	/* Enable the pins for the selected UART */
	 if (TERM_PORT == UART0_BASE_PTR)
	 {
		/* Enable the UART0_TXD function on PTD6 */
		PORTD_PCR6 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

		/* Enable the UART0_RXD function on PTD7 */
		PORTD_PCR7 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
	 }

	 if (TERM_PORT == UART1_BASE_PTR)
     {
	    /* Enable the UART1_TXD function on PTC4 */
	    PORTC_PCR4 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

	    /* Enable the UART1_RXD function on PTC3 */
	    PORTC_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
	  }

	  if (TERM_PORT == UART2_BASE_PTR)
	  {
		/* Enable the UART2_TXD function on PTD3 */
		  PORTD_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

		  /* Enable the UART2_RXD function on PTD2 */
		  PORTD_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
	  }

	  if (TERM_PORT == UART3_BASE_PTR)
	  {
		/* Enable the UART3_TXD function on PTC17 */
		  PORTC_PCR17 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

		  /* Enable the UART3_RXD function on PTC16 */
		  PORTC_PCR16 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
	  }
	  if (TERM_PORT == UART4_BASE_PTR)
	  {
				 /* Enable the UART3_TXD function on PTC17 */
		  PORTE_PCR24 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

		  /* Enable the UART3_RXD function on PTC16 */
		  PORTE_PCR25 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
	  }
	  if (TERM_PORT == UART5_BASE_PTR)
	  {
				 /* Enable the UART3_TXD function on PTC17 */
		  PORTE_PCR8 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

		  /* Enable the UART3_RXD function on PTC16 */
		  PORTE_PCR9 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
	  }
      /* UART0 and UART1 are clocked from the core clock, but all other UARTs are
		 clocked from the peripheral clock. So we have to determine which clock
		 to send to the uart_init function.*/
	  if ((TERM_PORT == UART0_BASE_PTR) | (TERM_PORT == UART1_BASE_PTR))
		  uart_init (TERM_PORT, core_clk_khz, TERMINAL_BAUD);
	  else
		  uart_init (TERM_PORT, periph_clk_khz, TERMINAL_BAUD);
}


/*
*********************************************************************************************************
*                                         BSP_Ser_ISR_Handler()
*
* Description : Serial ISR
*
* Argument(s) : none
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_ISR_Handler (void)
{
    CPU_INT08U  status;


    status = UART_S1_REG(UART3_BASE_PTR);

    if (DEF_BIT_IS_SET(status, UART_S1_RDRF_MASK)) {            /* Check the Receive Data Register Full bit.          */
        BSP_SerRxData = UART_D_REG(UART3_BASE_PTR);             /* Read one byte from the receive data register.      */
        BSP_OS_SemPost(&BSP_SerRxWait);                         /* Post to the sempahore                              */
    }

    if (DEF_BIT_IS_SET(status, UART_S1_TC_MASK)) {              /* Check the Transmission Complete bit.               */
        DEF_BIT_CLR(UART3_C2, UART_C2_TE_MASK);                 /* Clear and Set the TE bit of the C2 Register.       */
        DEF_BIT_SET(UART3_C2, UART_C2_TE_MASK);
        BSP_OS_SemPost(&BSP_SerTxWait);                         /* Post to the sempahore                              */
    }
}


/*
*********************************************************************************************************
*                                                BSP_Ser_Printf()
*
* Description : Formatted output to the serial port.
*               This funcion reads a string from a serial port. This call blocks until a
*               character appears at the port and the last character is a Carriage
*               Return (0x0D).
*
* Argument(s) : Format string follwing the C format convention.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_Printf (CPU_CHAR *format, ...)
{
    static  CPU_CHAR  buffer[70 + 1];
            va_list   vArgs;


    va_start(vArgs, format);
    vsprintf((char *)buffer, (char const *)format, vArgs);
    va_end(vArgs);

    BSP_Ser_WrStr((CPU_CHAR*) buffer);
}


/*
*********************************************************************************************************
*                                                BSP_Ser_RdByte()
*
* Description : Receive a single byte.
*
* Argument(s) : none.
*
* Return(s)   : The received byte
*
* Caller(s)   : Application
*
* Note(s)     : (1) This functions blocks until a data is received.
*
*               (2) It can not be called from an ISR.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Ser_RdByte (void)
{
    CPU_INT08U  rx_byte;


    BSP_OS_SemWait(&BSP_SerLock, 0);                            /* Obtain access to the serial interface              */

    rx_byte = BSP_Ser_RdByteUnlocked();

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */

    return (rx_byte);
}


/*
*********************************************************************************************************
*                                       BSP_Ser_RdByteUnlocked()
*
* Description : Receive a single byte.
*
* Argument(s) : none.
*
* Return(s)   : The received byte
*
* Caller(s)   : BSP_Ser_RdByte()
*               BSP_Ser_RdStr()
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Ser_RdByteUnlocked (void)
{
    CPU_INT08U   rx_byte;


    DEF_BIT_SET(UART3_C2, UART_C2_RIE_MASK);                    /* Enable the Receive Data Register Full interrupt.     */

    BSP_OS_SemWait(&BSP_SerRxWait, 0);                          /* Wait until a data is received                        */

    DEF_BIT_CLR(UART3_C2, UART_C2_RIE_MASK);                    /* Disable the Receive Data Register Full interrupt.    */

    rx_byte = BSP_SerRxData;                                    /* Read the data form the temporal register.            */

    return (rx_byte);
}


/*
*********************************************************************************************************
*                                                BSP_Ser_RdStr()
*
* Description : This function reads a string from a UART.
*
* Argument(s) : p_str      A pointer to a buffer at which the string can be stored.
*
*               len         The size of the string that will be read.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_RdStr (CPU_CHAR    *p_str,
                     CPU_INT16U   len)
{
    CPU_CHAR     rx_data;
    CPU_CHAR     rx_buf_ix;
    CPU_BOOLEAN  err;


    rx_buf_ix = 0;
    p_str[0]  = 0;

    err = BSP_OS_SemWait(&BSP_SerLock, 0);                      /* Obtain access to the serial interface              */

    if (err != DEF_OK ) {
        return;
    }

    while (DEF_TRUE)
    {
        rx_data = BSP_Ser_RdByteUnlocked();

        if ((rx_data == ASCII_CHAR_CARRIAGE_RETURN) ||          /* Is it '\r' or '\n' character  ?                    */
            (rx_data == ASCII_CHAR_LINE_FEED      )) {

            BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_LINE_FEED);
            BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_CARRIAGE_RETURN);

            p_str[rx_buf_ix] = 0;                              /* set the null character at the end of the string     */
            break;                                             /* exit the loop                                       */
        }

        if (rx_data == ASCII_CHAR_BACKSPACE) {                 /* Is backspace character                              */
            if (rx_buf_ix > 0) {
                BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_BACKSPACE);
                BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_SPACE);
                BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_BACKSPACE);

                rx_buf_ix--;                                   /* Decrement the index                                 */
                p_str[rx_buf_ix] = 0;
            }
        }

        if (ASCII_IsPrint(rx_data)) {                           /* Is it a printable character ... ?                  */
            BSP_Ser_WrByteUnlocked((CPU_INT08U)rx_data);        /* Echo-back                                          */
            p_str[rx_buf_ix] = rx_data;                         /* Save the received character in the buffer          */
            rx_buf_ix++;                                        /* Increment the buffer index                         */
            if (rx_buf_ix >= len) {
               rx_buf_ix = len;
            }
        }
    }

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface            */
}


/*
*********************************************************************************************************
*                                          BSP_Ser_WrByteUnlocked()
*
* Description : Writes a single byte to a serial port.
*
* Argument(s) : c    The character to output.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Ser_WrByte()
*               BSP_Ser_WrByteUnlocked()
*
* Note(s)     : (1) This function blocks until room is available in the UART for the byte to be sent.
*********************************************************************************************************
*/

void  BSP_Ser_WrByteUnlocked (CPU_INT08U c)
{
    DEF_BIT_SET(UART3_C2, UART_C2_TCIE_MASK);                   /* Enable the Transmission Complete interrupt.          */

    UART_D_REG(UART3_BASE_PTR) = c;                             /* Send a character.                                    */

    BSP_OS_SemWait(&BSP_SerTxWait, 0);

    BSP_OS_TimeDly(1);                                          /* Bug workaround.                                      */

    DEF_BIT_CLR(UART3_C2, UART_C2_TCIE_MASK);                   /* Disable the Transmission Complete interrupt.         */
}


/*
*********************************************************************************************************
*                                                BSP_Ser_WrByte()
*
* Description : Writes a single byte to a serial port.
*
* Argument(s) : tx_byte     The character to output.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_WrByte(CPU_INT08U  c)
{
    BSP_OS_SemWait(&BSP_SerLock, 0);                            /* Obtain access to the serial interface              */

    BSP_Ser_WrByteUnlocked(c);

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */
}


/*
*********************************************************************************************************
*                                                BSP_Ser_WrStr()
*
* Description : Transmits a string.
*
* Argument(s) : p_str      Pointer to the string that will be transmitted
*
* Caller(s)   : Application
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_WrStr (CPU_CHAR  *p_str)
{
    CPU_BOOLEAN  err;

    err = BSP_OS_SemWait(&BSP_SerLock, 0);                      /* Obtain access to the serial interface              */

    if (err != DEF_OK ) {
        return;
    }

    while ((*p_str) != (CPU_CHAR )0) {

        if (*p_str == ASCII_CHAR_LINE_FEED) {
            BSP_Ser_WrByteUnlocked(ASCII_CHAR_CARRIAGE_RETURN);
            BSP_Ser_WrByteUnlocked(ASCII_CHAR_LINE_FEED);
            p_str++;
        } else {
            BSP_Ser_WrByteUnlocked(*p_str++);
        }
    }

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */
}
