#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;    //point to NO+1 node
	
	/* TODO: Add more members if necessary */
	
	int val;          //the expression value
	char expr[32];   //the expression
	bool isused;	//check the if the node is used

} WP;

WP *new_wp(char *, int);
void free_wp(int);

#endif
