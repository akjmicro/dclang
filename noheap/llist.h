#ifndef _LLIST_H
#define _LLIST_H

void insque(void *, void *);
void remque(void *);

#ifdef _GNU_SOURCE
struct qelem {
	struct qelem *q_forw, *q_back;
	char q_data[1];
};
#endif
