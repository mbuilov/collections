#ifndef PRBTREE_H_INCLUDED
#define PRBTREE_H_INCLUDED

/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2012-2018 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* prbtree.h */

#include "btree.h"

/* declaration for exported functions, such as:
  __declspec(dllexport)/__declspec(dllimport) or __attribute__((visibility("default"))) */
#ifndef PRBTREE_EXPORTS
#define PRBTREE_EXPORTS
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define A_Restrict restrict
#elif defined(_MSC_VER) && (_MSC_VER >= 1600)
#define A_Restrict __restrict
#elif defined(__GNUC__) && (__GNUC__ >= 3)
#define A_Restrict __restrict__
#elif defined(__clang__)
#define A_Restrict __restrict__
#else
#define A_Restrict
#endif
#endif /* !SAL_DEFS_H_INCLUDED */

#ifndef PRBTREE_ASSERT
#ifdef ASSERT
#define PRBTREE_ASSERT(expr) ASSERT(expr)
#else
#define PRBTREE_ASSERT(expr) ((void)0)
#endif
#endif

#ifndef PRBTREE_ASSERT_PTRS
#define PRBTREE_ASSERT_PTRS(expr) PRBTREE_ASSERT(expr)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Embedded binary tree:
  one object may encapsulate multiple tree nodes - to reference it from multiple trees */

/* NOTE: address of this structure must be aligned on at least 2 bytes:
  lowest bit of parent_color pointer encodes node color */
struct prbtree_node {
	union {
		struct btree_node n;
		struct prbtree_node *leaves[2]; /* left, right */
	} u;
	void *parent_color; /* parent pointer + red/black color, NULL for root node */
};

/* left/right leaves */
#define prbtree_left  u.leaves[0]
#define prbtree_right u.leaves[1]

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L

#if defined(__GNUC__) && (__GNUC__ >= 5)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-c11-compat" /* warning: ISO C99 does not support '_Alignof' */
#endif

typedef int prbtree_node_check_alignment_t[1-2*(_Alignof(struct prbtree_node) < 2)];

#if defined(__GNUC__) && (__GNUC__ >= 5)
#pragma GCC diagnostic pop
#endif

