#define _GNU_SOURCE

void tdestroy(void *root, void (*freekey)(void *))
{
	struct tree_node *r = root;

	if (r == 0)
		return;
	tdestroy(r->a[0], freekey);
	tdestroy(r->a[1], freekey);
	if (freekey) freekey((void *)r->key);
	dclang_free(r);
}
