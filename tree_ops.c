/* main stuct used for a dictionary-style tree entry.
   'key' is a string, 'value' is a double, which can represent
   either a number or a string pointer.

   It is up to the user to supply checks and use the types
   safely.
*/
struct tree_entry
{
    char *key;
    DCLANG_FLT value;
};

struct tree_entry *
make_tree_entry(char *key, DCLANG_FLT value)
{
    struct tree_entry *new_tree =
        (struct tree_entry *) dclang_malloc(sizeof(struct tree_entry));
    if(!new_tree) {
        printf("make_tree_entry malloc fail\n");
        exit(1);
    }
    new_tree->key = strdup(key);
    new_tree->value = value;
    return new_tree;
}

void *tree_roots[NUM_TREE_ROOTS] = {NULL};
int tree_roots_idx = -1;

void treemakefunc()
{
    tree_roots_idx += 1;
    if (tree_roots_idx > (NUM_TREE_ROOTS - 1))
    {
        printf("tmake -- tree root limit reached!\n");
        printf("You can change the limit when compiling via the NUM_TREE_ROOTS variable\n");
        return;
    }
    push((DCLANG_PTR) tree_roots_idx);
}

int tree_compare_func(const void *l, const void *r)
{
    if (l == NULL || r == NULL) return 0;
    struct tree_entry *tree_l = (struct tree_entry *)l;
    struct tree_entry *tree_r = (struct tree_entry *)r;
    return strcmp(tree_l->key, tree_r->key);
}

void treegetfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("t@ -- stack underflow! Need <tree_root> <keystr> on the stack.\n");
        printf("You can make a root node via 'tmake', and assign it to a variable ");
        printf("so it can be referred to later.\n");
        return;
    }

    // Pop args
    char *search_key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();
    struct tree_entry dummy_tree;
    dummy_tree.key = search_key;
    dummy_tree.value = 0;
    struct tree_entry *retval = tfind(&dummy_tree, &tree_roots[tree_idx], tree_compare_func);
    if (retval == NULL)
    {
        push((DCLANG_PTR) 0);
        return;
    }
    push((DCLANG_FLT)((*(struct tree_entry **)retval)->value));
}

void treesetfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("t! -- stack underflow! Need <tree_root> <keystr> <val> on the stack.\n");
        printf("You can make a root node via 'tmake', and assign it to a variable ");
        printf("so it can be referred to later.\n");
        return;
    }

    // Pop args
    DCLANG_FLT value = dclang_pop();
    char *search_key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();

    struct tree_entry *te_del = make_tree_entry(strdup(search_key), value);
    tdelete(te_del, &tree_roots[tree_idx], tree_compare_func);
    struct tree_entry *te = make_tree_entry(strdup(search_key), value);
    struct tree_entry *retval = tsearch(te, &tree_roots[tree_idx], tree_compare_func);
    push((DCLANG_FLT)((*(struct tree_entry **)retval)->value));
}

// helper used by `treewalk`
void print_node(const void *node, const VISIT order, const int depth)
{
    if (order == preorder || order == leaf ) {
		    printf(
		        "key=%s, value=%s\n",
		        (*(struct tree_entry **)node)->key,
		        (char *)(DCLANG_PTR)((*(struct tree_entry **)node)->value)
		    );
    }
}

void treewalkfunc()
{
    if (data_stack_ptr < 1) {
        printf("twalk -- stack underflow! Need <tree_root> on the stack.\n");
        return;
    }
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();
    twalk(tree_roots[tree_idx], print_node);
}

void treedeletefunc()
{
    if (data_stack_ptr < 2) {
        printf("tdel -- stack underflow! Need <tree_root> <key> on the stack.\n");
        return;
    }

    // Pop args
    char *key = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();

    struct tree_entry *te_del = make_tree_entry(strdup(key), 0);
    tdelete(te_del, &tree_roots[tree_idx], tree_compare_func);
    dclang_free(te_del);
}

#ifdef HAS_TREEDESTROY
void treedestroyfunc()
{
    if (data_stack_ptr < 1) {
        printf("tdestroy -- stack underflow! Need <tree_root> on the stack.\n");
        return;
    }
    DCLANG_PTR tree_idx = (DCLANG_PTR) dclang_pop();
    tdestroy(tree_roots[tree_idx], dclang_free);
    tree_roots[tree_idx] = NULL;
}
#endif
