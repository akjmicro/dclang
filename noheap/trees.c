#include "trees.h"

static inline int height(struct tree_node *n) { return n ? n->h : 0; }

static int rot(void **p, struct tree_node *x, int dir /* deeper side */)
{
    struct tree_node *y = x->a[dir];
    struct tree_node *z = y->a[!dir];
    int hx = x->h;
    int hz = height(z);
    if (hz > height(y->a[dir])) {
        /*
         *   x
         *  / \ dir          z
         * A   y            / \
         *    / \   -->    x   y
         *   z   D        /|   |\
         *  / \          A B   C D
         * B   C
         */
        x->a[dir] = z->a[!dir];
        y->a[!dir] = z->a[dir];
        z->a[!dir] = x;
        z->a[dir] = y;
        x->h = hz;
        y->h = hz;
        z->h = hz+1;
    } else {
        /*
         *   x               y
         *  / \             / \
         * A   y    -->    x   D
         *    / \         / \
         *   z   D       A   z
         */
        x->a[dir] = z;
        y->a[!dir] = x;
        x->h = hz+1;
        y->h = hz+2;
        z = y;
    }
    *p = z;
    return z->h - hx;
}

/* balance *p, return 0 if height is unchanged.  */
int __tsearch_balance(void **p)
{
    struct tree_node *n = *p;
    int h0 = height(n->a[0]);
    int h1 = height(n->a[1]);
    if (h0 - h1 + 1u < 3u) {
        int old = n->h;
        n->h = h0<h1 ? h1+1 : h0+1;
        return n->h - old;
    }
    return rot(p, n, h0<h1);
}

void *tsearch(const void *key, void **rootp,
    int (*cmp)(const void *, const void *))
{
    if (!rootp)
        return 0;

    void **a[MAXH];
    struct tree_node *n = *rootp;
    struct tree_node *r;
    int i=0;
    a[i++] = rootp;
    for (;;) {
        if (!n)
            break;
        int c = cmp(key, n->key);
        if (!c)
            return n;
        a[i++] = &n->a[c>0];
        n = n->a[c>0];
    }
    r = dclang_malloc(sizeof *r);
    if (!r)
        return 0;
    r->key = key;
    r->a[0] = r->a[1] = 0;
    r->h = 1;
    /* insert new tree_node, rebalance ancestors.  */
    *a[--i] = r;
    while (i && __tsearch_balance(a[--i]));
    return r;
}

void *tfind(const void *key, void *const *rootp,
    int(*cmp)(const void *, const void *))
{
    if (!rootp)
        return 0;

    struct tree_node *n = *rootp;
    for (;;) {
        if (!n)
            break;
        int c = cmp(key, n->key);
        if (!c)
            break;
        n = n->a[c>0];
    }
    return n;
}

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
