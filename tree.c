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

void *tree_root = NULL;

void treemakefunc()
{
    //void *tree_root = malloc(sizeof(void *));
    //tree_root = NULL;
    push((DCLANG_UINT) &tree_root);
}

void tree_free_func(void *tree_data)
{
    struct tree_entry *td = tree_data;
    if(!td) {
        return;
    }
    free(tree_data);
    return;
}

int tree_compare_func(const void *l, const void *r)
{
    struct tree_entry *tree_l = (struct tree_entry *)l;
    struct tree_entry *tree_r = (struct tree_entry *)r;
    //printf("tree_l->key is %s\n", tree_l->key);
    //printf("tree_r->key is %s\n", tree_r->key);
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
    void **tree_root = (void **)(DCLANG_UINT)pop();
    //void *root = *tree_root;
    char *search_key = (char *)(DCLANG_UINT) pop();
    //printf("about to make tree entry\n");
    struct tree_entry *te = make_tree_entry(strdup(search_key), 0);
    //printf("about to do tfind\n");
    struct tree_entry *retval = tfind(te, tree_root, tree_compare_func);
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
    void **tree_root = (void **)(DCLANG_UINT) pop();
    //void *root = *tree_root;
    char *search_key = (char *)(DCLANG_UINT) pop();
    DCLANG_FLT value = pop();
    //printf("about to make tree entry\n");
    struct tree_entry *te = make_tree_entry(strdup(search_key), value);
    //printf("about to do tsearch\n");
    struct tree_entry *retval = tsearch(te, tree_root, tree_compare_func);
    push((DCLANG_FLT)((*(struct tree_entry **)retval)->value));
}

void walk_entry(const void *tree_data, VISIT x, int level)
{
    struct tree_entry *node = *(struct tree_entry **)tree_data;
    printf("<%d>Walk on node %s %u %s  \n",
        level,
        x == preorder?"preorder":
        x == postorder?"postorder":
        x == endorder?"endorder":
        x == leaf?"leaf":
        "unknown",
        node->key, node->value);
    return;
}
