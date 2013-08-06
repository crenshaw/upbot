/**
 * 
 * mkaddr.h
 *
 * This file contains the function prototype for mkaddr(), a function
 * used to create a socket address using inet_aton.  It is essentially
 * the demonstration code from:
 * 
 * http://www.ccplusplus.com/2011/09/mkaddr-c-code.html
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */
int mkaddr(void *addr,
	   int *addrlen,
	   char *str_addr,
	   char *protocol);
