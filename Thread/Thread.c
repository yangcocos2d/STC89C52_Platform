#include "Thread.h"

thread_manage_list idata  g_thread_manage_list = {{0},0,0,0};

void thread_create(thread_block* block,thread_pfun pfun,char* sp_init,char *name)
{
	block->PC_L =  (unsigned int)pfun%256 ;
	block->PC_H =  (unsigned int)pfun/256 ;
	block->sp = sp_init;
	block->status = normal;
	block->name = name;
	g_thread_manage_list.list[g_thread_manage_list.list_len] = block;
	g_thread_manage_list.list_len++;
}

void thread_suspend(thread_block* block)
{
	block->status = suspend;
}

void thread_normal(thread_block* block)
{
	block->status = normal;
}

void thread_set_next_thread(thread_block* block)
{
	if(block != 0)
	{
		g_thread_manage_list.thread_next = block;
	}

}

thread_block* thread_schedule(void)
{
	static int next_thread = 0;
	do
	{
		next_thread++;
		if(next_thread >= g_thread_manage_list.list_len)
		{
			next_thread = 0;
		}
	}
	while(g_thread_manage_list.list[next_thread]->status != normal );

	return g_thread_manage_list.list[next_thread];
	
}

void thread_start(void)
{
	g_thread_manage_list.thread_cur = 0;
	g_thread_manage_list.thread_next = thread_schedule();
}

char thread_change(char sp)
{
	char *my_sp =(char *)sp;
	thread_block* p_th_block = 0;

	p_th_block = g_thread_manage_list.thread_cur;
	if(p_th_block != 0)
	{
		p_th_block->sp	= (char)(my_sp - THREAD_CONTEXT_SIZE);
		p_th_block->R7 	= *(my_sp);
		p_th_block->R6 	= *(my_sp-1);
		p_th_block->R5 	= *(my_sp-2);
		p_th_block->R4 	= *(my_sp-3);
		p_th_block->R3 	= *(my_sp-4);
		p_th_block->R2 	= *(my_sp-5);
		p_th_block->R1 	= *(my_sp-6);
		p_th_block->R0 	= *(my_sp-7);
		p_th_block->PSW = *(my_sp-8);
		p_th_block->DPL = *(my_sp-9);
		p_th_block->DPH = *(my_sp-10);
		p_th_block->B 	= *(my_sp-11);
		p_th_block->ACC = *(my_sp-12);
		p_th_block->PC_H= *(my_sp-13);
		p_th_block->PC_L= *(my_sp-14); 
	}
 
	p_th_block = g_thread_manage_list.thread_next;
	my_sp 	   = (char *)(p_th_block->sp + THREAD_CONTEXT_SIZE);
	*(my_sp)   = p_th_block->R7 ;
	*(my_sp-1) = p_th_block->R6 ;
	*(my_sp-2) = p_th_block->R5 ;
	*(my_sp-3) = p_th_block->R4 ;
	*(my_sp-4) = p_th_block->R3 ;
	*(my_sp-5) = p_th_block->R2 ;
	*(my_sp-6) = p_th_block->R1 ;
	*(my_sp-7) = p_th_block->R0 ;
	*(my_sp-8) = p_th_block->PSW ;
	*(my_sp-9) = p_th_block->DPL ;
	*(my_sp-10)= p_th_block->DPH ;
	*(my_sp-11)= p_th_block->B ;
	*(my_sp-12)= p_th_block->ACC ;
	*(my_sp-13)= p_th_block->PC_H;
	*(my_sp-14)= p_th_block->PC_L;
	
	g_thread_manage_list.thread_cur = g_thread_manage_list.thread_next;
	g_thread_manage_list.thread_next = thread_schedule();
	
	return (char)my_sp;
}
