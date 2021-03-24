#include "hostbug.h"
#include "AlphiBoard.h"
int barNbr;

#define MAX_PARAMS	10
int extractParam(char *szParm, int *argc, char *argv[], int maxparam)
{
	int nbrParam;

	nbrParam = 0;
	while (szParm && nbrParam<maxparam) {
		argv[nbrParam] = szParm;
		nbrParam++;
		szParm = strtok(NULL, " ");
	}
	*argc = nbrParam;
	return nbrParam;
}

char inBuffer = EOF;
char inBufferValid = 0;

#define myGetC getc
#define nonBlockingGets gets
#define blockingGetchar getche
AddrSpace *CurrentAs;

const char szHelpGetSoftwareVersion[] = "Usage:\n\n\tVERSION\n\n"
"Returns a version from the board software.\n";

void GetSoftwareVersion(
	char *szParm
)
{
/*	printf("Hardware ID 	   : 0x%08x\n", IORD_ALTERA_AVALON_SYSID_ID(SYSID_BASE));
	printf("Hardware timestamp : 0x%08x\n", IORD_ALTERA_AVALON_SYSID_TIMESTAMP(SYSID_BASE));
	printf("Software ID 	   : 0x%08x\n", SYSID_ID);
	printf("Software timestamp : 0x%08x\n", SYSID_TIMESTAMP);*/
}

const char szHelpWriteLoop[] = "Usage:\n\n\tWL [dwb] addr data [[dwb] addr data ...]\n\n"
"Repeatedly write in a loop where addr is the hex address to write and\n"
"data is the DWORD to be written.\n"
"'d' is 32 bit; 'w' is 16 bit, 'b' is 8 bit access.\n"
"Press any key to stop.\n";

void WriteLoop(
	char *szParm
)
{
	char *p = szParm;
	const int tableSize = 10;
	uint32_t Addr[tableSize];
	uint32_t Data[tableSize];
	char Mode[tableSize];
	int index = 0;
	int i;

	if (!szParm) {
		printf(szHelpWriteLoop);
		return;
		}

	for (i=0; i<tableSize; i++)
		Mode[i] = 'd';

	while (szParm && index < tableSize) {
		if (szParm[0] == 'd' || szParm[0] == 'w' || szParm[0] == 'b') {
			for (i=index; i<tableSize; i++)
				Mode[i] = szParm[0];

			szParm = strtok(NULL, " ");
			continue;
			}

		Addr[index] = strtoul(szParm, &p, 16);

		if ((Mode[i] == 'd' && Addr[index] >= CurrentAs->getLength() - 3) ||
				((Mode[i] == 'w') && (Addr[index] >= CurrentAs->getLength() - 1)) ||
				((Mode[i] == 'b') && (Addr[index] >= CurrentAs->getLength()))) {
			printf("This address space is 0x%08x bytes in length.", (unsigned int)CurrentAs->getLength());
			return;
			}

		szParm = strtok(NULL, " ");

		if (!szParm) {
			printf(szHelpWriteLoop);
			return;
			}

		Data[index] = strtoul(szParm, &p, 16);
		szParm = strtok(NULL, " ");
		index++;
		}

	printf("In a Write loop, write the following data:\n");

	for (i=0; i<index; i++) {
		switch (Mode[i]) {
			case 'd':
				printf("0x%08x:\t0x%08x\n", (unsigned int)Addr[i], (unsigned int)Data[i]);
				break;
			case 'w':
				printf("0x%08x:\t0x%04x\n", (unsigned int)Addr[i], (unsigned int)Data[i] & 0xffff);
				break;
			case 'b':
				printf("0x%08x:\t0x%02x\n", (unsigned int)Addr[i], (unsigned int)Data[i] & 0xff);
				break;
			}
		}
	printf("press <enter> to exit.");

	// If no char has been pressed, continue the read
    while(!_kbhit()) {
		for (i=0; i<index; i++) {
			switch (Mode[i]) {
				case 'd':
					CurrentAs->writeU32( Addr[i], (uint32_t)Data[i]);
					break;
				case 'w':
					CurrentAs->writeU16( Addr[i], (uint16_t)Data[i]);
					break;
				case 'b':
					CurrentAs->writeU8( Addr[i], (uint8_t)Data[i]);
					break;
				}
			}
		}

    blockingGetchar();
	putchar('\n');
}

