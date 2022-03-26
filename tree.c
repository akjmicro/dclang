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
        (struct tree_entry *)calloc(sizeof(struct tree_entry), 1);
    if(!new_tree) {
        printf("make_tree_entry calloc fail\n");
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
        printf("treemake -- tree root limit reached!\n");
        printf("You can change the limit when compiling via the NUM_TREE_ROOTS variable\n");
        return;
    }
    push((DCLANG_UINT) tree_roots_idx);
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
        printf("tree@ -- stack underflow! Need <keystr> <rootnode_addr> on the stack.\n");
        printf("You can make a root node via 'treemake', and assign it to a variable ");
        printf("so it can be referred to later.\n");
        return;
    }
    DCLANG_UINT tree_idx = (DCLANG_UINT) dclang_pop();
    char *search_key = (char *)(DCLANG_UINT) dclang_pop();
    struct tree_entry *te = make_tree_entry(strdup(search_key), 0);
    struct tree_entry *retval = tfind(te, &tree_roots[tree_idx], tree_compare_func);
    if (retval == NULL)
    {
        push((DCLANG_UINT) 0);
        return;
    }
    push((DCLANG_FLT)((*(struct tree_entry **)retval)->value));
}

void treesetfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("tree! -- stack underflow! Need <val> <keystr> <rootnode_addr> on the stack.\n");
        printf("You can make a root node via 'treemake', and assign it to a variable ");
        printf("so it can be referred to later.\n");
        return;
    }
    DCLANG_UINT tree_idx = (DCLANG_UINT) dclang_pop();
    char *search_key = (char *)(DCLANG_UINT) dclang_pop();
    DCLANG_FLT value = dclang_pop();
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
		        (char *)(DCLANG_UINT)((*(struct tree_entry **)node)->value)
		    );
    }
}

void treewalkfunc()
{
    if (data_stack_ptr < 1) {
        printf("treewalk -- stack underflow! Need <tree_index> on the stack.\n");
        return;
    }
    DCLANG_UINT tree_idx = (DCLANG_UINT) dclang_pop();
    twalk(tree_roots[tree_idx], print_node);
}

void treedelfunc()
{
    if (data_stack_ptr < 2) {
        printf("treedel -- stack underflow! Need <key> <tree_index> on the stack.\n");
        return;
    }
    DCLANG_UINT tree_idx = (DCLANG_UINT) dclang_pop();
    char *key = (char *)(DCLANG_UINT) dclang_pop();
    struct tree_entry *te_del = make_tree_entry(strdup(key), 0);
    tdelete(te_del, &tree_roots[tree_idx], tree_compare_func);
    free(te_del);
}

void treedestroyfunc()
{
    if (data_stack_ptr < 1) {
        printf("treedestroy -- stack underflow! Need <tree_index> on the stack.\n");
        return;
    }
    DCLANG_UINT tree_idx = (DCLANG_UINT) dclang_pop();
    tdestroy(tree_roots[tree_idx], free);
    tree_roots[tree_idx] = NULL;
}
