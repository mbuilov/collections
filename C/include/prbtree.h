#ifndef PRBTREE_H_INCLUDED
#define PRBTREE_H_INCLUDED

/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2012-2018 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* prbtree.h */

#include "btree.h"

/* declaration for exported functions, such as __declspec(dllexport)/__declspec(dllimport) or __attribute__((visibility("default"))) */
#ifndef PRBTREE_EXPORTS
#define PRBTREE_EXPORTS
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Embedded binary tree:
  one object may encapsulate multiple tree nodes - to reference it from multiple trees */

/* NOTE: address of any object of this structure must be aligned on at least 2 bytes:
  lowest bit of parent_color pointer encodes node color */
struct prbtree_node {
	union {
		struct _btree_node n;
		struct prbtree_node *leaves[2]; /* left, right */
	} u;
	char *parent_color; /* parent pointer + red/black color, NULL for root node */
};

#define prbtree_left u.leaves[0]
#define prbtree_right u.leaves[1]

A_Const_function A_Check_return A_Ret_range(==,n)
static inline struct prbtree_node *_prbtree_node_from_btree_node(
	A_In_opt const struct _btree_node *n/*NULL?*/)
{
	union {
		const struct _btree_node *n;
		struct prbtree_node *pn;
	} u;
	u.n = n;
	return u.pn;
}

A_Const_function A_Check_return A_Ret_range(==,pn)
static inline struct _btree_node *_prbtree_node_to_btree_node(
	A_In_opt const struct prbtree_node *pn/*NULL?*/)
{
	union {
		const struct prbtree_node *pn;
		struct _btree_node *n;
	} u;
	u.pn = pn;
	return u.n;
}

struct _prbtree {
	struct prbtree_node *root; /* NULL if tree is empty */
};

A_Nonnull_all_args
static inline void _prbtree_init(A_Out struct _prbtree *tree)
{
	tree->root = (struct prbtree_node*)0;
}

A_Nonnull_all_args
static inline void _prbtree_init_node(A_Out struct prbtree_node *e)
{
	e->prbtree_left = (struct prbtree_node*)0;
	e->prbtree_right = (struct prbtree_node*)0;
	e->parent_color = (char*)0;
}

#ifdef ASSERT
A_Nonnull_all_args A_Pure_function
static inline void _prbtree_check_new_node(A_In const struct prbtree_node *e)
{
	ASSERT(!e->prbtree_left);
	ASSERT(!e->prbtree_right);
	ASSERT(!e->parent_color);
	(void)e;
}
#else
#define _prbtree_check_new_node(e) ((void)0)
#endif

A_Nonnull_all_args A_Const_function A_Check_return
static inline struct prbtree_node *PRB_GET_PARENT(A_In const struct prbtree_node *n)
{
	char *p = n->parent_color;
	return (struct prbtree_node*)(p - (unsigned)(1u & (p - (const char*)0)));
}

A_Const_function A_Check_return
static inline char *_PRB_MAKE_PARENT_COLOR(
	A_In_opt struct prbtree_node *p/*NULL?*/,
	A_In_range(0,1) unsigned c)
{
	return (char*)p + c;
}

/* store node color in lowest bit of parent pointer */
#define PRB_RED_COLOR 1u
#define PRB_BLACK_COLOR 0u
#define PRB_BLACK_NODE_PARENT(n)    ((struct prbtree_node*)(n)->parent_color)
#define PRB_GET_COLOR(n)            ((unsigned)(1u & ((n)->parent_color - (const char*)0)))
#define _PRB_SET_PARENT_COLOR(n,pc) ((n)->parent_color = (pc))
#define PRB_SET_PARENT_COLOR(n,p,c) _PRB_SET_PARENT_COLOR(n, _PRB_MAKE_PARENT_COLOR(p,c))

A_Nonnull_all_args
PRBTREE_EXPORTS void __prbtree_insert(
	A_In struct _prbtree *tree/*!=NULL*/,
	A_In struct prbtree_node *p/*!=NULL*/,
	A_In struct prbtree_node *e/*!=NULL*/);

/* insert new node into the tree */
/* c - result of _btree_search_parent():
  c  < 0: parent key  < e's key,
  c >= 0: parent key >= e's key */
