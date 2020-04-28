#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>

struct Process {
	char name[32];
	int s, t, idx;
	int isrun, isfork;
	pid_t pid;
};
void SetPidCPU(pid_t pid, int cpu_id);
void SchedulerFIFO(struct Process *ps, int n);
void SchedulerRR(struct Process *ps, int n);
void SchedulerSJF(struct Process *ps, int n);
void SchedulerPSJF(struct Process *ps, int n);

#endif