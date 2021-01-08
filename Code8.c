//Alexander Rollison
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MovieTheaterLib.h"
#include "QueueLib.h"
#include "BSTLib.h"
#include "ListLib.h"
#include "StackLib.h"

void PrintReceipts(SNODE **StackTop)
{
	char Ticket[4];
	if(*StackTop == NULL)
	{
		printf("\n\nNo Receipts\n\n");
	}
	else if(*StackTop != NULL)
	{
		printf("\n\nToday's Receipts\n\n");
		while(*StackTop!= NULL)
		{
			printf("\n\nReceipt #%d\n     %s\n", (*StackTop)->ReceiptNumber, (*StackTop)->MovieTheaterName);
			do
			{
				ReturnAndFreeLinkedListNode(&(*StackTop)->TicketList,Ticket);
				printf("\t%s", Ticket);
			}
			while((*StackTop)->TicketList!= NULL);
			pop(StackTop);
		}
	}
	else
	{
		printf("\n\nToday's receipts have already printed\n\n");
	}
}

BNODE *PickAndDisplayTheater(BNODE *BSTRoot, char MovieTheaterMap[][MAXCOLS], int *MapRow, int *MapCol)
{
	BNODE *MyTheater = NULL;
	char zip[6] = {};
	char *token;
	char Dims[6] = {};
	printf("\n\nPick a theater by entering the zipcode\n\n");////
	
	
	InOrder(BSTRoot);
	printf("\nEnter zipcode: ");
	scanf("%s",zip);
	getchar();
	MyTheater = SearchForBNODE(BSTRoot,zip);
	
	if(MyTheater == NULL)
	{
		printf("\n\nTheater not found\n\n");
	}
	else if(MyTheater != NULL)
	{
		strcpy(Dims,MyTheater->Dimensions);
		token = strtok(Dims, "x");
		*MapRow = atoi(token);
		token = strtok(NULL, "x");
		*MapCol = atoi(token);
		
		if(ReadMovieTheaterFile(MovieTheaterMap,MyTheater->FileName,*MapRow,*MapCol) == 0)
		{
			printf("\nUnable to print that seat map at this time.  Check back later.\n");
		}
		else
		{
			PrintSeatMap(MovieTheaterMap,*MapRow,*MapCol);
		}
	}
	
	return MyTheater;
}	

void ReadFileIntoQueue(FILE *QueueFile, QNODE **QH, QNODE **QT)
{
	char index[100];
	int count = 0;
	
	while(fgets(index, sizeof(index)-1,QueueFile))
	{
		if(index[strlen(index)-1] == '\n')
		{
			index[strlen(index)-1] = '\0';
		}
		enQueue(index, QH,QT);
		count++;
	}
}

void ReadFileIntoBST(FILE *BSTFile, BNODE **BSTnode)
{
	char index[100];
	char *token;
	int count = 0;
	char MTName[88];
	char Zipp[6];
	char Dim[6];
	char FN[88];
	
while(fgets(index, sizeof(index)-1,BSTFile))
	{
		if(index[0] == ' ')
		{
			continue;
		}
		if(index[strlen(index)-1] == '\n')
		{
			index[strlen(index)-1] = '\0';
		}
		
		token = strtok(index,"|");
		strcpy(MTName, token);
		
		token = strtok(NULL,"|");
		strcpy(Zipp, token);
		
		token = strtok(NULL,"|");
		strcpy(FN, token);
		
		token = strtok(NULL,"|");
		strcpy(Dim, token);
		
		AddBSTNode(BSTnode,MTName,Zipp,FN,Dim);
		count++;
	}
}

void get_command_line_parameter(char *argv[], char ParamName[], char ParamValue[])
{
	int i = 0; 
	
	while (argv[++i] != NULL)
	{
		if (!strncmp(argv[i], ParamName, strlen(ParamName)))
		{
			strcpy(ParamValue, strchr(argv[i], '=') + 1);
		}
	}
}

FILE* OpenFile(char filename[], char mode[])
{
	FILE* FH = NULL;
	FH = fopen(filename,mode);
	while(FH == NULL)
	{
		printf("\nA File you entered does not exist. Exiting...\n");
		exit(0);
	}
	return FH;
}

int PrintMenu()
{
	int choice = 0;
	
	printf("\n\n1.	Sell tickets to next customer\n\n");
	printf("2.	See who's in line\n\n");
	printf("3.	See the seat map for a given theater\n\n");
	printf("4.	Print today's receipts\n\n");
	printf("Choice : ");
	scanf("%d", &choice);
	
	while (choice < 1 || choice > 4)
	{
		printf("Invalid choice.  Please reenter. ");
		scanf("%d", &choice);
	}
	
	return choice;
}

