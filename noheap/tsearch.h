#include <features.h>

/* AVL tree height < 1.44*log2(tree_nodes+2)-0.3, MAXH is a safe upper bound.  */
#define MAXH (sizeof(void*)*8*3/2)

struct tree_node {
	const void *key;
	void *a[2];
	int h;
};

int __tsearch_balance(void **);
