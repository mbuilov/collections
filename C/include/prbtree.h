#ifndef PRBTREE_H_INCLUDED
#define PRBTREE_H_INCLUDED

/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2012-2022 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* prbtree.h */

#include "btree.h"

/* declaration for exported functions, such as:
  __declspec(dllexport)/__declspec(dllimport) or __attribute__((visibility("default"))) */
#ifndef PRBTREE_EXPORTS
#define PRBTREE_EXPORTS
#endif

/* PRBTREE_RESTRICT - annotates pointer pointing to memory that is not writable via other pointers */
#ifndef PRBTREE_RESTRICT
#ifdef A_Restrict
#define PRBTREE_RESTRICT A_Restrict
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define PRBTREE_RESTRICT restrict
#elif defined(_MSC_VER) && (_MSC_VER >= 1600)
#define PRBTREE_RESTRICT __restrict
#elif defined(__GNUC__) && (__GNUC__ >= 3)
#define PRBTREE_RESTRICT __restrict__
#elif defined(__clang__)
#define PRBTREE_RESTRICT __restrict__
#else
#define PRBTREE_RESTRICT
#endif
#endif

/* expr - do not compares pointers */
#ifndef PRBTREE_ASSERT
#define PRBTREE_ASSERT(expr) BTREE_ASSERT(expr)
#endif

/* check that pointer is not NULL */
#ifndef PRBTREE_ASSERT_PTR
#define PRBTREE_ASSERT_PTR(ptr) BTREE_ASSERT_PTR(ptr)
#endif

/* expr - may compare pointers for equality */
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

/* lowest bit of parent_color pointer encodes flags - make sure that pointer is properly aligned */
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

static inline struct prbtree_node *prbtree_node_from_btree_node_(
	const struct btree_node *const n/*NULL?*/)
{
	void *const x = btree_const_cast(n/*NULL?*/);
	return (struct prbtree_node*)x;
}

static inline struct btree_node *prbtree_node_to_btree_node_(
	const struct prbtree_node *const p/*NULL?*/)
{
	const void *const x = p;
	return btree_const_cast((const struct btree_node*)x/*NULL?*/);
}

/* tree - just a pointer to the root node */
struct prbtree {
	struct prbtree_node *root; /* NULL if tree is empty */
};

static inline void prbtree_init(
	struct prbtree *const tree/*!=NULL,out*/)
{
	PRBTREE_ASSERT_PTR(tree);
	tree->root = (struct prbtree_node*)0;
}

static inline void prbtree_init_node(
	struct prbtree_node *const e/*!=NULL,out*/)
{
	PRBTREE_ASSERT_PTR(e);
	e->prbtree_left  = (struct prbtree_node*)0;
	e->prbtree_right = (struct prbtree_node*)0;
	e->parent_color  = (void*)0;
}

static inline void prbtree_check_new_node(
	const struct prbtree_node *const e/*!=NULL*/)
{
	PRBTREE_ASSERT_PTR(e);
	PRBTREE_ASSERT(!e->prbtree_left);
	PRBTREE_ASSERT(!e->prbtree_right);
	PRBTREE_ASSERT(!e->parent_color);
}

static inline struct prbtree_node *prbtree_get_parent_(
	void *const parent_color/*NULL?*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#pragma warning(disable:4305) /* 'type cast': truncation from 'unsigned __int64' to 'void *' */
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast" /* warning: cast from pointer to integer of different size */
#endif
	return (struct prbtree_node*)((unsigned long long)parent_color & ~1llu &
		(((1llu << (8*sizeof(void*) - 1)) | ~(~0llu << (8*sizeof(void*) - 1))))) +
		0*sizeof(int[1-2*(255 != (unsigned char)-1)]) +
		0*sizeof(int[1-2*(sizeof(unsigned long long) < sizeof(void*))]);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}

static inline struct prbtree_node *prbtree_get_parent(
	const struct prbtree_node *const n/*!=NULL*/)
{
	PRBTREE_ASSERT_PTR(n);
	return prbtree_get_parent_(n->parent_color); /* NULL? */
}

