#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	
	/* TODO: Add more members if necessary */
	
	int val;
	char expr[128];
	bool isused;	

} WP;

WP *new_wp(char *, int);
void free_wp(int);

#endif