#elif defined __cplusplus && __cplusplus >= 201103L
typedef int prbtree_node_check_alignment_t[1-2*(alignof(struct prbtree_node) < 2)];
#elif defined __GNUC__
typedef int prbtree_node_check_alignment_t[1-2*(__alignof__(struct prbtree_node) < 2)];
#elif defined _MSC_VER
typedef int prbtree_node_check_alignment_t[1-2*(__alignof(struct prbtree_node) < 2)];
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_At(n, A_In_opt)
A_Check_return
A_Ret_range(==,n)
#endif
static inline struct prbtree_node *prbtree_node_from_btree_node_(
	const struct btree_node *n/*NULL?*/)
{
	void *p = btree_const_cast(n/*NULL?*/);
	return (struct prbtree_node*)p;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_At(n, A_In_opt)
A_Check_return
A_Ret_range(==,pn)
#endif
static inline struct btree_node *prbtree_node_to_btree_node_(
	const struct prbtree_node *pn/*NULL?*/)
{
	const void *p = pn;
	return btree_const_cast((const struct btree_node*)p/*NULL?*/);
}

/* tree - just a pointer to the root node */
struct prbtree {
	struct prbtree_node *root; /* NULL if tree is empty */
};

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(tree, A_Out)
#endif
static inline void prbtree_init(
	struct prbtree *tree/*!=NULL,out*/)
{
	PRBTREE_ASSERT(tree);
	tree->root = (struct prbtree_node*)0;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(e, A_Out)
#endif
static inline void prbtree_init_node(
	struct prbtree_node *e/*!=NULL,out*/)
{
	PRBTREE_ASSERT(e);
	e->prbtree_left  = (struct prbtree_node*)0;
	e->prbtree_right = (struct prbtree_node*)0;
	e->parent_color  = (void*)0;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(e, A_In)
A_Pre_satisfies(!e->prbtree_left)
A_Pre_satisfies(!e->prbtree_right)
A_Pre_satisfies(!e->parent_color)
#endif
static inline void prbtree_check_new_node(
	const struct prbtree_node *e/*!=NULL*/)
{
	PRBTREE_ASSERT(!e->prbtree_left);
	PRBTREE_ASSERT(!e->prbtree_right);
	PRBTREE_ASSERT(!e->parent_color);
	(void)e;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Const_function
A_At(n, A_In)
A_Ret_maybenull
A_Check_return
#endif
static inline struct prbtree_node *prbtree_get_parent(
	const struct prbtree_node *n/*!=NULL*/)
{
	PRBTREE_ASSERT(n);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#endif
	return (struct prbtree_node*)(~1llu & (unsigned long long)n->parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_Ret_range(0,1)
A_Check_return
#endif
static inline unsigned prbtree_get_color_1(
	const void *parent_color/*NULL?*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#endif
	return (unsigned)(1llu & (unsigned long long)parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Const_function
A_At(n, A_In)
A_Ret_range(0,1)
A_Check_return
#endif
static inline unsigned prbtree_get_color_(
	const struct prbtree_node *n/*!=NULL*/)
{
	PRBTREE_ASSERT(n);
	return prbtree_get_color_1(n->parent_color);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Const_function
A_At(n, A_In)
A_Ret_range(==,n->parent_color)
A_Ret_maybenull
A_Check_return
#endif
static inline struct prbtree_node *prbtree_black_node_parent_(
	struct prbtree_node *n/*!=NULL*/)
{
	PRBTREE_ASSERT(n);
	PRBTREE_ASSERT(!prbtree_get_color_(n));
	return (struct prbtree_node*)n->parent_color;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Const_function
A_At(p, A_In)
A_At(c, A_In_range(0,1))
A_Check_return
#endif
static inline void *prbtree_make_parent_color_(
	struct prbtree_node *p/*!=NULL*/,
	unsigned c)
{
	PRBTREE_ASSERT(p);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#endif
	return (void*)((unsigned long long)p | c);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(tree, A_Inout)
A_At(p, A_Inout)
A_At(e, A_Inout)
#endif
PRBTREE_EXPORTS void prbtree_rebalance(
	struct prbtree *tree/*!=NULL*/,
	struct prbtree_node *A_Restrict p/*!=NULL*/,
	struct prbtree_node *A_Restrict e/*!=NULL*/);

/* insert new node into the tree,
  c - result of btree_search_parent():
  c  < 0: parent key  < e's key, insert e at right of the parent;
  c >= 0: parent key >= e's key, insert e at left of the parent */
/* if p is NULL, assume the tree is empty - e becomes the root node */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
A_Nonnull_arg(3)
A_At(tree, A_Inout)
A_At(p, A_Inout_opt)
A_At(e, A_Inout)
#endif
static inline void prbtree_insert(
	struct prbtree *tree/*!=NULL*/,
	struct prbtree_node *A_Restrict p/*NULL?*/,
	struct prbtree_node *A_Restrict e/*!=NULL*/,
	int c)
{
	PRBTREE_ASSERT(tree);
	PRBTREE_ASSERT(e);
	PRBTREE_ASSERT_PTRS(p != e);
	prbtree_check_new_node(e); /* new node must have NULL children and parent */
	if (p) {
		PRBTREE_ASSERT(!p->u.leaves[c < 0]);
		p->u.leaves[c < 0] = e;
		prbtree_rebalance(tree, p, e);
	}
	else {
		PRBTREE_ASSERT(!tree->root);
		tree->root = e; /* black node */
	}
}

/* replace old node in the tree with a new one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(n, A_Out)
A_At(o, A_In)
A_At(e, A_Out)
#endif
static inline void prbtree_replace_(
	struct prbtree_node **A_Restrict n/*!=NULL,out*/,
	const struct prbtree_node *A_Restrict o/*!=NULL,in*/,
	struct prbtree_node *A_Restrict e/*!=NULL,out*/)
{
	PRBTREE_ASSERT(n);
	PRBTREE_ASSERT(o);
	PRBTREE_ASSERT(e);
	PRBTREE_ASSERT_PTRS(o != e);
	{
		struct prbtree_node **A_Restrict el = &e->prbtree_left;
		struct prbtree_node **A_Restrict er = &e->prbtree_right;
		struct prbtree_node *A_Restrict l = o->prbtree_left;
		struct prbtree_node *A_Restrict r = o->prbtree_right;
		void *p = o->parent_color;
		PRBTREE_ASSERT_PTRS(o != l);
		PRBTREE_ASSERT_PTRS(o != r);
		PRBTREE_ASSERT_PTRS(e != l);
		PRBTREE_ASSERT_PTRS(e != r);
		PRBTREE_ASSERT_PTRS(n != el);
		PRBTREE_ASSERT_PTRS(n != er);
		PRBTREE_ASSERT_PTRS(el != er);
		*n = e;
		*el = l;
		*er = r;
		e->parent_color = p;
		if (l) {
			PRBTREE_ASSERT_PTRS(l != r);
			l->parent_color = prbtree_make_parent_color_(e, prbtree_get_color_(l));
		}
		if (r) {
			PRBTREE_ASSERT_PTRS(l != r);
			r->parent_color = prbtree_make_parent_color_(e, prbtree_get_color_(r));
		}
	}
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
A_Nonnull_arg(3)
A_At(tree, A_Inout)
A_At(p, A_Inout_opt)
A_At(o, A_In)
A_Ret_never_null
A_Check_return
#endif
static inline struct prbtree_node **prbtree_slot_at_parent_(
	struct prbtree *tree/*!=NULL*/,
	struct prbtree_node *A_Restrict p/*NULL?*/,
	const struct prbtree_node *A_Restrict o/*!=NULL*/)
{
	PRBTREE_ASSERT(tree);
	PRBTREE_ASSERT(o);
	PRBTREE_ASSERT_PTRS(p != o);
	return p ? &p->u.leaves[o != p->u.leaves[0]] : &tree->root;
}

/* replace old node in the tree with a new one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(tree, A_Inout)
A_At(o, A_In)
A_At(e, A_Out)
#endif
static inline void prbtree_replace(
	struct prbtree *tree/*!=NULL*/,
	const struct prbtree_node *A_Restrict o/*!=NULL*/,
	struct prbtree_node *A_Restrict e/*!=NULL,out*/)
{
	PRBTREE_ASSERT(tree);
	PRBTREE_ASSERT(o);
	PRBTREE_ASSERT(e);
	PRBTREE_ASSERT_PTRS(o != e);
	prbtree_replace_(prbtree_slot_at_parent_(tree, prbtree_get_parent(o), o), o, e);
}

/* remove node from the tree */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(tree, A_Inout)
A_At(e, A_Inout)
#endif
PRBTREE_EXPORTS void prbtree_remove(
	struct prbtree *tree/*!=NULL*/,
	struct prbtree_node *A_Restrict e/*!=NULL*/);

/* non-recursive iteration over nodes of the tree */

/* find right parent */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Pure_function
A_At(current, A_In)
A_Ret_maybenull
A_Check_return
#endif
static inline struct prbtree_node *prbtree_right_parent(
	const struct prbtree_node *current/*!=NULL*/)
{
	PRBTREE_ASSERT(current);
	for (;;) {
		struct prbtree_node *p = prbtree_get_parent(current);
		if (!p || current == p->prbtree_left)
			return p; /* NULL? */
		current = p;
	}
}

/* find left parent */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Pure_function
A_At(current, A_In)
A_Ret_maybenull
A_Check_return
#endif
static inline struct prbtree_node *prbtree_left_parent(
	const struct prbtree_node *current/*!=NULL*/)
{
	PRBTREE_ASSERT(current);
	for (;;) {
		struct prbtree_node *p = prbtree_get_parent(current);
		if (!p || current == p->prbtree_right)
			return p; /* NULL? */
		current = p;
	}
}

/* get next node, returns NULL for the rightmost node */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Pure_function
A_At(current, A_In)
A_Ret_maybenull
A_Check_return
#endif
static inline struct prbtree_node *prbtree_next(
	const struct prbtree_node *current/*!=NULL*/)
{
	PRBTREE_ASSERT(current);
	{
		const struct prbtree_node *n = current->prbtree_right;
		if (n)
			return prbtree_node_from_btree_node_(btree_first(&n->u.n)); /* != NULL */
	}
	return prbtree_right_parent(current); /* NULL? */
}

/* get previous node, returns NULL for the leftmost node */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Pure_function
A_At(current, A_In)
A_Ret_maybenull
A_Check_return
#endif
static inline struct prbtree_node *prbtree_prev(
	const struct prbtree_node *current/*!=NULL*/)
{
	PRBTREE_ASSERT(current);
	{
		const struct prbtree_node *p = current->prbtree_left;
		if (p)
			return prbtree_node_from_btree_node_(btree_last(&p->u.n)); /* != NULL */
	}
	return prbtree_left_parent(current); /* NULL? */
}

#ifdef __cplusplus
}
#endif

#undef PRBTREE_ASSERT_PTRS
#undef PRBTREE_ASSERT

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#undef A_Restrict
#endif

#endif /* PRBTREE_H_INCLUDED */
