#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args)
{
	char *arg=strtok(args,"");
	if(arg==NULL)
	{
		printf("arguments were not enough");
		return 1;
	}
	int num=atoi(arg);
	cpu_exec(num);
	printf("Execute sucessfully");
	return 0;
}

static int cmd_info(char *args)
{
	char *arg = strtok(args,"");     //get the command r or w
	printf("%s\n",arg);
	if(strcmp(arg,"r")==0)
	    		{
			printf("register eax is %x\n",cpu.eax);
			printf("register ecx is %x\n",cpu.ecx);
			printf("register edx is %x\n",cpu.edx);
			printf("register ebx is %x\n",cpu.ebx);
			printf("register ebp is %x\n",cpu.esp);
			printf("register esi is %x\n",cpu.esi);
			printf("register edi is %x\n",cpu.edi);
			}
	//else
	return 0;
}

static int cmd_x(char *args)
{
	char *arg = strtok(args," ");  //get the number for recurcive times
	int n = atoi(arg);    	      //change the type from char to int
	char *EXPR = strtok(NULL," "); //get the adrress(char type)
	char *str;
	swaddr_t addr = strtol(EXPR,&str,16);
	for (int i = 0; i < n; i++)
	{
		uint32_t data = swaddr_read(addr+i*4,4);
		printf("0x%08x  ",addr+i*4);
		for(int j=0; j<4; j++)
		{
			printf("%02x  ",data & 0xff);
			data=data>>8;
		}
		printf("\n");
	}
	return 0;
}

static int cmd_p(char *args)
{
	uint32_t num;
	bool suc;
	num=expr(args,&suc);
	if(suc)
		printf("0x%x:\t%d\n",num,num);
	else 
	{
		printf("What's wrong?");
	}
	return 0;
}

static int cmd_w(char *args)
{
	WP *nnew=NULL;
	bool suc;
	int val;

	val=expr(args,&suc);
	if(!suc) Assert(1,"expression error");
	nnew=new_wp(args,val);

	printf("The watchpoint %d has been set, and the expression is %s\n",nnew->NO,nnew->expr);
	printf("The value is %d\n",nnew->val);

	return 0;
}

static int cmd_d(char *args)
{
	int dNO;
	sscanf(args,"%d",&dNO);
	free_wp(dNO);

	printf("Watchpoint %d has already been removed",dNO);	
	return 0;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{"si", "execute the given steps",cmd_si},
	{"info","print the register information or the watchpoint information",cmd_info},
	{"x", "print the data in memory in the certain area",cmd_x},
	{"p", "calculate  the expression",cmd_p},
	{"w", "set the watchpoint",cmd_w},
	{"d", "delete the watchpoint numbered NO",cmd_d}
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
