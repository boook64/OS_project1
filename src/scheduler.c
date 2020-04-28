#include "scheduler.h"

int CmpReadyTime(const void *_v1, const void *_v2) {
	struct Process *v1 = (struct Process *)_v1;
	struct Process *v2 = (struct Process *)_v2;
	if (v1->s != v2->s) 
		return (v1->s > v2->s) - (v1->s < v2->s);
	return (v1->idx > v2->idx) - (v1->idx < v2->idx);
}

int main() {

	char policy_type[64];
	int n;
	
	assert(scanf("%s", policy_type) == 1);
	assert(scanf("%d", &n) == 1);

	struct Process ps[n];
	for (int i = 0; i < n; ++ i) {
		assert(scanf("%s%d%d", ps[i].name, &ps[i].s, &ps[i].t) == 3);
		ps[i].idx = i;
		ps[i].isrun = 0;
		ps[i].isfork = 0;
	}

	qsort((void *)ps, n, sizeof(struct Process), CmpReadyTime);
	SetPidCPU(getpid(), 0);

	if (strcmp(policy_type, "FIFO") == 0)
		SchedulerFIFO(ps, n);
	else if (strcmp(policy_type, "RR") == 0)
		SchedulerRR(ps, n);
	else if (strcmp(policy_type, "SJF") == 0)
		SchedulerSJF(ps, n);
	else if (strcmp(policy_type, "PSJF") == 0)
		SchedulerPSJF(ps, n);
	else {
		fprintf(stderr, "[Unknown policy_type] %s\n", policy_type), exit(-1);
	}
	return 0;
}