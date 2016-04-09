/**********************************************************************
Program cs2123p4.c by Timothy Hennessy
Purpose:	
    Program parses a file and uses a linked list to process
    the data into arrival events to be used with a user
    selected simulation.
Command Parameters:
    program name    switch option   switch option
    p4              -v              -a[A or B or C]
    Examples: 
    p4 -v -aA       // run simulation A in verbose mode
    p4 -aB          // run simulation B without verbose
Input:
    This program uses a file to parse arrival events into linked list.
Results:
   Time Widget Event
      0     1  Arrived
      0     1  Enter Queue 1
      0     1  Leave Queue 1, waited 0
      0     1  Seized Server 1
     13     2  Arrived
     13     2  Enter Queue 1
     15     1  Released Server 1
     15     2  Leave Queue 1, waited 2
     15     2  Seized Server 1
     15     1  Enter Queue 2
     15     1  Leave Queue 2, waited 0 
Returns:
    Refer to cs2123p4.h for a full listing
    of various return values.
Notes:
    1.  User is responsible for specifiying switch options
        for desired simulation and verbose mode.
    2.  The program implements a singly linked list.
    3.  The program implements at least one queue.
**********************************************************************/
/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cs2123p4.h"

FILE *pAlternativeFileName;         // will be used to store file names for alternatives

