#define _GNU_SOURCE

#include "scheduler.h"
#define RR_ROUND 500
#define UNIT_TIME { volatile unsigned long x; for (x = 0; x < 1000000UL; x ++); }

void SetPidCPU(pid_t pid, int cpu_id) {
	assert(cpu_id < CPU_SETSIZE);
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_id, &cpu);
	assert(sched_setaffinity(pid, sizeof(cpu), &cpu) != -1);
}

void SetPidPriority(pid_t pid, int priority) {
	struct sched_param par;
	par.sched_priority = priority;
	if (sched_setscheduler(pid, SCHED_FIFO, &par)) {
		printf("[FAIL] want to set priority %d\n", priority);
		exit(1);
	}
}

void StartRunProcess(struct Process *now) {
	pid_t pid = fork();
	assert(pid >= 0);

	if (pid == 0) {
		struct timespec st, ed;
		syscall(333, &st);
		for (int i = 0; i < now->t; ++ i) UNIT_TIME;
		syscall(333, &ed);
		syscall(334, getpid(), st.tv_sec, st.tv_nsec, ed.tv_sec, ed.tv_nsec);
		exit(0);
	}
	else {
		now->pid = pid;
		now->isrun = 0;
		now->isfork = 1;
		SetPidCPU(now->pid, 1);
		SetPidPriority(now->pid, 1);
		printf("%s %d\n", now->name, (int)now->pid);
		fflush(stdout);
	}
}

void SchedulerFIFO(struct Process *ps, int n) {
	int run_id = 0, next_job = 0;
	for (int times = 0; ; ++ times) {
		if (ps[run_id].isrun == 1 && ps[run_id].t == 0) {
			ps[run_id].isrun = 0;
			wait(0);
			++ run_id;
			if (run_id == n) return;
		}

		while (next_job < n && ps[next_job].s <= times) {
			StartRunProcess(&ps[next_job]), ++ next_job;
		}

		if (ps[run_id].isrun == 0 && ps[run_id].isfork) {
			SetPidPriority(ps[run_id].pid, 99);
			ps[run_id].isrun = 1;
		}

		UNIT_TIME;
		if (ps[run_id].isrun) --ps[run_id].t;
	}
}

void SchedulerRR(struct Process *ps, int n) {
	int rrcnt = 0, next_job = 0;
	int queue[n], q_head = -1, q_tail = -1;
	memset(queue, -1, sizeof queue);
	for (int times = 0; ; ++ times) {
		if (q_head != -1 && ps[q_head].t == 0) {
			ps[q_head].isrun = 0, wait(0);
			rrcnt = 0;

			if (q_head != q_tail) q_head = queue[q_head];
			else q_head = q_tail = -1;
			if (q_head == -1 && next_job == n) return;
		}

		while (next_job < n && ps[next_job].s <= times) {
			StartRunProcess(&ps[next_job]);
			if (q_head == -1) q_head = q_tail = next_job;
			else queue[q_tail] = next_job, q_tail = next_job;
			++ next_job;
		}

		if (rrcnt == RR_ROUND && q_head != -1) {
			rrcnt = 0;
			SetPidPriority(ps[q_head].pid, 1);
			ps[q_head].isrun = 0;
			if (queue[q_head] != -1) {
				queue[q_tail] = q_head;
				q_tail = queue[q_tail];
				int org = q_head;
				q_head = queue[q_head];
				queue[org] = -1;
			}
		}

		if (q_head != -1 && ps[q_head].isrun == 0) {
			SetPidPriority(ps[q_head].pid, 99);
			ps[q_head].isrun = 1;
		}

		UNIT_TIME;
		if (q_head != -1 && ps[q_head].isrun == 1)
			++ rrcnt, -- ps[q_head].t;
	}
}

void SchedulerSJF(struct Process *ps, int n) {
	int run_id = -1, next_job = 0, finish_cnt = 0;
	
	int status[n]; // -1: finish, 0: unbegin, 1: ready
	memset(status, 0, sizeof status);

	for (int times = 0; ; ++ times) {
		if (run_id != -1 && ps[run_id].isrun != 0 && ps[run_id].t == 0) {
			ps[run_id].isrun = 0, wait(0);
			status[run_id] = -1;
			++ finish_cnt;
			if (finish_cnt == n) return;
		}

		while (next_job < n && ps[next_job].s <= times) {
			status[next_job] = 1, ++ next_job;
		}

		if (run_id == -1 || ps[run_id].isrun == 0) {
			run_id = -1;
			for (int i = 0; i < n; ++ i) 
				if (status[i] == 1 && (run_id == -1 || ps[i].t < ps[run_id].t)) run_id = i;
			if (run_id != -1) {
				StartRunProcess(&ps[run_id]);
				SetPidPriority(ps[run_id].pid, 99);
				ps[run_id].isrun = 1;
			}
		}

		UNIT_TIME;
		if (run_id != -1 && ps[run_id].isrun) -- ps[run_id].t;
	}
}

void SchedulerPSJF(struct Process *ps, int n) {
	int run_id = -1, next_job = 0, finish_cnt = 0;
	
	int status[n]; // -1: finish, 0: unbegin, 1: ready
	memset(status, 0, sizeof status);

	for (int times = 0; ; ++ times) {
		if (run_id != -1 && ps[run_id].isrun != 0 && ps[run_id].t == 0) {
			ps[run_id].isrun = 0, wait(0);
			status[run_id] = -1;
			++ finish_cnt;
			if (finish_cnt == n) return;
		}
		while (next_job < n && ps[next_job].s <= times) {
			status[next_job] = 1;
			if (run_id != -1 && ps[run_id].isrun != 0 && ps[next_job].t < ps[run_id].t) {
				SetPidPriority(ps[run_id].pid, 1);
				ps[run_id].isrun = 0;
				run_id = next_job;
				StartRunProcess(&ps[run_id]);
				SetPidPriority(ps[run_id].pid, 99);
				ps[run_id].isrun = 1;
			}
			++ next_job;
		}

		if (run_id == -1 || ps[run_id].isrun == 0) {
			run_id = -1;
			for (int i = 0; i < n; ++ i) 
				if (status[i] == 1 && (run_id == -1 || ps[i].t < ps[run_id].t)) run_id = i;
			if (run_id != -1) {
				if (ps[run_id].isfork == 0) 
					StartRunProcess(&ps[run_id]);
				SetPidPriority(ps[run_id].pid, 99);
				ps[run_id].isrun = 1;
			}
		}

		UNIT_TIME;
		if (run_id != -1 && ps[run_id].isrun) -- ps[run_id].t;
	}
}
