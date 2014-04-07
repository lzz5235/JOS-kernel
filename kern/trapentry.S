/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
TRAPHANDLER_NOEC(__idt_divide,T_DIVIDE)
TRAPHANDLER_NOEC(__idt_debug,T_DEBUG)
TRAPHANDLER_NOEC(__idt_nmi,T_NMI)
TRAPHANDLER_NOEC(__idt_breakpoint,T_BRKPT)
TRAPHANDLER_NOEC(__idt_overflow,T_OFLOW)
TRAPHANDLER_NOEC(__idt_bound,T_BOUND)
TRAPHANDLER_NOEC(__idt_illop,T_ILLOP)
TRAPHANDLER_NOEC(__idt_device,T_DEVICE)
TRAPHANDLER(__idt_dblflt,T_DBLFLT)
TRAPHANDLER(__idt_tss,T_TSS)
TRAPHANDLER(__idt_segnp,T_SEGNP)
TRAPHANDLER(__idt_stack,T_STACK)
TRAPHANDLER(__idt_gpflt,T_GPFLT)
TRAPHANDLER(__idt_pgflt,T_PGFLT)
TRAPHANDLER_NOEC(__idt_fperr,T_FPERR)
TRAPHANDLER(__idt_align,T_ALIGN)
TRAPHANDLER_NOEC(__idt_mchk,T_MCHK)//MCE 
TRAPHANDLER_NOEC(__idt_simd,T_SIMDERR)


TRAPHANDLER_NOEC(__idt_syscall,T_SYSCALL)
TRAPHANDLER(__idt_default,T_DEFAULT)


TRAPHANDLER_NOEC(__idt_irq0,IRQ_OFFSET+ IRQ_TIMER)

TRAPHANDLER_NOEC(__idt_irq1,IRQ_OFFSET+ IRQ_KBD)
TRAPHANDLER_NOEC(__idt_irq2,IRQ_OFFSET+ 2)
TRAPHANDLER_NOEC(__idt_irq3,IRQ_OFFSET+ 3)
TRAPHANDLER_NOEC(__idt_irq4,IRQ_OFFSET+ IRQ_SERIAL)
TRAPHANDLER_NOEC(__idt_irq5,IRQ_OFFSET+ 5)
TRAPHANDLER_NOEC(__idt_irq6,IRQ_OFFSET+ 6)
TRAPHANDLER_NOEC(__idt_irq7,IRQ_OFFSET+ IRQ_SPURIOUS)
TRAPHANDLER_NOEC(__idt_irq8,IRQ_OFFSET+ 8)
TRAPHANDLER_NOEC(__idt_irq9,IRQ_OFFSET+ 9)
TRAPHANDLER_NOEC(__idt_irq10,IRQ_OFFSET+ 10)
TRAPHANDLER_NOEC(__idt_irq11,IRQ_OFFSET+ 11)
TRAPHANDLER_NOEC(__idt_irq12,IRQ_OFFSET+ 12)
TRAPHANDLER_NOEC(__idt_irq13,IRQ_OFFSET+ 13)
TRAPHANDLER_NOEC(__idt_irq14,IRQ_OFFSET+ IRQ_IDE)
TRAPHANDLER_NOEC(__idt_irq15,IRQ_OFFSET+ 15)
TRAPHANDLER_NOEC(__idt_irq19,IRQ_OFFSET+ IRQ_ERROR)



/*
 * Lab 3: Your code here for _alltraps
 */
_alltraps:
	pushl %ds
	pushl %es
	pushal

	movw  $GD_KD, %ax
	movw  %ax,%ds
	movw  %ax,%es

	pushl %esp
	call trap






