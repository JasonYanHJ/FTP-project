#include "try_catch.h"

jmp_stack g_jump_stack;
int g_jump_stack_idx;

void jmp_stack_push(jmp_buf* jb)
{
	g_jump_stack[g_jump_stack_idx++] = jb;
}

jmp_buf* jmp_stack_pop()
{
	return g_jump_stack[--g_jump_stack_idx];
}
