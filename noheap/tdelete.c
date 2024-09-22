void *tdelete(const void *restrict key, void **restrict rootp,
	int(*cmp)(const void *, const void *))
{
	if (!rootp)
		return 0;

	void **a[MAXH+1];
	struct tree_node *n = *rootp;
	struct tree_node *parent;
	struct tree_node *child;
	int i=0;
	/* *a[0] is an arbitrary non-null pointer that is returned when
	   the root tree_node is deleted.  */
	a[i++] = rootp;
	a[i++] = rootp;
	for (;;) {
		if (!n)
			return 0;
		int c = cmp(key, n->key);
		if (!c)
			break;
		a[i++] = &n->a[c>0];
		n = n->a[c>0];
	}
	parent = *a[i-2];
	if (n->a[0]) {
		/* free the preceding tree_node instead of the deleted one.  */
		struct tree_node *deleted = n;
		a[i++] = &n->a[0];
		n = n->a[0];
		while (n->a[1]) {
			a[i++] = &n->a[1];
			n = n->a[1];
		}
		deleted->key = n->key;
		child = n->a[0];
	} else {
		child = n->a[1];
	}
	/* freed tree_node has at most one child, move it up and rebalance.  */
	dclang_free(n);
	*a[--i] = child;
	while (--i && __tsearch_balance(a[i]));
	return parent;
}
