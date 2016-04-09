#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cs2123p4.h"

// simulation functions for program 4
/******************** arrival ***************************************************************
void arrival(Simulation sim, Widget *widget)
Purpose:
    Literally does nothing...  It prints out an arrival message and increments total
    widget count.
Parameters:
    I    Simulation simulation     pointer to a SimulationImp structure
    I    Widget *widget            a pointer to a widget removed from linked list
Returns:
    N/A
Notes:
    Will not allow any new arrival events to be put into queue after MAX_ARRIVAL_TIME.
*******************************************************************************************/
void arrival(Simulation sim, Widget *widget)
{
	// allow new arrivals events until MAX_ARRIVAL_TIME
	if (sim->iClock >= MAX_ARRIVAL_TIME)
		return;
	
	if (sim->bVerbose == TRUE)
	{
		printf("%4d %5ld  %s\n"
			, widget->iArrivalTime
			, widget->lWidgetNr
			, "Arrived");
	}
	sim->lWidgetCount++;    // total number of widgets processed in simulation
}
/******************** queueUp ***************************************************************
void queueUp(Simulation sim, Queue queueTeller, Widget *widget)
Purpose:
    Responsible for adding removed node from linked list to appropriate queueTeller (queue).
Parameters:
    I    Simulation simulation     pointer to a SimulationImp structure
    I/O  Queue queueTeller         a pointer to the queue (queue 1 or queue 2)
    I    Widget *widget            a pointer to a widget removed from linked list
Returns:
    N/A
Notes:
    Adds new nodes into queue for later processing.
*******************************************************************************************/
void queueUp(Simulation sim, Queue queueTeller, Widget *widget)
{
	QElement elementForQ;            // store info from widget and insert into queue
	elementForQ.widget = *widget;     // store current widget
	elementForQ.iEnterQTime = sim->iClock; // time widget inserted into queue
	if (sim->bVerbose == TRUE)
	{
		printf("%4d %5ld  Enter %s\n"
			, sim->iClock
			, widget->lWidgetNr
			, queueTeller->szQName);
	}
	insertQ(queueTeller, elementForQ);
	queueTeller->lQueueWidgetTotalCount++;  // increment widget count by one
}
/******************** seize ***************************************************************
void seize(Simulation sim, Queue queueTeller, Server serverTeller)
Purpose:
    Responsible for sending removed queue node into the appropriate server for processing.
Parameters:
    I    Simulation sim               pointer to a SimulationImp structure
    I/O  Queue queueTeller            a pointer to the queue (queue 1 or queue 2)
    I/O  Server serverTeller          a pointer to a ServerImp struct
Returns:
    N/A
Notes:
    1.) Checks if serverTeller is busy.
    2.) If serverTeller is not busy a node is removed from queueTeller.
         -checks queueTeller for empty before attempting to process widget into
          server
    3.) Updates serverTeller to busy if a node is removed from queueTeller
    4.) Creates EVT_SERVER(1 or 2)_COMPLETE events to be added into linked list
*******************************************************************************************/
void seize(Simulation sim, Queue queueTeller, Server serverTeller)
{
	QElement removedQElement;    // use to store removed NodeQ from queueTeller	
	Event addValue;              // use to add to linked list
	int iQWaitTime = 0;          // time a single nodeQ spent in queue
	
	// if server is not busy remove nodeQ from queueTeller and begin processing
	if (serverTeller->bBusy == FALSE)
	{
		if (removeQ(queueTeller, &removedQElement) == TRUE)
		{
			iQWaitTime = sim->iClock - removedQElement.iEnterQTime;
			queueTeller->lQueueWaitSum += iQWaitTime;
			if (sim->bVerbose == TRUE)
			{
				// print leave queue
				printf("%4d %5ld  Leave %s, waited %d\n"
					, sim->iClock
					, removedQElement.widget.lWidgetNr
					, queueTeller->szQName
					, iQWaitTime); // amount of time widget spent in queue
				// print seized server	
				if (sim->cRunType == 'A')
				{
					printf("%4d %5ld  Seized %s\n"
						, sim->iClock
						, removedQElement.widget.lWidgetNr
						, serverTeller->szServerName);
				}
				else 
				{
					printf("%4d %5ld  Begin Processing %s\n"
						, sim->iClock
						, removedQElement.widget.lWidgetNr
						, "Server 1");
				}
			}
			serverTeller->widget = removedQElement.widget;
		    serverTeller->bBusy = TRUE;   // server is now busy
		    if (sim->cRunType == 'A')
			{
			    if (strcmp("Server 1", serverTeller->szServerName) == 0)
			    {  // add EVT_SERVER1_COMPLETE to linked list
					addValue.iEventType = EVT_SERVER1_COMPLETE;
					// iStep1tu + iStep2tu will be the total amount of time
					// widget spends processing in server 1
					addValue.iTime = sim->iClock + serverTeller->widget.iStep1tu;
					addValue.widget = serverTeller->widget;
					insertOrderedLL(sim->eventList, addValue);
			    }
				else 
				{ // should be server 2 event now
					addValue.iEventType = EVT_SERVER2_COMPLETE;
					addValue.iTime = sim->iClock + serverTeller->widget.iStep2tu;
					addValue.widget = serverTeller->widget;
					insertOrderedLL(sim->eventList, addValue);
				}
			} 
			else 
			{ // simulation B or C
					addValue.iEventType = EVT_SERVER1_COMPLETE;
					addValue.iTime = sim->iClock 
						+ serverTeller->widget.iStep1tu
						+ serverTeller->widget.iStep2tu;
					addValue.widget = serverTeller->widget;
					insertOrderedLL(sim->eventList, addValue);
			}			
		}			
	} // end if
} // end seize function
/******************** release *************************************************************
void realease(Simulation sim, Queue queueTeller, Server serverTeller, Widget *widget)
Purpose:
    This function prints out information if verbose mode is selected and sets serverTeller
    to available.  
Parameters:
    I  Simulation simulation     pointer to a SimulationImp structure
    I  Queue queueTeller         pointer to a QueueImp (can be either 1 or 2)
    I  Server serverTeller       pointer to a ServerImp structure (either server 1 or server 2)
    I  Widget widget             widget structure
    
Returns:
    N/A
Notes:
    N/A 
*******************************************************************************************/
void release(Simulation sim, Queue queueTeller, Server serverTeller, Widget *widget)
{
	if (sim->bVerbose == TRUE)
	{
		if (sim->cRunType == 'A')
		{
			printf("%4d %5ld  Released %s\n"
				, sim->iClock
				, widget->lWidgetNr
				, serverTeller->szServerName);
		}
		else
		{
			printf("%4d %5ld  Complete Processing %s\n"
				, sim->iClock
				, widget->lWidgetNr
				, "Server 1");
		}
	}
	serverTeller->bBusy = FALSE;  // server can now take next widget from queue
}
/******************** leaveSystem ***********************************************************
void leaveSystem(Simulation sim, Widget widget)
Purpose:
    Processes simulation exit events.
Parameters:
    I  Simulation sim     pointer to a SimulationImp structure
    I  Widget widget      widget structure
Returns:
    N/A
Notes:
     Updates sim->lSystemTimeSum which is used for averaging total time widgets spend in
     system.  Each widget that exits the systems time in system is added to the total 
     time sum.
*******************************************************************************************/
void leaveSystem(Simulation sim, Widget widget)
{
	int iTimeInSystem = 0;    // used to store total time in system
	iTimeInSystem = sim->iClock - widget.iArrivalTime;
	sim->lSystemTimeSum += iTimeInSystem;   // increment total time
	                                        // widgets spent in system
	if (sim->bVerbose == TRUE)
	{
		printf("%4d %5ld  Exit system, in system %d\n"
			, sim->iClock
			, widget.lWidgetNr
			, iTimeInSystem);
	}
}
/******************** generateArrival *******************************************************
void generateArrival(Simulation sim)
Purpose:
    Truthfully, this is completely unnecessary.  As it stands it initializes values
    and prints out an initial header format.  Remove and do inside of newSim().
Parameters:
    I/O  Simulation simulation     pointer to a SimulationImp structure
Returns:
    N/A
Notes:
    N/A 
*******************************************************************************************/
void generateArrival(Simulation sim)
{
	sim->iClock = 0;    // initialize clock time
	sim->lSystemTimeSum = 0;
	sim->lWidgetCount = 0; 
	if (sim->bVerbose == TRUE)
		printf("%4s %5s %s\n", "Time", "Widget", "Event");
}

