#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {       //initialize each element in wp_pool array
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];      //create the list
		wp_pool[i].val=0;
		wp_pool[i].isused=0;
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp(char *str, int value)    //creat new watchpoint function
{
	if(free_==NULL)            // there is no free watchpoint
	{
		printf("There is no free watchpoint!!!");
		assert(0);
	}

	WP *new=free_;              //create a new node      
	free_=free_->next;

	new->val=value;            //assignment
	strcpy(new->expr,str);
	new->isused=1; 

	if((head=NULL))             //assign the value to new->next    
	{ 
		new->next=NULL;
		head=new;
	}
	else
	{
		new->next=head;
		head=new;
	}
	return new;

}

void free_wp(int dNO)     //free the node numbered dNO
{
	WP *p=head;
	
	if(head==NULL) Assert(1,"There is no watchpoint need to remove");

	else if(p->NO==dNO)             //the watchpoint which need to free is at the head of list
	{
		head=head->next;
		p->val=0;
		p->isused=0;
		p->next=free_;
		free_=p;
				
		return;
	}
	
	else                          //the ordinary situation then operate the list
	{
		WP *q=head;
		p=p->next;
		while(p!=NULL)
		{
			if(p->NO==dNO)
			{
				q->next=p->next;
				p->next=free_;
				p->val=0;
				p->isused=0;
				free_=p;
				
				return;

			}
		p=p->next;
		q=q->next;
		}
	}

	printf("There is no watchpoing numbered dNO");
	return;
}	
		