A_Nonnull_arg(1)
A_Nonnull_arg(3)
static inline void _prbtree_insert(
	A_In struct _prbtree *tree/*!=NULL*/,
	A_In struct prbtree_node *A_Restrict p/*NULL?*/,
	A_In struct prbtree_node *A_Restrict e/*!=NULL*/,
	int c)
{
	_prbtree_check_new_node(e); /* new node must have NULL children and parent */
	if (p) {
		p->u.leaves[c < 0] = e;
		__prbtree_insert(tree, p, e);
	}
	else
		tree->root = e; /* black node */
}

A_Nonnull_arg(1)
A_Nonnull_arg(3)
A_Nonnull_arg(4)
static inline void __prbtree_replace_at_parent(
	A_In struct _prbtree *tree/*!=NULL*/,
	A_In_opt struct prbtree_node *A_Restrict p/*NULL?*/,
	A_In const struct prbtree_node *A_Restrict o/*!=NULL*/,
	A_In struct prbtree_node *A_Restrict e/*!=NULL*/)
{
	if (!p)
		tree->root = e;
	else
		p->u.leaves[o != p->prbtree_left] = e;
}

/* replace old node with a new one in the tree */
A_Nonnull_all_args
static inline void _prbtree_replace(
	A_In struct _prbtree *tree/*!=NULL*/,
	A_In const struct prbtree_node *A_Restrict o/*!=NULL*/,
	A_In struct prbtree_node *A_Restrict e/*!=NULL*/)
{
	__prbtree_replace_at_parent(tree, PRB_GET_PARENT(o), o, e);
	{
		struct prbtree_node *A_Restrict a = o->prbtree_left;
		if (a)
			PRB_SET_PARENT_COLOR(a, e, PRB_GET_COLOR(a));
	}
	{
		struct prbtree_node *A_Restrict b = o->prbtree_right;
		if (b)
			PRB_SET_PARENT_COLOR(b, e, PRB_GET_COLOR(b));
	}
	*e = *o;
}

/* remove node from the tree */
A_Nonnull_all_args
PRBTREE_EXPORTS void _prbtree_remove(
	A_In struct _prbtree *tree/*!=NULL*/,
	A_In struct prbtree_node *e/*!=NULL*/);

/* non-recursive iteration over nodes of the tree */

A_Nonnull_all_args A_Pure_function A_Check_return A_Ret_maybenull
static inline struct prbtree_node *_prbtree_right_parent(A_In const struct prbtree_node *current)
{
	for (;;) {
		struct prbtree_node *p = PRB_GET_PARENT(current);
		if (!p || p->prbtree_left == current)
			return p; /* NULL? */
		current = p;
	}
}

A_Nonnull_all_args A_Pure_function A_Check_return A_Ret_maybenull
static inline struct prbtree_node *_prbtree_left_parent(A_In const struct prbtree_node *current)
{
	for (;;) {
		struct prbtree_node *p = PRB_GET_PARENT(current);
		if (!p || p->prbtree_right == current)
			return p; /* NULL? */
		current = p;
	}
}

/* get next node, returns NULL for the rightmost node */
A_Nonnull_all_args A_Pure_function A_Check_return A_Ret_maybenull
static inline struct prbtree_node *_prbtree_next(A_In const struct prbtree_node *current)
{
	const struct prbtree_node *n = current->prbtree_right;
	if (n)
		return _prbtree_node_from_btree_node(_btree_first(&n->u.n)); /* != NULL */
	return _prbtree_right_parent(current); /* NULL? */
}

/* get previous node, returns NULL for the leftmost node */
A_Nonnull_all_args A_Pure_function A_Check_return A_Ret_maybenull
static inline struct prbtree_node *_prbtree_prev(A_In const struct prbtree_node *current)
{
	const struct prbtree_node *p = current->prbtree_left;
	if (p)
		return _prbtree_node_from_btree_node(_btree_last(&p->u.n)); /* != NULL */
	return _prbtree_left_parent(current); /* NULL? */
}

#ifdef __cplusplus
}
#endif

#endif /* PRBTREE_H_INCLUDED */