const char szHelpReadLoop[] = "Usage:\n\n\tRL [dwb] addr [[dwb] addr ...]\n\n"
"Repeatedly read in a loop where addr is the hex address to read.\n"
"'d' is 32 bit; 'w' is 16 bit, 'b' is 8 bit access.\n"
"Press any key to stop.\n";

void ReadLoop(
	char *szParm
)
{
	char *p = szParm;
	const int tableSize = 10;
	uint32_t Addr[tableSize];
	char Mode[tableSize];
	volatile uint32_t temp;
	int index = 0;
	int i;

	for (i=0; i<tableSize; i++)
		Mode[i] = 'd';

	if (!szParm) {
		printf(szHelpReadLoop);
		return;
		}

	while (szParm && index < tableSize) {
		if (szParm[0] == 'd' || szParm[0] == 'w' || szParm[0] == 'b') {
			for (i=index; i<tableSize; i++)
				Mode[i] = szParm[0];

			szParm = strtok(NULL, " ");
			continue;
			}

		Addr[index] = strtoul(szParm, &p, 16);

		if ((Mode[i] == 'd' && Addr[index] >= CurrentAs->getLength() - 3) ||
				((Mode[i] == 'w') && (Addr[index] >= CurrentAs->getLength() - 1)) ||
				((Mode[i] == 'b') && (Addr[index] >= CurrentAs->getLength()))) {
			printf("This address space is 0x%08x bytes in length.", (unsigned int)CurrentAs->getLength());
			return;
			}

		szParm = strtok(NULL, " ");
		index++;
		}

	printf("In a Read loop, read the following addresses:\n");

	for (i=0; i<index; i++) {
		switch (Mode[i]) {
			case 'd':
				printf("0x%08x:(DWORD)\n", (unsigned int)Addr[i]);
				break;
			case 'w':
				printf("0x%08x:(WORD)\n", (unsigned int)Addr[i]);
				break;
			case 'b':
				printf("0x%08x:(BYTE)\n", (unsigned int)Addr[i]);
				break;
			}
		}
	printf("press <enter> to exit.");

	// If no char has been pressed, continue the read
    while(!_kbhit()) {
		for (i=0; i<index; i++)
			switch (Mode[i]) {
				case 'd':
    			    temp = CurrentAs->readU32( Addr[i] );
					break;
				case 'w':
					temp = CurrentAs->readU16( Addr[i] );
					break;
				case 'b':
	    			temp = CurrentAs->readU8( Addr[i] );
					break;
				}
			}

    blockingGetchar();
	putchar('\n');

}

const char szHelpMemoryModify[] = "Usage:\n\n\tMM addr\n\nwhere addr is the hex"
" address to display and modify."
"\n'\\n' or '+' modifies the entry and shows the next location.\n'-' modifies the entry"
" and shows the previous location.\n' ' modifies the location and displays it again.\n"
"'.' exits.\nEntries are in Hexadecimal. If no entry, then nothing is written.\n";

