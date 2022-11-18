#pragma once

#include <setjmp.h>
#include "defines.h"

#define MAX_JUMP 1024
typedef jmp_buf* jmp_stack[MAX_JUMP];

void jmp_stack_push(jmp_buf* jb);
jmp_buf* jmp_stack_pop();

#define try \
	jmp_buf __jmp_buf; \
	int __jmp_result = setjmp(__jmp_buf); \
	if (!__jmp_result) jmp_stack_push(&__jmp_buf); \
	if (!__jmp_result)

#define catch(x) \
	int x = __jmp_result; \
	if (!x) jmp_stack_pop(); \
	else

#define throw(x) longjmp (*jmp_stack_pop(), x)

#define ASSERT(exp, err_no) if (!(exp)) throw(err_no);