int main(int argc, char *argv[])
{
	int i, j, k;
	FILE *queueFile = NULL;
	FILE *zipFile = NULL;
	char mode[2] = "r+";
	char arg_value[20];
	char queuefilename[20];
	char zipfilename[20];
	int ReceiptNumber = 0;
	int choice = 0;
	int SeatNumber;
	char Row;
	char Ticket[5];
	int ArrayRow, ArrayCol;
	int MapRow, MapCol;
	char MovieTheaterMap[MAXROWS][MAXCOLS] = {};
	LNODE *TicketLinkedListHead = NULL;
	QNODE *QueueHead = NULL;
	QNODE *QueueTail = NULL;
	BNODE *BSTRoot = NULL;
	BNODE *MyTheater = NULL;
	SNODE *StackTop = NULL;
	int NumberOfTickets = 0;
	
	/* Added Code
	MyTheater = malloc(sizeof(BNODE));
	MyTheater->MovieTheaterName = malloc(13);
	strcpy(MyTheater->MovieTheaterName, "Test Theater");
	strcpy(MyTheater->ZipCode, "76109");
	MyTheater->FileName = malloc(sizeof("file1.txt"));
	strcpy(MyTheater->FileName, "file1.txt");
	MapRow = 5;MapCol = 5;
	
	for (i = 0; i < 5; i++)
		for (j = 0; j < 5; j++)
			MovieTheaterMap[i][j] = 'O';
	
	*///End Added Code
	if (argc != 4)
	{
		printf("%s QUEUE=xxxxxx ZIPFILE=xxxxx RECEIPTNUMBER=xxxxx\n", argv[0]);
		exit(0);
	}
	
	get_command_line_parameter(argv, "QUEUE=", queuefilename);
	get_command_line_parameter(argv, "ZIPFILE=", zipfilename);
	get_command_line_parameter(argv, "RECEIPTNUMBER=", arg_value);
	ReceiptNumber = atoi(arg_value);
		
	queueFile = OpenFile(queuefilename, mode);
	zipFile = OpenFile(zipfilename,mode);
	
	ReadFileIntoQueue(queueFile, &QueueHead, &QueueTail);
/**/ReadFileIntoBST(zipFile, &BSTRoot);
	
	while (QueueHead != NULL)
	{
		choice = PrintMenu();
	
		switch (choice)
		{
			case 1 :
				printf("\n\nHello %s\n", QueueHead->name);	
				//// Added Code
//				PrintSeatMap(MovieTheaterMap, MapRow, MapCol);	
				////End Added Code			
				MyTheater = PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
				if (MyTheater != NULL)
				{
					printf("\n\nHow many movie tickets do you want to buy? ");
					scanf("%d", &NumberOfTickets);
					for (i = 0; i < NumberOfTickets; i++)
					{
						do
						{
							printf("\nPick a seat (Row Seat) ");
							scanf(" %c%d", &Row, &SeatNumber);
							Row = toupper(Row);
							ArrayRow = (int)Row - 65;
							ArrayCol = SeatNumber - 1;
						
							if ((ArrayRow < 0 || ArrayRow >= MapRow) ||
								(ArrayCol < 0 || ArrayCol >= MapCol))
							{
								printf("\nThat is not a valid seat.  Please choose again\n\n");
							}		
						}
						while ((ArrayRow < 0 || ArrayRow >= MapRow) ||
							   (ArrayCol < 0 || ArrayCol >= MapCol));
						
						if (MovieTheaterMap[ArrayRow][ArrayCol] == 'O')
						{	
							MovieTheaterMap[ArrayRow][ArrayCol] = 'X';
							sprintf(Ticket, "%c%d", Row, SeatNumber); 
							InsertNode(&TicketLinkedListHead, Ticket);
						}
						else
						{
							printf("\nSeat %c%d is not available.\n\n", Row, SeatNumber);
							i--;
						}
						PrintSeatMap(MovieTheaterMap, MapRow, MapCol);
					}
					
					WriteSeatMap(MyTheater, MovieTheaterMap, MapRow, MapCol);
					push(&StackTop, TicketLinkedListHead, ReceiptNumber, MyTheater->MovieTheaterName);
					TicketLinkedListHead = NULL;
					ReceiptNumber++;
					printf("\nThank you %s - enjoy your movie!\n", QueueHead->name);
					deQueue(&QueueHead);
				}
				break;
			case 2 : 
				printf("\n\nCustomer Queue\n\n");
				DisplayQueue(QueueHead);
				printf("\n\n");
				break;
			case 3 :
				PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
				break;
			case 4 : 
				PrintReceipts(&StackTop);
				break;
			default :
				printf("Bad menu choice");
		}
	}
	
	printf("Good job - you sold tickets to all of the customers in line.\n");
	PrintReceipts(&StackTop);
	
	printf("\n");
	return 0;
}