void MemoryModify(
	char *szParm
)
{
    char buff[20];
    char *p;
	char c;
    uint32_t Addr;
	uint32_t temp;
	char Mode = 'd';

	if (!szParm) {
		printf(szHelpMemoryModify);
		return;
		}

	if (szParm[0] == 'd' || szParm[0] == 'w' || szParm[0] == 'b') {
		Mode = szParm[0];

		szParm = strtok(NULL, " ");

		if (!szParm) {
			printf(szHelpMemoryModify);
			return;
			}
		}

	Addr = strtoul(szParm, &p, 16);

	if (Addr > CurrentAs->getLength()) {
		printf("This address space is 0x%08x bytes in length.", (unsigned int)CurrentAs->getLength());
		return;
		}

    printf("\n");

	while (1) {
next_location:
//		Addr = Addr % CurrentAs->getLength();

		switch (Mode) {
			case 'd':
   			    temp = CurrentAs->readU32( Addr );
		        printf("0x%08x : 0x%08x ", (unsigned int)Addr, (unsigned int)temp);
				break;
			case 'w':
				temp = CurrentAs->readU16( Addr );
		        printf("0x%08x : 0x%04x ", (unsigned int)Addr, (unsigned int)temp);
				break;
			case 'b':
    			temp = CurrentAs->readU8( Addr );
		        printf("0x%08x : 0x%02x ", (unsigned int)Addr, (unsigned int)temp);
				break;
			}

		p = buff;

		while (1) {
			switch (c = blockingGetchar()) {
				case '\n':
				case '+':
				case '-':
				case ' ':
					putchar('\n');

					if (p != buff) {
						*p = '\0';
						temp = strtoul(buff, &p, 16);
						switch (Mode) {
							case 'd':
								CurrentAs->writeU32( Addr, (uint32_t)temp);
								if (CurrentAs->readU32( Addr ) != temp)
									printf("Location does not read back correctly!\n");
								break;
							case 'w':
								CurrentAs->writeU16( Addr, (uint16_t)temp);
								if (CurrentAs->readU16( Addr ) != temp)
									printf("Location does not read back correctly!\n");
								break;
							case 'b':
								CurrentAs->writeU8( Addr, (uint8_t)temp);
								if (CurrentAs->readU8( Addr ) != temp)
									printf("Location does not read back correctly!\n");
								break;
							}
						}

					if (c == '-')
						switch (Mode) {
							case 'd':
								if (Addr <= 3)
									Addr = CurrentAs->getLength() - 4;
								else
									Addr-=4;
								break;
							case 'w':
								if (Addr <= 1)
									Addr = CurrentAs->getLength() - 2;
								else
									Addr-=2;
								break;
							case 'b':
								if (Addr == 0)
									Addr = CurrentAs->getLength() - 1;
								else
									Addr--;
								break;
							}
					else if (c == ' ')
						;
					else
						switch (Mode) {
							case 'd':
								Addr+=4;
								if (Addr >= CurrentAs->getLength())
									Addr = 0;
								break;
							case 'w':
								Addr+=2;
								if (Addr >= CurrentAs->getLength())
									Addr = 0;
								break;
							case 'b':
								Addr++;
								if (Addr >= CurrentAs->getLength())
									Addr = 0;
								break;
							}

					goto next_location;

				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9': case 'a': case 'b':
				case 'c': case 'd': case 'e': case 'f':
				case 'A': case 'B': case 'C': case 'D':
				case 'E': case 'F':
					*p++ = c;
					break;

				case '.':
					putchar('\n');
					return;
				case '\b':
					p--;
					break;
				case '\r':
					break;
				default:
					// beep and erase the errant character
					printf("\007\b \b");
					break;
				};
			}
		}
}

const char szHelpMemoryWrite[] = "Usage:\n\n\tMW [DWB] addr\n\nwhere addr is the hex"
" address to display and modify."
"\n'\\n' or '+' modifies the entry and shows the next location.\n'-' modifies the entry"
" and shows the previous location.\n' ' modifies the location and displays it again.\n"
"'.' exits.\nEntries are in Hexadecimal. If no entry, then nothing is written.\n";