/******************** newServer *******************************************
Server newServer(char szServerNm[])
Purpose:
    Dynamically allocates space for a structure of type ServerImp to
    be used with simulations.
Parameters:
    I   char szServerNm[]  a string to be used for Server name
Returns:
    Server - a pointer to a dynamically allocated structure of type
             ServerImp
Notes:
    If dynamic memory allocation fails, program aborts.
**************************************************************************/
Server newServer(char szServerNm[])
{
	Server serverTeller = (Server)malloc(sizeof(ServerImp));
	if (serverTeller == NULL)
		ErrExit(ERR_ALGORITHM, "No available memory for server");
	strcpy(serverTeller->szServerName, szServerNm);
	serverTeller->bBusy = FALSE;
	return serverTeller;
}
// queue functions
/******************** removeQ *******************************************
int removeQ(Queue queue, QElement *pFromQElement)
Purpose:
    Removes a NodeQ from the queue.  Since, a queue is a first-in-first-
    out data structure, the first NodeQ is removed.
Parameters:
    I   Queue queue                 pointer to a QueueImp structure
    I   QElement *pFromQElement     pointer to a QElement structure
Returns:
    Functionally:
       TRUE  - queue is not empty
       FALSE - queue is empty
    Via param:
       Through a pointer to a QElement structure the function returns
       the removed NodeQ's QElement structure.
Notes:
    1. Ensures the queue is not empty otherwise return FALSE.
    2. Remove first NodeQ in queue then set queue to point to
       the next NodeQ.
    3. Free memory for removed NodeQ.
**************************************************************************/
int removeQ(Queue queue, QElement *pFromQElement)
{
	NodeQ *pRemove;
	// check for empty
	if (queue->pHead == NULL)
		return FALSE;                   // FALSE = no element returned
	                                    // i.e., queue is already empty
	pRemove = queue->pHead;             // remove first node
	*pFromQElement = pRemove->element;  // element returned via param
	queue->pHead = pRemove->pNext;      // set pHead to next node
	// Removing the node could make the list empty.
	// See if we need to update pFoot, due to empty list
	if (queue->pHead == NULL)
		queue->pFoot = NULL;
	free(pRemove);
	return TRUE;
}
/******************** insertQ *******************************************
void insertQ(Queue queue, QElement element)
Purpose:
    Inserts a new NodeQ into the queue using data provided via param
    "element" to populate QElement's member structure.
Parameters:
    I   Queue queue          pointer to a QueueImp structure
    I   QElement element     QElement structure
Returns:
    N/A
Notes:
    1. Calls allocNodeQ to dynamically allocate memory for new NodeQ.
    2. If queue is empty the queue pointers to head and foot are set to
       point to the new NodeQ.
    3. If queue is not empty the new NodeQ is added to the end of the queue.
**************************************************************************/
void insertQ(Queue queue, QElement element)
{
	NodeQ *pNew;
	pNew = allocNodeQ(queue, element);
	// check for empty
	if (queue->pFoot == NULL)
	{
		queue->pFoot = pNew;
		queue->pHead = pNew;
	}	
	else
	{ // insert after foot
		queue->pFoot->pNext = pNew;
		queue->pFoot = pNew;
	}
}
/******************** allocNodeQ *******************************************
NodeQ *allocNodeQ(Queue queue, QElement value)
Purpose:
    Dynamically allocates memory for a new NodeQ structure and uses the 
    QElement value passed in (value) to populate the new NodeQ's QElement
    structure.
Parameters:
    I   Queue queue                 pointer to a QueueImp structure
    I   QElement value              QElement structure
Returns:
    Functionally:
       NodeQ * - a pointer pNew which is the memory 
       address to where the dynamically allocated NodeQ structure exists.
Notes:
    1. If dynamic memory allocation fails program aborts.
**************************************************************************/
NodeQ *allocNodeQ(Queue queue, QElement value)
{
	NodeQ *pNew;
	pNew = (NodeQ *)malloc(sizeof(NodeQ));
	if (pNew == NULL)
		ErrExit(ERR_ALGORITHM, "No available memory for queue");
	pNew->element = value;
	pNew->pNext = NULL;
	return pNew;
}
/******************** newQueue *******************************************
Queue newQueue(char szQueueNm[])
Purpose:
    Dynamically allocates memory and initializes a new queue.  
Parameters:
    I   char szQueueNM[]    name of queue being initialized
Returns:
    Functionally:
       q - which is a pointer to a QueueImp structure
Notes:
    1. Dynamically allocates memory for a new queue
    2. Sets pHead & pFoot to NULL
    3. New queue is assigned the name stored in the array szQueueNm
**************************************************************************/
Queue newQueue(char szQueueNm[])
{
	Queue q = (Queue)malloc(sizeof(QueueImp));
	// Mark the list as empty
	q->pHead = NULL;    // empty list
	q->pFoot = NULL;    // empty list
	strcpy(q->szQName, szQueueNm);
	return q;
}
// linked list functions - you must provide the code for these (see course notes)
/******************** removeLL *************************************
int removeLL(LinkedList list, Event  *pValue)
Purpose:
    Removes a node from the front of a LinkedList and returns the
    Event structure (via the parameter list).
Parameters:
    I   LinkedList list       pointer to the LinkedList
    I   Event *pValue         value structure of node being removed

Returns:
  Functionally
      TRUE  - LinkedList was not empty
      FALSE - LinkedList was passed in empty
  Parameter
      Event structure containing value of node which was removed

Notes:
  Removes the first node from the linked list.  Returns the nodes event
  value via the parameter list.  Sets list->pHead to list->pHead->pNext.
**************************************************************************/
int removeLL(LinkedList list, Event *pValue)
{
	NodeLL *pRemove;
	if (list->pHead == NULL)
		return FALSE;                   // list was passed in empty
	*pValue = list->pHead->event;       // store Event structure into pValue
	                                    // so it can be returned via param
	pRemove = list->pHead;              // set the address of p to first 
	                                    // node in LinkedList
	list->pHead = list->pHead->pNext;   // new first node in LinkedList
	free(pRemove);
	return TRUE;                        // successfully removed a node
}
/******************** allocateNodeLL *************************************
NodeLL *allocateNodeLL(LinkedList list, Event value)
Purpose:
    Allocates new node for LinkedList.
Parameters:
    I   LinkedList list       pointer to the LinkedList
    I   Event value           values to be given to new node

Returns:
  A pointer to the new dynamically allocated node.

Notes:
  If dynamic allocation returns NULL pointer the program aborts.    
**************************************************************************/
NodeLL *allocateNodeLL(LinkedList list, Event value)
{
	NodeLL *pNew;
	pNew = (NodeLL *)malloc(sizeof(NodeLL));
	if (pNew == NULL)
		ErrExit(ERR_ALGORITHM, "No available memory for linked list");
	pNew->event = value;
	pNew->pNext = NULL;
	return pNew;
}
/******************** searchLL *******************************************
NodeLL *searchLL(LinkedList list, int match, NodeLL **ppPrecedes)
Purpose:
    Uses match (from argument passed in to function via parameter)
    to iterate over linked list and find if a NodeLL contains a matching
    value.
Parameters:
    I   LinkedList list       pointer to the LinkedList
    I   int   match           stores value to be found in LinkedList
    I   NodeLL ppPrecedes     pointer to a pointer to a NodeLL; used 
                              to return the address of the NodeLL found
                              right before the matching NodeLL

Returns:
  Functionally
    NULL    - If a match is not found in LinkedList
    Address - The address of the match in LinkedList
  Parameter
    ppPrecedes - returns address of previous NodeLL
Notes:
                               Values Returned
  Condition                    Functionally         Via Param
  Match at beginning           ppPrecedes = NULL    p = pHead
  Match Less than first Node   ppPrecedes = NULL    p = NULL
  Match between Nodes          ppPrecedes = p       p = p->pNext
  List is empty                ppPrecedes = NULL    p = NULL
  Match is less than           ppPrecedes = p       p = p->pNext
  (between nodes)
**************************************************************************/
NodeLL *searchLL(LinkedList list, int match, NodeLL **ppPrecedes)
{
	NodeLL *pCurrent;
	// used when the list is empty or we need to insert at the beginning
	*ppPrecedes = NULL;
	
	// Traverse through the list loooking for where the key belongs or
	// the end of the list.
	for (pCurrent = list->pHead; pCurrent != NULL; pCurrent = pCurrent->pNext)
	{
		if (match == pCurrent->event.iTime)     // assume match is an integer
			return pCurrent;
		if (match < pCurrent->event.iTime)      // assume the LL is ordered
			return NULL;
		*ppPrecedes = pCurrent;                 // pointer to a NodeLL
	}
	// Not found return NULL
	return NULL;
}
/******************** insertOrderedLL *************************************
NodeLL *insertOrderedLL(LinkedList list, Event value)
Purpose:
    Inserts a NodeLL into its respective place in the ordered
    LinkedList.
Parameters:
    I   LinkedList list       pointer to the LinkedList
    I   Event value           event structure

Returns:
   A pointer to a dynamically allocated NodeLL with its pNext pointer
   set according to the status of the singly linked list.
Notes:
   This function first searches the linked list to determine what the
   corresponding condition is regarding the status of the linked list.
   Next a new NodeLL is dynamically allocated and placed into the linked
   list according to the results of searchLL.  Below are the conditions:
   1. Empty linked list
   2. Insert at beginning
   3. Insert between two nodeLL's
   4. Insert at the end 

   Additionally, this function allows for duplicate entries based on
   value.iTime.
**************************************************************************/
NodeLL *insertOrderedLL(LinkedList list, Event value)
{
	NodeLL *pNew;
	NodeLL *pFind;
	NodeLL *pPrecedes;
	// see if it already exists
	// we could remove pFind since we are not concerned
	// with having duplicate entries
	pFind = searchLL(list, value.iTime, &pPrecedes);
	// commented out code to allow for duplicates
	//if (pFind != NULL)
	//	return pFind;
	
	// Allocate a node and insert.
	pNew = allocateNodeLL(list, value);
	
	// Check for inserting at the beginning of the list
	// this will also handle when the list is empty
	if (pPrecedes == NULL)
	{
		pNew->pNext = list->pHead;
		list->pHead = pNew;
	}
	else
	{
		pNew->pNext = pPrecedes->pNext;
		pPrecedes->pNext = pNew;
	}
	return pNew;
}
/******************** newLinkedList **************************************
LinkedList newLinkedList(void)
Purpose:
    Dynamically allocates memory for a new linked list.
Parameters:
    N/A
Returns:
    A pointer to a LinkedListImp structure (the pointer is called 
    list) that has a pointer (called pHead) to a NodeLL initialized to NULL.
Notes:
    If dynamic memory allocation fails program aborts.
**************************************************************************/
LinkedList newLinkedList()
{
	LinkedList list = (LinkedList) malloc(sizeof(LinkedListImp));
	
	if (list == NULL)
		ErrExit(ERR_ALGORITHM, "No available memory for linked list");
	// Mark the list as empty
	list->pHead = NULL;      // empty list
	return list;
}
// end of functions from notes
// begin program 3 and 4 functions
/******************** readEventData **************************************
int readEventData(LinkedList list)
Purpose:
    Takes input from stdin and adds it to list.
Parameters:
    I  LinkedList list       pointer to a LinkedListImp

Returns:
    Returns a time corresponding to the end of events
Notes:
    N/A
**************************************************************************/
int readEventData(LinkedList list)
{
	char szInputBuffer[MAX_LINE_SIZE];    // entire input line
	int iCurrentArriveTime = 0;           // used to store current
                                          // arrive event times
	int iNextArrivalTime;                 // stores next arrival
	int iScanfCnt;                        // successful converisons
	Event tempEventNode;                  // used to temporarily store data 
	                                      // to be passed as an arg later
	
	// take a line from p4ACinput.txt file
	while(fgets(szInputBuffer, MAX_LINE_SIZE, pAlternativeFileName) != NULL)
	{
		// stop reading new arrival events into linked list after MAX_ARRIVAL_TIME
		if (iCurrentArriveTime >= MAX_ARRIVAL_TIME)
			break;
		// skip line
		if (szInputBuffer[0] == '\n')
			continue;
			
        iScanfCnt = sscanf(szInputBuffer, "%ld %d %d %d[^\n]\n"  
			    , &tempEventNode.widget.lWidgetNr                      // number of current widget 
                , &tempEventNode.widget.iStep1tu                       // time widget spends at step1
				, &tempEventNode.widget.iStep2tu                       // time widget spends at step2
                , &iNextArrivalTime);                                  // time when next widget arrives
		
		tempEventNode.iEventType = EVT_ARRIVAL;
		tempEventNode.widget.iArrivalTime = iCurrentArriveTime;
		tempEventNode.iTime = iCurrentArriveTime;
		
		// check for bad input resulting in conversion issues
        if (iScanfCnt < 4)
           ErrExit(ERR_ALGORITHM, "Input conversion failed");
		insertOrderedLL(list, tempEventNode);
		// update iArriveTime to reflect when next person will arrive
		iCurrentArriveTime += iNextArrivalTime;
	} // end while
	return iCurrentArriveTime;
}
/******************** printLL *********************************************
void printLL(LinkedList list)
Purpose:
    Takes a linked list and prints it out iteratively.
Parameters:
    I  LinkedList list       pointer to a LinkedListImp

Returns:
    N/A
Notes:
    N/A
**************************************************************************/
void printLL(LinkedList list)
{
	NodeLL *p;
	
	printf("%-10s %-7s %-7s %-7s %8s %7s", "WidgetNr", "Step1tu", "Step2tu"
		, "ArrTime"
		, "EVT_TYPE"
		, "EvtTime");
	printf("\n");
	// iterate node-by-node printing each event
	for (p = list->pHead; p != NULL; p = p->pNext)
	{
		// print list node-by-node
		printf("%-10ld %-7d %-7d %-7d %-8d %3d\n"
			, p->event.widget.lWidgetNr
			, p->event.widget.iStep1tu
			, p->event.widget.iStep2tu
			, p->event.widget.iArrivalTime
			, p->event.iEventType
			, p->event.iTime);
	}
}
/************************** freeLL **************************************
void freeLL(LinkedList list)
Purpose:
    Correctly frees an entire linked list ensuring no memory leaks.
Parameters:
    I  LinkedList list       pointer to a LinkedListImp

Returns:
    N/A
Notes:
    First, this function frees the NodeLL's which have been allocated 
    memory.  Next, the actual memory allocated for the LinkedListImp
    structure is freed.  Finally, the pointer pointing to the
    LinkedListImp is set to NULL.
**************************************************************************/
void freeLL(LinkedList list)
{
	NodeLL *pRemove;           // used to iterate through
	                           // LL 
	Event removeEvent;   // used to store returned 
	                     // event from removeLL
	// list is either free or dangling
	// not much we can do
	if (list->pHead == NULL)
	{
		free(list);
		return;
	}
	// iterate node-by-node freeing each node
	for (pRemove = list->pHead; pRemove != NULL; pRemove = pRemove->pNext)
	{
		removeLL(list, &removeEvent);     // function frees node at i
	}
	// finally free the memory allocated for LinkedListImp structure
	free(list);
	// just to make sure the pointer is not used improperly in the future
	list->pHead = NULL;
}
/************************** newSim *******************************************************
Simulation newSim(LinkedList list)
Purpose:
    Conducts a simulation using the linked list with event nodes.
Parameters:
    I  Simulation simulation     pointer to a SimulationImp structure
    I  int bVerboseFlag          used to assign TRUE or FALSE to verbose mode
Returns:
    A pointer to a SimulationImp structure
Notes:
    If memory is unable to be allocated the program aborts.
*******************************************************************************************/
Simulation newSim(LinkedList list, int bVerboseFlag, char cRunAltV)
{
	Simulation simulation = (Simulation)malloc(sizeof(SimulationImp));
	if (simulation == NULL)
	    ErrExit(ERR_ALGORITHM, "No memory available for simulation");
	// set simulation to point to first node in list
	simulation->bVerbose = bVerboseFlag;   // sets value to user selected switch
	simulation->cRunType = cRunAltV;       // version of simulation to be executed
	simulation->eventList = list;
	return simulation;
}


