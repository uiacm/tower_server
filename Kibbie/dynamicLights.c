/* dynamicLights.h
	Written by: Jordan Lynn 
	email: snake.river52@gmail.com
	
	Light bars each get assigned their own node and are 
	given some type of identity, currently just a number, 
	for the structure. As well as three interger variables
	representing red, green and blue brightness for each
	LED.

	Currently each bar is represented by a node in a single linked
	list, there is only a head pointer currently but a tail could be
	added if it's needed for some reason.
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
//#include <avr/pgmspace.h>
//#include <avr/interrupt.h>
//#include "twrlights.h"

struct lightBar{
	
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	int position;
	
	struct lightBar *next;
};

// Globals:
// root will be global for now
struct lightBar *root = NULL;
int position = 0;

// Prototypes
void addToLL( struct lightBar *ptr );
void newLightBar( int route );
void shutDown();
void printList();
void deleteBars( int barNum );

int main(){
	int i;
	// Add in some switch for when the zigbee network detects a new
	// device entering the network.
	
	// Simulated scan and new device is found.
	// right now there are 5 different 'routes' being run.
	for( i = 0; i < 5; i++ ){
		newLightBar( i );
		printf("light bar created\n");
	}
	printf("printing list\n");	
	// Show deleteBars works.	
	printList();	

	// Now to simulate a lightbar leaving the zigbee network
	// this given bar was apart of route 4.
	deleteBars( 4 );	
	// Show deleteBars works.
	printList();
	
return 0;
}

// Simple function to remove a lighbar from linked list that is found by it's route #
// This is delete all bars of the route so all of route 5 would be removed.
void deleteBars( int barNum ){
	struct lightBar *curr;
	struct lightBar *prev;

	curr = root;
	prev = curr;
	printf("Searching through list to delete node...\n");
	while ( curr != NULL ){
		// handle case where we need to move the root ahead.
		if( curr == root && curr->position == barNum ){
			root = root->next; // advance the root
			free(curr); // let it go, you just gotta.
		}
		else if( curr->position == barNum ){
			prev->next = curr->next; // remove curr from list.
			free(curr); // let it go.
		}
		
		prev = curr;
		curr = curr->next; // advance down list.
	}
	printf("Bar(s) removed || didn't exist\n");
}	
				
		
// Simple function to print the current LL
void printList(){
	struct lightBar *curr; // Conductor to traverse list

	curr = root;
	while( curr != NULL ){
		printf("current value %d\n", curr->position);
		curr = curr->next;	
	}
}

/* Currently this is only a linked list but the way I've built it the code can easily
	be adapted for other structures, it completely makes the new node so it can exist on
	it's own and THEN places it in the list which can be useful if we need to hurry and
	push the new node's information out onto the network for whatever reason. 

	Calling "newLightBar()" will create a new node with the light bar's info, it then
	could possibly be put into a linked list and a Binary Search Tree per say. This is
	why I usually build my data structures like this.

	The variable 'route' is passed in is the route that is being run on say a field like
	in football, each route number that is the same will recieve the same data. So all
	route 3 bars would do the same pattern while all route 1 bars could do a different pattern.
*/
void newLightBar( int route ){
	// Ued to traverse list
	struct lightBar *curr;

	// Allocate new bar
	struct lightBar *ptr = (struct lightBar*)malloc(sizeof(struct lightBar));
	
	// Initialize it's values so that the lights don't come on
	// when it enters the network. 
	ptr->red = 0;
	ptr->green = 0;
	ptr->blue = 0;
	ptr->next = NULL;
	ptr->position = route;	
	/* From here you can now push the new lightbar's information to it
	  out onto the network and it will be accurate so you could call a
	  function here to do so and it would update nodes outside of the LL.
	  I've always liked building structures like this because now we have
	  a completed mini node that can be stuck anywhere we like.
	*/
	// Now call the function to attach it to the end of the LL
	addToLL( ptr );
	// addToBST( ptr ); // example on how versitile this technique is
	// addToQueue( ptr ); // you can pass this node to any function that
						  // creates a data structure.
}

/*	Quick function that adds new node that is passed in to the
	 end of the LL if one doesn't exist yet the funciton will
	 create one.
*/
void addToLL( struct lightBar *ptr ){
	struct lightBar *curr; // Used to traverse the list.
	
	// Handle empty case, or rather this is the first device
	if ( root == NULL )
		root = ptr;
	
	else{
		// start at the start of course!
		curr = root;
		// now advance down the list until you reach the last node.
		while(	curr->next != NULL ){
			curr = curr->next;
		}
		// We should be at the end of the list now so
		// we can attach "ptr" the node made in the begining.
		curr->next = ptr;
	}

	// If we run out of memory we end up here
	if ( curr == NULL )
		printf("RAN INTO NULL IN \"newLightBar\" function!");
}

/* shutDown is an example of the new dynamic code working, it will
	 shut off all light bars in the network.
*/
void shutDown(){
	int i = 0;
	// curr is used to traverse and shut down LL
	struct lightBar *curr = root; // start at the begining of list 
	for( i = 0; curr->next != NULL; i++ ){
		curr->red = 0; // set all colors to the 'off' position.
		curr->green = 0;
		curr->blue = 0;
		//outroom( curr->position, curr->red, curr->green, curr->blue );	
		//newframe();
		curr = curr->next; // advance to next node.
	}
	
	
	// All lights should be turned off! 
	// ... but I have a feeling I forgot something...
}