void MemoryWrite(
	char *szParm
)
{
    char buff[20];
    char *p;
	char c;
    uint32_t Addr;
	uint32_t temp;
	char Mode = 'd';

	if (!szParm) {
		printf(szHelpMemoryWrite);
		return;
		}

	if (szParm[0] == 'd' || szParm[0] == 'w' || szParm[0] == 'b') {
		Mode = szParm[0];

		szParm = strtok(NULL, " ");

		if (!szParm) {
			printf(szHelpMemoryWrite);
			return;
			}
		}

	Addr = strtoul(szParm, &p, 16);

	if (Addr > CurrentAs->getLength()) {
		printf("This address space is 0x%08x bytes in length.", (unsigned int)CurrentAs->getLength());
		return;
		}

    printf("\n");

	while (1) {
next_location:
//		Addr = Addr % CurrentAs->getLength();

		switch (Mode) {
			case 'd':
		        printf("0x%08x : ", (unsigned int)Addr);
				break;
			case 'w':
		        printf("0x%08x : ", (unsigned int)Addr);
				break;
			case 'b':
		        printf("0x%08x : ", (unsigned int)Addr);
				break;
			}

		p = buff;

		while (1) {
			switch (c = blockingGetchar()) {
				case '\n':
				case '\r':
				case '+':
				case '-':
				case ' ':
					putchar('\n');

					if (p != buff) {
						*p = '\0';
						temp = strtoul(buff, &p, 16);
						switch (Mode) {
							case 'd':
								CurrentAs->writeU32( Addr, (uint32_t)temp);
								break;
							case 'w':
								CurrentAs->writeU16( Addr, (uint16_t)temp);
								break;
							case 'b':
			    			    CurrentAs->writeU8( Addr, (uint8_t)temp);
								break;
							}
						}

					if (c == '-')
						switch (Mode) {
							case 'd':
								if (Addr <= 3)
									Addr = CurrentAs->getLength() - 4;
								else
									Addr-=4;
								break;
							case 'w':
								if (Addr <= 1)
									Addr = CurrentAs->getLength() - 2;
								else
									Addr-=2;
								break;
							case 'b':
								if (Addr == 0)
									Addr = CurrentAs->getLength() - 1;
								else
									Addr--;
								break;
							}
					else if (c == ' ')
						;
					else
						switch (Mode) {
							case 'd':
								Addr+=4;
								if (Addr >= CurrentAs->getLength())
									Addr = 0;
								break;
							case 'w':
								Addr+=2;
								if (Addr >= CurrentAs->getLength())
									Addr = 0;
								break;
							case 'b':
								Addr++;
								if (Addr >= CurrentAs->getLength())
									Addr = 0;
								break;
							}

					goto next_location;

				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9': case 'a': case 'b':
				case 'c': case 'd': case 'e': case 'f':
				case 'A': case 'B': case 'C': case 'D':
				case 'E': case 'F':
					*p++ = c;
					break;

				case '.':
					putchar('\n');
					return;
				case '\b':
					p--;
					break;
				default:
					// beep and erase the erroneous character
					printf("\007\b \b");
					break;
				};
			}
		}
}

const char szHelpMemoryDisplay[] = "Usage:\n\n\tMD addr count\n\nwhere addr is "
"the hex address to display and count\n"
"is the number of DWORDS to display.\nXOFF and XON pause the display, ESC quits.\n";

void MemoryDisplay(
	char *szParm
)
{
	uint32_t Addr;
	long Count;

    int i, j;
    char buff[81], c, *p;
    uint32_t temp;
    uint32_t nibble;
	char Mode = 'd';

	if (!szParm) {
		printf(szHelpMemoryDisplay);
		return;
		}

	Addr = strtoul(szParm, &p, 16);

	if ((Mode == 'd' && Addr >= CurrentAs->getLength() - 3) ||
			(Mode == 'w' && Addr >= CurrentAs->getLength() - 1) ||
			(Mode == 'b' && Addr >= CurrentAs->getLength())) {
		printf("This address space is 0x%08x bytes in length.", (unsigned int)CurrentAs->getLength());
		return;
		}

	szParm = strtok(NULL, " ");

	if (!szParm) {
		printf(szHelpMemoryDisplay);
		return;
		}

	Count = strtol(szParm, &p, 16);

	if ((Mode == 'd' && Addr + Count*sizeof(uint32_t) > CurrentAs->getLength()) ||
			(Mode == 'w' && Addr + Count*sizeof(uint16_t) > CurrentAs->getLength()) ||
			(Mode == 'b' && Addr + Count*sizeof(uint8_t) > CurrentAs->getLength())) {
		printf("This address space is 0x%08x bytes in length.\n", (unsigned int)CurrentAs->getLength());
		return;
		}

    while( Count > 0 )
    {

        printf("0x%08x: ", (unsigned int)Addr );

        for ( i = 0, j = 0; i < 4; i++, Addr+=sizeof(uint32_t), Count--)
        {
			temp = CurrentAs->readU32( Addr );

            printf("%08x ", (unsigned int)temp );

            nibble = (temp & 0xFF000000) >> 24;
            if ( isprint(nibble) ) buff[j++] = (char)nibble;
            else                    buff[j++] = '.';

            nibble = (temp & 0x00FF0000) >> 16;
            if ( isprint(nibble) ) buff[j++] = (char)nibble;
            else                    buff[j++] = '.';

            nibble = (temp & 0x0000FF00) >> 8;
            if ( isprint(nibble) ) buff[j++] = (char)nibble;
            else                    buff[j++] = '.';

            nibble = (temp & 0x000000FF) >> 0;
            if ( isprint(nibble) ) buff[j++] = (char)nibble;
            else                    buff[j++] = '.';
        }

        buff[ j ] = '\0';

        printf("    %s", buff );

        printf("\n");

	    if (!kbhit())
            continue;

        c = blockingGetchar();    /* Yes, read it */

        if ( (c == 27) || (c == 3) )
            break;

        if ( c != 19 )    /* Ignore if it was NOT an XOFF <CTRL-S> */
            continue;

            /* Wait for <CTRL-Q> */
        do
        {
            c = blockingGetchar();
        } while( c != 17 );
    }
	putchar('\n');
}

