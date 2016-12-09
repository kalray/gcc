/* { dg-do run } */
/* { dg-options "-O2 -fstack-check" } */

#include <setjmp.h>

jmp_buf back_to_roots;
unsigned int overflows;

extern void abort();

asm ("\
.pushsection .text			\n\
.global __stack_overflow_detected	\n\
					\n\
__stack_overflow_detected:		\n\
	make $r1 = overflows		\n\
	;;				\n\
	lw $r0 = 0[$r1]			\n\
	;;				\n\
	add $r0 = $r0, 1		\n\
	;;				\n\
	sw 0[$r1] = $r0			\n\
	;;				\n\
	make $r0 = back_to_roots	\n\
	call longjmp			\n\
	;;				\n\
	goto .				\n\
	;;				\n\
					\n\
.popsection				\n\
");

__attribute__((noclone,noinline))
static void test_static_alloc ()
{
	char array[1024*1024];

	asm volatile("nop" :: "r"(array) : "memory");
}

__attribute__((noclone,noinline))
static void test_dynamic_alloc ()
{
        char *array;

	array = __builtin_alloca(1024*1024);

        asm volatile("nop" :: "r"(array) : "memory");
}

__attribute__((noclone,noinline))
static void test_dynamic_alloc2 (int size)
{
        char *array;

        array = __builtin_alloca(size);

        asm volatile("nop" :: "r"(array) : "memory");
}


__attribute__((noclone,noinline))
static void test_aligned_var ()
{
	char array[1024*1024] __attribute__((aligned(32)));

        asm volatile("nop" :: "r"(array) : "memory");
}

extern unsigned int RM_STACK_END;

int main()
{
	__builtin_k1_set (31, (unsigned int)&RM_STACK_END);

	if (!setjmp (back_to_roots))
		test_static_alloc ();

	if (!setjmp (back_to_roots))
		test_dynamic_alloc ();

        if (!setjmp (back_to_roots))
                test_dynamic_alloc2 (1024*1024);

	if (!setjmp (back_to_roots))
		test_aligned_var ();

	if (overflows != 4)
		abort ();

	return 0;
}