/* clang complains about side-effects if prbtree_get_color_() is used in PRBTREE_ASSERT(), so use this define */
#define prbtree_get_color_2(parent_color) ((unsigned)(1llu & (unsigned long long)(parent_color)))

/* returns: 0 or 1 */
static inline unsigned prbtree_get_color_(
	const struct prbtree_node *const n/*!=NULL*/)
{
	PRBTREE_ASSERT_PTR(n);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	return prbtree_get_color_2(n->parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}

static inline struct prbtree_node *prbtree_black_node_parent_(
	struct prbtree_node *const n/*!=NULL*/)
{
	PRBTREE_ASSERT_PTR(n);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	PRBTREE_ASSERT(!prbtree_get_color_2(n->parent_color));
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
	return (struct prbtree_node*)n->parent_color; /* NULL? */
}

static inline void *prbtree_make_parent_color_(
	struct prbtree_node *const p/*NULL?*/,
	const unsigned c/*0,1*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#pragma warning(disable:4305) /* 'type cast': truncation from 'unsigned __int64' to 'void *' */
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast" /* warning: cast from pointer to integer of different size */
#endif
	PRBTREE_ASSERT(c <= 1);
	PRBTREE_ASSERT(!(1llu & (unsigned long long)p));
	return (char*)(((unsigned long long)p | c) &
		(((1llu << (8*sizeof(void*) - 1)) | ~(~0llu << (8*sizeof(void*) - 1))))) +
		0*sizeof(int[1-2*(255 != (unsigned char)-1)]) +
		0*sizeof(int[1-2*(sizeof(unsigned long long) < sizeof(void*))]);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}

PRBTREE_EXPORTS void prbtree_rebalance(
	struct prbtree *const tree/*!=NULL*/,
	struct prbtree_node *PRBTREE_RESTRICT p/*!=NULL*/,
	struct prbtree_node *PRBTREE_RESTRICT e/*!=NULL*/);

/* insert new node into the tree,
  c - result of btree_search_parent():
  c  < 0: parent key  < e's key, insert e at right of the parent;
  c >= 0: parent key >= e's key, insert e at left of the parent */
/* if p is NULL, assume the tree is empty - e becomes the root node */
#if 0 /* example */
  struct btree_node *parent = prbtree_node_to_btree_node_(tree->root); /* NULL? */
  int c = btree_search_parent(&parent, key, key_comparator, allow_duplicates);
  prbtree_insert(tree, prbtree_node_from_btree_node_(parent), node, c);
#endif
static inline void prbtree_insert(
	struct prbtree *const tree/*!=NULL*/,
	struct prbtree_node *PRBTREE_RESTRICT const p/*NULL?*/,
	struct prbtree_node *PRBTREE_RESTRICT const e/*!=NULL*/,
	int c)
{
	PRBTREE_ASSERT_PTR(tree);
	PRBTREE_ASSERT_PTR(e);
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
static inline void prbtree_replace_(
	struct prbtree_node **PRBTREE_RESTRICT const n/*!=NULL,out*/,
	const struct prbtree_node *PRBTREE_RESTRICT const o/*!=NULL,in*/,
	struct prbtree_node *PRBTREE_RESTRICT const e/*!=NULL,out*/)
{
	PRBTREE_ASSERT_PTR(n);
	PRBTREE_ASSERT_PTR(o);
	PRBTREE_ASSERT_PTR(e);
	PRBTREE_ASSERT_PTRS(o != e);
	{
		struct prbtree_node **PRBTREE_RESTRICT const el = &e->prbtree_left;
		struct prbtree_node **PRBTREE_RESTRICT const er = &e->prbtree_right;
		struct prbtree_node *PRBTREE_RESTRICT const l = o->prbtree_left;    /* NULL? */
		struct prbtree_node *PRBTREE_RESTRICT const r = o->prbtree_right;   /* NULL? */
		void *const p = o->parent_color;
		PRBTREE_ASSERT_PTRS(o != l);
		PRBTREE_ASSERT_PTRS(o != r);
		PRBTREE_ASSERT_PTRS(e != l);
		PRBTREE_ASSERT_PTRS(e != r);
		PRBTREE_ASSERT_PTRS((!l && !r) || (l != r));
		PRBTREE_ASSERT_PTRS(n != el);
		PRBTREE_ASSERT_PTRS(n != er);
		PRBTREE_ASSERT_PTRS(el != er);
		*n = e;
		*el = l;
		*er = r;
		e->parent_color = p;
		if (l)
			l->parent_color = prbtree_make_parent_color_(e, prbtree_get_color_(l));
		if (r)
			r->parent_color = prbtree_make_parent_color_(e, prbtree_get_color_(r));
	}
}

static inline struct prbtree_node **prbtree_slot_at_parent_(
	struct prbtree *const tree/*!=NULL*/,
	struct prbtree_node *PRBTREE_RESTRICT const p/*NULL?*/,
	const struct prbtree_node *PRBTREE_RESTRICT const o/*!=NULL*/)
{
	PRBTREE_ASSERT_PTR(tree);
	PRBTREE_ASSERT_PTR(o);
	PRBTREE_ASSERT_PTRS(p != o);
	return p ? &p->u.leaves[o != p->u.leaves[0]] : &tree->root;
}

/* replace old node in the tree with a new one */
static inline void prbtree_replace(
	struct prbtree *const tree/*!=NULL*/,
	const struct prbtree_node *PRBTREE_RESTRICT const o/*!=NULL*/,
	struct prbtree_node *PRBTREE_RESTRICT const e/*!=NULL,out*/)
{
	PRBTREE_ASSERT_PTR(tree);
	PRBTREE_ASSERT_PTR(o);
	PRBTREE_ASSERT_PTR(e);
	PRBTREE_ASSERT_PTRS(o != e);
	prbtree_replace_(prbtree_slot_at_parent_(tree, prbtree_get_parent(o), o), o, e);
}

/* remove node from the tree */
PRBTREE_EXPORTS void prbtree_remove(
	struct prbtree *const tree/*!=NULL*/,
	struct prbtree_node *PRBTREE_RESTRICT e/*!=NULL*/);

/* non-recursive iteration over nodes of the tree */

/* find right parent */
static inline struct prbtree_node *prbtree_right_parent(
	const struct prbtree_node *current/*!=NULL*/)
{
	PRBTREE_ASSERT_PTR(current);
	for (;;) {
		struct prbtree_node *const p = prbtree_get_parent(current);
		if (!p || current == p->prbtree_left)
			return p; /* NULL? */
		current = p;
	}
}

/* find left parent */
static inline struct prbtree_node *prbtree_left_parent(
	const struct prbtree_node *current/*!=NULL*/)
{
	PRBTREE_ASSERT_PTR(current);
	for (;;) {
		struct prbtree_node *const p = prbtree_get_parent(current);
		if (!p || current == p->prbtree_right)
			return p; /* NULL? */
		current = p;
	}
}

/* get next node, returns NULL for the rightmost node */
static inline struct prbtree_node *prbtree_next(
	const struct prbtree_node *const current/*!=NULL*/)
{
	PRBTREE_ASSERT_PTR(current);
	{
		const struct prbtree_node *const n = current->prbtree_right;
		if (n)
			return prbtree_node_from_btree_node_(btree_first(&n->u.n)); /* != NULL */
	}
	return prbtree_right_parent(current); /* NULL? */
}

/* get previous node, returns NULL for the leftmost node */
static inline struct prbtree_node *prbtree_prev(
	const struct prbtree_node *const current/*!=NULL*/)
{
	PRBTREE_ASSERT_PTR(current);
	{
		const struct prbtree_node *const p = current->prbtree_left;
		if (p)
			return prbtree_node_from_btree_node_(btree_last(&p->u.n)); /* != NULL */
	}
	return prbtree_left_parent(current); /* NULL? */
}

#ifdef __cplusplus
}
#endif

#endif /* PRBTREE_H_INCLUDED */