int main(int argc, char *argv[])
{
	LinkedList list = newLinkedList();          // LinkedList
	Simulation sim;                             // simulation
	char *pszFileName = NULL;                   // will be used for extra credit portion
	char cRunSimVer;                            // initialized to run simulation Alt. A
	int bVerbose = FALSE;                       // TRUE or FALSE flag for verbose mode
	int simulationVersion;                      // Alt. A, Alt. B, or Alt. C to be executed
	
	simulationVersion = processCommandSwitches(argc, argv, &pszFileName, &bVerbose, &cRunSimVer);
	pAlternativeFileName = fopen(pszFileName, "r");
    if (pAlternativeFileName == NULL)
        exitUsage(USAGE_ERR, "Invalid customer file name, found "
            , pszFileName);
	
	readEventData(list);
	
	sim = newSim(list, bVerbose, cRunSimVer); 
	switch(simulationVersion)
	{
		case SIM_ALT_A:
		    runSimulationA(sim, MAX_SIM_TIME);
			break;
		case SIM_ALT_B:
		case SIM_ALT_C:
		    runSimulationBC(sim, MAX_SIM_TIME);
			break;
		default:
		    ErrExit(ERR_ALGORITHM, "Unknown switch option");
	} 
	// ensure memory is not leaked
	freeLL(list);
	free(sim);
	fclose(pAlternativeFileName);
	
	return (EXIT_SUCCESS);
}

