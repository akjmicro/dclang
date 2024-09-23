#include <features.h>

/* AVL tree height < 1.44*log2(tree_nodes+2)-0.3, MAXH is a safe upper bound.  */
#define MAXH (sizeof(void*)*8*3/2)

struct tree_node {
	const void *key;
	void *a[2];
	int h;
};

int __tsearch_balance(void **);

typedef enum { preorder, postorder, endorder, leaf } VISIT;

void *tdelete(const void *__restrict, void **__restrict, int(*)(const void *, const void *));
void *tfind(const void *, void *const *, int(*)(const void *, const void *));
void *tsearch(const void *, void **, int (*)(const void *, const void *));
void twalk(const void *, void (*)(const void *, VISIT, int));

#ifdef _GNU_SOURCE
void tdestroy(void *, void (*)(void *));
#endif
