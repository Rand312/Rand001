#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256,
       	ADD, SUB,	      //+ -
       	MUL, DIV, MOD,		//* / %
	NON,
        EQ, NEQ,	
	LOR, LAND,            //Logic operation
        LBRA, RBRA,           //(  )
	HNUMBER,NUMBER,REGISTER
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
	int priority;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE,0},				// spaces
	{"\\+", ADD,4},            			// plus
	{"-",SUB,4},
	{"\\*",MUL,5},
	{"/",DIV,5},
	{"%",MOD,5},
	{"!",NON,6},
	{"==",EQ,3},	// equal
	{"!=",NEQ,3},
	{"&&",LAND,2},
	{"\\|\\|",LOR,1},
	{"\\(",LBRA,7},
	{"\\)",RBRA,7},
	{"0[xX][0-9a-fA-F]+",HNUMBER,0},
	{"[0-9]+",NUMBER,0},
	{"\\$[a-zA-Z]+",REGISTER,0}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
	int priority;
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				switch(rules[i].token_type) {
					case NOTYPE: break;
			     		default:
						     tokens[nr_token].type=rules[i].token_type;
						     tokens[nr_token].priority=rules[i].priority;
						     for(int j=0;j<substr_len;j++)
						     {
							     tokens[nr_token].str[j]=*(substr_start+j);
						     }
						     tokens[nr_token].str[substr_len]=0;
						     nr_token++;
				}
				position+=substr_len;

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(int p, int q)
{
	int i;
	int lc=0, rc=0;
	if(tokens[p].type==LBRA && tokens[q].type==RBRA)
	{
		for(i=p+1;i<q;i++)
		{
			if(tokens[i].type==LBRA) lc++;
			if(tokens[i].type==RBRA) rc++;
			if(rc>lc) return false;
		}
	if(lc==rc) return true;
	}
	return false;
}

int dominant_operator(int p, int q)
{
	int i=0;
	int op=0;
	int num_LBRA=0;
	int min_priority=10;
	for(i=p;i<=q;i++)
	{
		if(tokens[i].type==NUMBER || tokens[i].type==HNUMBER || tokens[i].type==REGISTER) continue;
		if(tokens[i].type==LBRA)
		{
			i++;
			num_LBRA++;
			while(1)
			{
				if(tokens[i].type==LBRA) num_LBRA++;
				else if(tokens[i].type==RBRA) num_LBRA--;
				i++;
				if(num_LBRA==0) break;
			}
		}
		if(i<q && tokens[i].priority<=min_priority)
		{
			op=i;
			min_priority=tokens[i].priority;
		}
	}
	return op;
}
uint32_t eval(int p, int q)
{
	int value=0;
	int i;
	if(p>q) {Assert(p>q,"Bad epressions!\n");}
	else if(p==q)
	{
		switch(tokens[p].type)
		{
		case NUMBER:
			sscanf(tokens[p].str,"%d",&value);
			break;
		case HNUMBER:
			i=2;	
			while(tokens[p].str[i]!=0)    //sscanf(tokens[p].str,"%d",&value)
			{
				value*=16;
				value+=tokens[p].str[i]<58?tokens[p].str[i]-'0':tokens[p].str[i]-'a'+10;
				i++;
			}
			break;
		case REGISTER:
			if(!strcmp(tokens[p].str,"$eax")) value=cpu.eax;
			if(!strcmp(tokens[p].str,"$ebx")) value=cpu.ebx;
			if(!strcmp(tokens[p].str,"$ecx")) value=cpu.ecx;
			if(!strcmp(tokens[p].str,"$edx")) value=cpu.edx;
			if(!strcmp(tokens[p].str,"$esp")) value=cpu.esp;
			if(!strcmp(tokens[p].str,"$ebp")) value=cpu.ebp;
			if(!strcmp(tokens[p].str,"$esi")) value=cpu.esi;
			if(!strcmp(tokens[p].str,"$edi")) value=cpu.edi;
		}
	}

	
	else if(check_parentheses(p,q))
	{
		printf("9999999999999999\n");
		return eval(p+1,q-1);
	}
	else
	{
		int op;
		int op_pos;
		op_pos=dominant_operator(p,q);
		op=tokens[op_pos].type;
		int val1=eval(p,op_pos-1);
		int val2=eval(op_pos+1,q);

		switch(op)
		{
			case ADD:
			value=val1+val2;
			break;
			case SUB:
			value=val1-val2;
			break;
			case MUL:
			value=val1*val2;
			break;
			case DIV:
			value=val1/val2;
			break;
			case MOD:
			value=val1%val2;
			break;
			case EQ:
			value=(val1 == val2);
			break;
			case NEQ:
			value=(val1 != val2);
			break;
			case LAND:
			value=(val1 && val2);
			break;
			case LOR:
			value=(val1 || val2);
			break;

		}
			
	}
	return value;
}

uint32_t expr(char *e, bool *success)
{
	if(!make_token(e)) 
	{
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	for(int i=0;i<nr_token;i++)
	{
		printf(" 333 %d\n",tokens[i].type);
	}
	*success=true;
	return eval(0,nr_token-1);
}

