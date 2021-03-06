// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>
#include <kern/env.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
	{ "backtrace", "Display the infomation about the stack",mon_backtrace},
	{ "continue", "Continue execution from the current location",mon_continue},
	{ "si","Single-step one instruction at a time",mon_si},
};
#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < NCOMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.
	int i;
	unsigned int ebp = read_ebp();
    struct Eipdebuginfo info;
	char buffer[100];
    
	cprintf("Stack backtrace:\n");
	while(ebp!=0)
	{ 
		unsigned int eip = *((unsigned int *)ebp+1);
		unsigned int args[5] = {0} ;
		for(i= 0;i<5;i++)
			args[i] = *((unsigned int *)ebp+ 2 + i);
		
		cprintf(" ebp %08x eip %08x args %08x %08x %08x %08x %08x\n",ebp,eip,args[0],args[1],args[2],args[3],args[4]);
		
		debuginfo_eip(eip,&info);

		for(i=0;i<100&&i<info.eip_fn_namelen;i++)
			buffer[i] = info.eip_fn_name[i];
		buffer[i] = '\0';

		cprintf("      %s:%d:%.*s+%d\n",info.eip_file, info.eip_line, info.eip_fn_namelen ,info.eip_fn_name ,eip-info.eip_fn_addr);
        
	    ebp = *((unsigned int *)ebp);
	}      
	return 0;
}  

int 
mon_continue(int argc,char **argv,struct Trapframe *tf)
{
/*	extern struct Env *curenv;
	if(tf== NULL)
	{
		cprintf("cannot continue:tr==NULL\n");
		return -1;
	}
	else if(tf->tf_trapno !=T_BRKPT && tf->tf_trapno !=T_DEBUG)
	{
		cprintf("cannot continue:wrong trap number\n");
		return -1;
	}
	cprintf("continue execution from the location.........\n");
	tf->tf_eflags &=~FL_TF;//TFH置零，程序继续执行
	
	env_run(curenv);
*/
	return 0;
}

int 
mon_si(int argc,char **argv,struct Trapframe *tf)
{
	/* 
	extern struct Env *curenv;
    if(tf== NULL)                                          
	{
		cprintf("cannot continue:tr==NULL\n");
		return -1;
	}
	else if(tf->tf_trapno !=T_BRKPT && tf->tf_trapno !=T_DEBUG)
	{	
		cprintf("cannot continue:wrong trap number\n");
		return -1;
	}
	cprintf("Single-step one instruction at a time......\n");

	tf->tf_eflags |= FL_TF;

	struct Eipdebuginfo info;
	debuginfo_eip(tf->tf_eip,&info);
	cprintf("Si information :\n tf_eip=%08x\n%s:%d:%.*s+%d\n",tf->tf_eip,info.eip_file,info.eip_line,
			info.eip_fn_namelen,info.eip_fn_name,tf->tf_eip-info.eip_fn_addr);
	
	env_run(curenv);
*/
	return 0;
		
}
/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
