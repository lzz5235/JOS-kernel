#include <inc/assert.h>
#include <inc/x86.h>
#include <kern/spinlock.h>
#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>

void sched_halt(void);

// Choose a user environment to run and run it.
void RR_sched(void)
{
	int now_env , i;
    if (curenv) 
    {
	//	thiscpu -> cpu_env;
		now_env = (ENVX(curenv ->env_id) + 1) % NENV;
	} 
	else 
	{
	    now_env = 0;
	}
	for (i = 0; i < NENV; i++, now_env = (now_env + 1) % NENV) 
	{
	    if (envs[now_env ]. env_status == ENV_RUNNABLE)
	    {
//			cprintf ("I am CPU %d , I am in sched yield , I find ENV %d\n",thiscpu ->cpu_id , now_env );
	        env_run (& envs[now_env ]);
		}
	}
	                           
	if (curenv && curenv ->env_status == ENV_RUNNING)
	{
	    env_run(curenv);
	}
}
	
void RR_Priority_sched(void)
{
	int now_env,i;
	if(curenv)
	{
		now_env = (ENVX(curenv->env_id) +1) % NENV;
	}
	else
	{
		now_env = 0 ;
	}

	uint32_t max_priority = 0;
	int select_env = -1;

//	cprintf("NENV=%d\n",NENV);
	for(i= 0;i< NENV;i++ , now_env = (now_env+1)%NENV)
	{
		if(envs[now_env].env_status ==ENV_RUNNABLE && (envs[now_env].env_priority > max_priority
					||select_env == -1))
		{
			select_env=now_env;
			max_priority = envs[now_env].env_priority;
//			cprintf ("I am CPU %d , I am in sched yield , I find ENV %d,Priority 0x%x, i = %d\n",
//					thiscpu ->cpu_id , select_env,max_priority,i);
		}
	}
	
	//cprintf ("I am CPU %d , I am in sched yield , I find ENV %d,Priority %d\n",thiscpu ->cpu_id , select_env,max_priority);
	
	if (select_env >= 0 && (! curenv || curenv ->env_status != ENV_RUNNING ||
				max_priority >= curenv ->env_priority)) 
	{
		env_run (& envs[select_env ]);
	}
	if (curenv && curenv ->env_status == ENV_RUNNING) 
	{
		env_run(curenv);
	}

}

void
sched_yield(void)
{
	struct Env *idle;

	// Implement simple round-robin scheduling.
	//
	// Search through 'envs' for an ENV_RUNNABLE environment in
	// circular fashion starting just after the env this CPU was
	// last running.  Switch to the first such environment found.
	//
	// If no envs are runnable, but the environment previously
	// running on this CPU is still ENV_RUNNING, it's okay to
	// choose that environment.
	//
	// Never choose an environment that's currently running on
	// another CPU (env_status == ENV_RUNNING). If there are
	// no runnable environments, simply drop through to the code
	// below to halt the cpu.

	// LAB 4: Your code here.

//	RR_sched();
	
	RR_Priority_sched();

	// sched_halt never returns
	sched_halt();
}

// Halt this CPU when there is nothing to do. Wait until the
// timer interrupt wakes it up. This function never returns.
//
void
sched_halt(void)
{
	int i=0;

	// For debugging and testing purposes, if there are no runnable
	// environments in the system, then drop into the kernel monitor.
	//
	//
/*	for (i = 0; i < NENV; i++) 
	{
	        if ((envs[i].env_status == ENV_RUNNABLE ||
	             envs[i].env_status == ENV_RUNNING)) 
			{
	            cprintf("CPU %d : %d env is ", thiscpu->cpu_id, i);
				if (envs[i].env_status == ENV_RUNNABLE) 
					cprintf("ENV_RUNNABLE\n");
				else 
					cprintf("ENV_RUNNING\n");
				
	        }
	}
*/
//	cprintf("NENV = %d\n",NENV);
	for (i = 0; i < NENV; i++) {
		if ((envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING))
			break;
	}
	if (i == NENV) {
		cprintf("No runnable environments in the system!\n");
		while (1)
			monitor(NULL);
	}

	// Mark that no environment is running on this CPU
	curenv = NULL;
	lcr3(PADDR(kern_pgdir));

	// Mark that this CPU is in the HALT state, so that when
	// timer interupts come in, we know we should re-acquire the
	// big kernel lock
	xchg(&thiscpu->cpu_status, CPU_HALTED);

	// Release the big kernel lock as if we were "leaving" the kernel
	unlock_kernel();

	// Reset stack pointer, enable interrupts and then halt.
	asm volatile (
		"movl $0, %%ebp\n"
		"movl %0, %%esp\n"
		"pushl $0\n"
		"pushl $0\n"
		"sti\n"
		"hlt\n"
	: : "a" (thiscpu->cpu_ts.ts_esp0));
}

