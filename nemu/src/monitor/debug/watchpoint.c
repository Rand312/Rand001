#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
		wp_pool[i].val=0;
		wp_pool[i].isused=0;
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
	printf("  111  \n");
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp(char *str, int value)
{
	printf("   222  \n");
	if(free_==NULL)
	{
		printf("There is no free watchpoint!!!");
		assert(0);
	}

	WP *new=free_;                 //create 
	free_=free_->next;

	new->val=value;               //assignment
	strcpy(new->expr,str);
	new->isused=1; 
	printf("  333  \n");
	if((head=NULL))                 //new->next assignment
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

void free_wp(int dNO)
{
	WP *p=head;
	
	if(head==NULL) Assert(1,"There is no watchpoint need to remove");

	else if(p->NO==dNO)
	{
		head=head->next;
		p->next=free_;
		free_=p;
		
		return;
	}
	
	else
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
		