/******************** runSimulationA *******************************************************
void runSimulationA(Simulation simulation, int iTimeLimit)
Purpose:
    Conducts simulation requested by user (-aA option) which uses simulation functions,
    a linked list and queue(s).
Parameters:
    I  Simulation simulation     pointer to a SimulationImp structure
    I  int iTimeLimit            maximum time allowed before function return
Returns:
    N/A
Notes:
    The simulation works by iteratively removing the first node in the linked list.
    The event returned from removeLL (via param) is used for the switch to determine
    which action to take.  If returned event value from removeLL is not recognized,
    the program is aborted.  
*******************************************************************************************/
void runSimulationA(Simulation simulation, int iTimeLimit)
{
	Event removedEvent;               // stores NodeLL returned from removeLL	
	int iClock = 0;                   // used to advance clock
	int iAverageQueueOneTime = 0;
	int iAverageQueueTwoTime = 0;
	Queue queueTeller1 = newQueue("Queue 1");  // queue number 1
	Queue queueTeller2 = newQueue("Queue 2");  // queue number 2
	Server serverTeller1 = newServer("Server 1"); // server 1
	Server serverTeller2 = newServer("Server 2"); // server 2
	
	generateArrival(simulation);
	
	while(removeLL(simulation->eventList, &removedEvent))
	{		
		if (simulation->iClock > iTimeLimit)
			break;
		simulation->iClock = removedEvent.iTime;
		switch(removedEvent.iEventType)
		{
			case EVT_ARRIVAL:
			    arrival(simulation, &removedEvent.widget);
				queueUp(simulation, queueTeller1, &removedEvent.widget);
				seize(simulation, queueTeller1, serverTeller1);
			    break;
			case EVT_SERVER1_COMPLETE:
     			release(simulation, queueTeller1, serverTeller1, &removedEvent.widget);
				seize(simulation, queueTeller1, serverTeller1);
	            queueUp(simulation, queueTeller2, &removedEvent.widget);
				seize(simulation, queueTeller2, serverTeller2);
			    break;
			case EVT_SERVER2_COMPLETE:
 			    release(simulation, queueTeller2, serverTeller2, &removedEvent.widget);
				seize(simulation, queueTeller2, serverTeller2);
			    leaveSystem(simulation, removedEvent.widget);
				break;
			default:
			    ErrExit(ERR_ALGORITHM, "Attempt to run simulation with unknown event");
		} // end switch
	} // end while
	// print statistics from simulation
	printf("%4d        %s\n"
			, simulation->iClock
		    , "Simulation Complete for Alternative A");
	printf("      Average Queue Time for Server 1 =     %-4.1f\n"
		, ((double)queueTeller1->lQueueWaitSum/queueTeller1->lQueueWidgetTotalCount));
	printf("      Average Queue Time for Server 2 =     %-4.1f\n"
		, ((double)queueTeller2->lQueueWaitSum/queueTeller2->lQueueWidgetTotalCount));
	printf("      Average Time in System          =     %-4.1f\n"
		, ((double)simulation->lSystemTimeSum/simulation->lWidgetCount));
	// free queues and queue nodes (still need to do this)
	free(queueTeller1);
	free(queueTeller2);
	// free servers
	free(serverTeller1);
	free(serverTeller2);
} // end runSimulationA
/******************** runSimulationBC *******************************************************
void runSimulationBC(Simulation simulation, int iTimeLimit)
Purpose:
    Conducts simulation requested by user (-aB or -aC option) which uses simulation functions,
    a linked list and queue(s).
Parameters:
    I  Simulation simulation     pointer to a SimulationImp structure
    I  int iTimeLimit            maximum time allowed before function return
Returns:
    N/A
Notes:
    The simulation works by iteratively removing the first node in the linked list.
    The event returned from removeLL (via param) is used for the switch to determine
    which action to take.  If returned event value from removeLL is not recognized,
    the program is aborted.  
*******************************************************************************************/
void runSimulationBC(Simulation simulation, int iTimeLimit)
{
	Event removedEvent;               // stores NodeLL returned from removeLL	
	int iClock = 0;                   // used to advance clock
	int iAverageQueueOneTime = 0;
	Queue queueTeller1 = newQueue("Queue 1");  // queue number 1
	Server serverTeller1 = newServer("Server 1"); // server 1
	
	generateArrival(simulation);
	
	while(removeLL(simulation->eventList, &removedEvent))
	{		
		if (simulation->iClock > iTimeLimit)
			break;
		simulation->iClock = removedEvent.iTime;
		switch(removedEvent.iEventType)
		{
			case EVT_ARRIVAL:
			    arrival(simulation, &removedEvent.widget);
				queueUp(simulation, queueTeller1, &removedEvent.widget);
				seize(simulation, queueTeller1, serverTeller1);
			    break;
			case EVT_SERVER1_COMPLETE:
     			release(simulation, queueTeller1, serverTeller1, &removedEvent.widget);
				seize(simulation, queueTeller1, serverTeller1);
				leaveSystem(simulation, removedEvent.widget);
			    break;
			default:
			    ErrExit(ERR_ALGORITHM, "Attempt to run simulation with unknown event");
		} // end switch
	} // end while
	// print statistics from simulation
	printf("%4d        %s f%c\n"
			, simulation->iClock
		    , "Simulation Complete for Alternative"
			, simulation->cRunType);
	printf("      Average Queue Time for Server 1 =     %-4.1f\n"
		, ((double)queueTeller1->lQueueWaitSum/queueTeller1->lQueueWidgetTotalCount));
	printf("      Average Time in System          =     %-4.1f\n"
		, ((double)simulation->lSystemTimeSum/simulation->lWidgetCount));
	// free queues and queue nodes (still need to do this)
	free(queueTeller1);
	// free servers
	free(serverTeller1);
} // end runSimulationB