const char szHelpMemoryFill[] = "Usage:\n\n\tMF [dwb] addr count value\n\nwhere 'addr' is "
"the hex address to display and 'count'\n"
"is the number of DWORDS to fill with 'value'. \n";

void MemoryFill(
	char *szParm
)
{
	uint32_t Addr, index;
	long Count, loop;

    char *p;
	clock_t start, finish;
	size_t transferLength;
	long errorCount;

	if (!szParm) {
		printf(szHelpMemoryDisplay);
		return;
		}

	Addr = strtoul(szParm, &p, 16);

	if (Addr >= CurrentAs->getLength() - 3 ) {
		printf("This address space is 0x%08x bytes in length.", (unsigned int)CurrentAs->getLength());
		return;
		}

	szParm = strtok(NULL, " ");

	if (!szParm) {
		printf(szHelpMemoryDisplay);
		return;
		}

	loop = strtol(szParm, &p, 16);

	transferLength = loop * sizeof(uint32_t);

	if (transferLength + Addr > CurrentAs->getLength()) {
		printf("This address space is 0x%08x bytes in length.\n", (unsigned int)CurrentAs->getLength());
		return;
	}

	while (1) {
		errorCount = 0;

		// write
		Count = loop;
		index = Addr;
		start = clock();
		while( Count > 0 )
		{
			CurrentAs->writeU32( index, index);
			index += 4;
			Count--;
		}
		finish = clock();
		if (finish > start)
			printf ("The memory fill took %d milliseconds, %d byte/second\n",
				(int)(finish - start), (int)((transferLength * 1000)/ (finish - start)));
		else printf ("The memory fill took less than 1 millisecond\n");
		if (kbhit()) {
			blockingGetchar();
			break;
		}

		// verification
		Count = loop;
		index = Addr;
		start = clock();
		while( Count > 0 )
		{
			if (CurrentAs->readU32( index ) != index) 
				errorCount++;
			index += 4;
			Count--;
		}
		finish = clock();
		if (finish > start)
			printf ("The memory verification took %d milliseconds, %d byte/second\n",
				(int)(finish - start), (int)((transferLength * 1000)/ (finish - start)));
		else printf ("The memory verification took less than 1 millisecond\n");
		if (errorCount) printf ("%d errors\n", (int)errorCount);
		else printf("no error!\n");
		if (kbhit()) {
			blockingGetchar();
			break;
		}
	}
}

#if 0
void ListAddressSpaces()
{
	printf("The following address spaces are available:\n\n");
	printf("space\tdescription\n");

	for (int i = 0; i < MAX_ADDRESS_SPACES; i++)
		if (brd.addressSpaces[i]->getLength()>0) {
			printf("%3d\t%s\n", i, brd.addressSpaces[i]->toString());
			}

	putchar('\n');
}

void AddressSpace(
	char *szParm
)
{
	ULONG newAS;
	char *p;

	if (!szParm) {
		printf(szHelpAddressSpace);
		ListAddressSpaces();
		return;
	}

	newAS = strtoul(szParm, &p, 10);
	if (newAS >= MAX_ADDRESS_SPACES || brd.addressSpaces[newAS]->getLength() == 0){
		printf(szHelpAddressSpace);
		ListAddressSpaces();
		return;
	}

	brd.CurrentAddressSpace = newAS;
	CurrentAs = brd.addressSpaces[newAS];
	printf("Address space #%d: %s\n",newAS, CurrentAs->toString());
}
#endif

