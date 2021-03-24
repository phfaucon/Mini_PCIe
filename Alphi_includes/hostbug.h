#ifndef _HOSTBUG_H
#define _HOSTBUG_H
//#include "AlphiBoard.h"
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include <time.h>
#include "AddrSpace.h"

#define MAX_ADDRESS_SPACES 16

#define MAX_BARS 6

#define table_size(a) (sizeof(a)/sizeof(a[0]))

int hostbug_main(AddrSpace *sp);

int OpenBoard();
int CloseBoard();

#endif
