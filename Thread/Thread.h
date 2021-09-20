#ifndef _THREAD_H_
#define _THREAD_H_

#define THREAD_LIST_LEN						5
#define THREAD_CONTEXT_SIZE				15

typedef void(*thread_pfun)(void);
typedef enum thread_status {normal,suspend} enum_thread_status;

typedef struct thread_block
{
	char PC_L;
	char PC_H;
	char ACC;
	char B;
	char DPH;
	char DPL;
	char PSW;
	char R0;
	char R1;
	char R2;
	char R3;
	char R4;
	char R5;
	char R6;
	char R7;
	char sp;
	enum_thread_status status;
	char* name;
}thread_block;

typedef struct thread_manage_list
{
	int 						list_len;
	thread_block* 	list[THREAD_LIST_LEN];
	thread_block*   thread_cur;
	thread_block*   thread_next;
}thread_manage_list;

void thread_create(thread_block* block,thread_pfun pfun,char* sp_init,char *name);
void thread_normal(thread_block* block);
void thread_suspend(thread_block* block);
void thread_start(void);
char thread_change(char sp); 
void thread_set_next_thread(thread_block* block);
#endif