char szHelpQuit[] = "Usage\n\n\tQUIT\n\nExit this utility program.\n";

void Quit(
	char *szParm
)
{
	exit(0);
}

const char szUnknownCommand[] = "Unknown command. Try '?' for help.\n";

void Help(
	char *szParm
);

typedef void (*CommandFunction_t)(char *);

typedef struct {
	const char *szCommand;
	CommandFunction_t Func;
	const char * szHelp;
	const char * szUsage;
} Command_t;

const char szHelpHelp[] = "Help for specific commands";
const char szHelpHelp2[] = "Usage:\n\n\tHELP cmd\n\nDetailed help for specific command cmd";

Command_t Command[] =
{
#ifdef AGGS_COMMANDS
	AGGS_COMMANDS
#endif
//	{"as",			AddressSpace, "Change BAR", szHelpAddressSpace},
	{"wl",			WriteLoop, "Repeatedly write to a series of addresses", szHelpWriteLoop},
	{"rl",			ReadLoop, "Repeatedly read a series of addresses", szHelpReadLoop},
	{"mm",			MemoryModify, "Memory modify", szHelpMemoryModify},
	{"mw",			MemoryWrite, "Memory write", szHelpMemoryWrite},
	{"md",			MemoryDisplay, "Memory Display", szHelpMemoryDisplay},
	{"mf", 			MemoryFill, "Memory Fill", szHelpMemoryFill},
	{"vers",		GetSoftwareVersion, "Get the board firmware version", szHelpGetSoftwareVersion},
//	{"info",		GetInfo, "Get the board register addresses", szHelpGetInfo},
//	{"config",		DisplayConfigSpace, "Get the board register addresses", szHelpDisplayConfigSpace},
	{"q",			Quit, "Exit this program", szHelpQuit},
	{"exit",		Quit, "Exit this program", szHelpQuit},
	{"quit",		Quit, "Exit this program", szHelpQuit},
	{"help",		Help, szHelpHelp, szHelpHelp2},
	{"?",			Help, szHelpHelp, szHelpHelp2},
};

void Help(
	char *szParm
)
{
	if (!szParm) {
		printf("The following commands are available.\nType ? cmd for specific help.\n\n");

		for (uint32_t i=0; i<table_size(Command); i++)
			printf("%s\t%s\n", Command[i].szCommand, Command[i].szHelp);

		return;
		}

	for (uint32_t i = 0; i < table_size(Command); i++) {
		if (!strcmp(szParm, Command[i].szCommand)) {
				printf("%s", Command[i].szUsage);
				return;
         		}
		}

	printf(szUnknownCommand);
}

void ProcessCommand(
	char *szCommand
)
{
	char *szTyped;
	int i;

	for (i=strlen(szCommand); i>=0; i--) {
		szCommand[i] = tolower(szCommand[i]);
		}

	szTyped = strtok(szCommand, " ");

 	if (szTyped[0] == '\0')
		return;

	for (uint32_t i = 0; i < table_size(Command); i++) {
		if (!strcmp(szTyped, Command[i].szCommand)) {
				Command[i].Func(strtok(NULL, " "));
				return;
         		}
		}

	printf(szUnknownCommand);
}

int hostbug_main(AddrSpace *sp)

{
	char szCommand[80];

	printf("Hostbug\n");

//	I2C_init();
//	getI2ctimeofday (&tod);
//	printf("%s\n", asctime(&tod));


//#ifdef ETH_CAP_0_BASE
//	printf("Phy #0 id: 0x%04x\n", (unsigned int)IORD_ALTERA_TSEMAC_MDIO(ETH_CAP_0_BASE, 0, 3));
//#endif
//	initEthernet();
	CurrentAs = sp;

	printf("%s>", CurrentAs->getName());

	while (1) {
//		while (nonBlockingGets(szCommand) == NULL);
		while (fgets(szCommand, sizeof(szCommand) , stdin) == NULL);

		if (szCommand[0] != '\0')
			ProcessCommand(szCommand);

		printf("%s>", CurrentAs->getName());
		}
}