// functions in most programs, but require modifications
/******************** exitUsage *****************************
    void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
Purpose:
    In general, this routine optionally prints error messages and diagnostics.
    It also prints usage information.

    If this is an argument error (iArg >= 0), it prints a formatted message 
    showing which argument was in error, the specified message, and
    supplemental diagnostic information.  It also shows the usage. It exits 
    with ERR_COMMAND_LINE.

    If this is a usage error (but not specific to the argument), it prints 
    the specific message and its supplemental diagnostic information.  It 
    also shows the usage and exist with ERR_COMMAND_LINE. 

    If this is just asking for usage (iArg will be -1), the usage is shown.
    It exits with USAGE_ONLY.
Parameters:
    I int iArg                      command argument subscript or control:
                                    > 0 - command argument subscript
                                    0 - USAGE_ONLY - show usage only
                                    -1 - USAGE_ERR - show message and usage
    I char *pszMessage              error message to print
    I char *pszDiagnosticInfo       supplemental diagnostic information
Notes:
    This routine causes the program to exit.
**************************************************************************/
void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
{
    switch (iArg)
    {
        case USAGE_ERR:
            fprintf(stderr, "Error: %s %s\n"
                , pszMessage
                , pszDiagnosticInfo);
            break;
        case USAGE_ONLY:
            break;
        default:
            fprintf(stderr, "Error: bad argument #%d.  %s %s\n"
                , iArg
                , pszMessage
                , pszDiagnosticInfo);
    }
    // print the usage information for any type of command line error
    fprintf(stderr, "Example usage : p4 -v -aA\n");
    if (iArg == USAGE_ONLY)
        exit(USAGE_ONLY); 
    else 
        exit(ERR_COMMAND_LINE);
}
/******************** processCommandSwitches *****************************
    void processCommandSwitches(int argc, char *argv[], char **ppszCustomerFileName
        , char **ppszQueryFileName)
Purpose:
    Checks the syntax of command line arguments and returns the filenames.  
    If any switches are unknown, it exits with an error.
Parameters:
    I   int argc                        count of command line arguments
    I   char *argv[]                    array of command line arguments
    O   char **ppszCustomerFileName     Customer File Name to return
    O   char **ppszQueryFileName        Query File Name to return 
    O   int  *pbVerboseFlag             value that turns verbose mode on
    O   char *pRunType					value that determines simulation version
 Returns:
    int which indicates which simulation version to run
Notes:
    If a -? switch is passed, the usage is printed and the program exits
    with USAGE_ONLY.
    If a syntax error is encountered (e.g., unknown switch), the program
    prints a message to stderr and exits with ERR_COMMAND_LINE_SYNTAX.
**************************************************************************/
int processCommandSwitches(int argc, char *argv[], char **ppszFileName, int *pbVerboseFlag, char *pRunType)
{
    int i;
	int simulationType = SIM_ALT_A;    // by default program will use Alternative A simulation
    // Examine each of the command arguments other than the name of the program.
    for (i = 1; i < argc; i++)
    {
		// bad usage
		if (argv[i][0] != '-')
			exitUsage(i, ERR_EXPECTED_SWITCH, argv[i]);
        // check for a switch
        if (argv[i][0] == '-')
		{
	        // determine which switch it is
	        switch (argv[i][1])
	        {	
	        case 'v':                   // Customer File Name
			    *pbVerboseFlag = TRUE;   // set to verbose mode
	            break;
			case 'a':
			    if (argv[i][2] == 'A')
				{
					*ppszFileName = "p4ACinput.txt";
					*pRunType = 'A';
					simulationType = SIM_ALT_A;		
					break;		
				}
				else if (argv[i][2] == 'B')
				{
					*ppszFileName = "p4Binput.txt";
					*pRunType = 'B';
					simulationType = SIM_ALT_B;
					break;
				}
				else if (argv[i][2] == 'C')
				{
					*ppszFileName = "p4ACinput.txt";
					*pRunType = 'C';
					simulationType = SIM_ALT_C;
					break;
				}
				else
				{
					exitUsage(i, ERR_EXPECTED_SWITCH, argv[i]);
				}
	        default:
			    exitUsage(i, ERR_EXPECTED_SWITCH, argv[i]);
	        } // end switch
		} // end if      
    } // end for
	return simulationType;
}
/******************** ErrExit **************************************
  void ErrExit(int iexitRC, char szFmt[], ... )
Purpose:
    Prints an error message defined by the printf-like szFmt and the
    corresponding arguments to that function.  The number of 
    arguments after szFmt varies dependent on the format codes in
    szFmt.  
    It also exits the program with the specified exit return code.
Parameters:
    I   int iexitRC             Exit return code for the program
    I   char szFmt[]            This contains the message to be printed
                                and format codes (just like printf) for 
                                values that we want to print.
    I   ...                     A variable-number of additional arguments
                                which correspond to what is needed
                                by the format codes in szFmt. 
Notes:
    - Prints "ERROR: " followed by the formatted error message specified 
      in szFmt. 
    - Prints the file path and file name of the program having the error.
      This is the file that contains this routine.
    - Requires including <stdarg.h>
Returns:
    Returns a program exit return code:  the value of iexitRC.
**************************************************************************/
void ErrExit(int iexitRC, char szFmt[], ... )
{
    va_list args;               // This is the standard C variable argument list type
    va_start(args, szFmt);      // This tells the compiler where the variable arguments
                                // begins.  They begin after szFmt.
    printf("ERROR: ");
    vprintf(szFmt, args);       // vprintf receives a printf format string and  a
                                // va_list argument
    va_end(args);               // let the C environment know we are finished with the
                                // va_list argument
    printf("\n");
    exit(iexitRC);
}