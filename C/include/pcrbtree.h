#ifndef PCRBTREE_H_INCLUDED
#define PCRBTREE_H_INCLUDED

/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2018-2022 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* pcrbtree.h */

#include "btree.h"

/* declaration for exported functions, such as:
  __declspec(dllexport)/__declspec(dllimport) or __attribute__((visibility("default"))) */
#ifndef PCRBTREE_EXPORTS
#define PCRBTREE_EXPORTS
#endif

/* PCRBTREE_RESTRICT - annotates pointer pointing to memory that is not writable via other pointers */
#ifndef PCRBTREE_RESTRICT
#ifdef A_Restrict
#define PCRBTREE_RESTRICT A_Restrict
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define PCRBTREE_RESTRICT restrict
#elif defined(_MSC_VER) && (_MSC_VER >= 1600)
#define PCRBTREE_RESTRICT __restrict
#elif defined(__GNUC__) && (__GNUC__ >= 3)
#define PCRBTREE_RESTRICT __restrict__
#elif defined(__clang__)
#define PCRBTREE_RESTRICT __restrict__
#else
#define PCRBTREE_RESTRICT
#endif
#endif

/* expr - do not compares pointers */
#ifndef PCRBTREE_ASSERT
#define PCRBTREE_ASSERT(expr) BTREE_ASSERT(expr)
#endif

/* check that pointer is not NULL */
#ifndef PCRBTREE_ASSERT_PTR
#define PCRBTREE_ASSERT_PTR(ptr) BTREE_ASSERT_PTR(ptr)
#endif

/* expr - may compare pointers for equality */
#ifndef PCRBTREE_ASSERT_PTRS
#define PCRBTREE_ASSERT_PTRS(expr) PCRBTREE_ASSERT(expr)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Embedded binary tree:
  one object may encapsulate multiple tree nodes - to reference it from multiple trees */

/* Note: no pointer comparisons are used in search/insert/delete operations */

/* NOTE: address of this structure must be aligned on at least 4 bytes:
  lowest two bits of parent_color pointer encode node color and whenever
  the node is a left or right child of the parent */
struct pcrbtree_node {
	union {
		struct btree_node n;
		struct pcrbtree_node *leaves[2]; /* left, right */
	} u;
	void *parent_color; /* parent pointer + red/black color + left/right flag, NULL for root node */
};

/* left/right leaves */
#define pcrbtree_left  u.leaves[0]
#define pcrbtree_right u.leaves[1]

/* lowest 2 bits of parent_color pointer encodes flags - make sure that pointer is properly aligned */
#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L

#if defined(__GNUC__) && (__GNUC__ >= 5)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-c11-compat" /* warning: ISO C99 does not support '_Alignof' */
#endif

typedef int pcrbtree_node_check_alignment_t[1-2*(_Alignof(struct pcrbtree_node) < 4)];

#if defined(__GNUC__) && (__GNUC__ >= 5)
#pragma GCC diagnostic pop
#endif

#elif defined __cplusplus && __cplusplus >= 201103L
typedef int pcrbtree_node_check_alignment_t[1-2*(alignof(struct pcrbtree_node) < 4)];
#elif defined __GNUC__
typedef int pcrbtree_node_check_alignment_t[1-2*(__alignof__(struct pcrbtree_node) < 4)];
#elif defined _MSC_VER
typedef int pcrbtree_node_check_alignment_t[1-2*(__alignof(struct pcrbtree_node) < 4)];
#endif

static inline struct pcrbtree_node *pcrbtree_node_from_btree_node_(
	const struct btree_node *const n/*NULL?*/)
{
	void *const x = btree_const_cast(n/*NULL?*/);
	return (struct pcrbtree_node*)x;
}

static inline struct btree_node *pcrbtree_node_to_btree_node_(
	const struct pcrbtree_node *const p/*NULL?*/)
{
	const void *const x = p;
	return btree_const_cast((const struct btree_node*)x/*NULL?*/);
}

/* tree - just a pointer to the root node */
struct pcrbtree {
	struct pcrbtree_node *root; /* NULL if tree is empty */
};

static inline void pcrbtree_init(
	struct pcrbtree *const tree/*!=NULL,out*/)
{
	PCRBTREE_ASSERT_PTR(tree);
	tree->root = (struct pcrbtree_node*)0;
}

static inline void pcrbtree_init_node(
	struct pcrbtree_node *const e/*!=NULL,out*/)
{
	PCRBTREE_ASSERT_PTR(e);
	e->pcrbtree_left  = (struct pcrbtree_node*)0;
	e->pcrbtree_right = (struct pcrbtree_node*)0;
	e->parent_color   = (void*)0;
}

static inline void pcrbtree_check_new_node(
	const struct pcrbtree_node *const e/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(e);
	PCRBTREE_ASSERT(!e->pcrbtree_left);
	PCRBTREE_ASSERT(!e->pcrbtree_right);
	PCRBTREE_ASSERT(!e->parent_color);
}

static inline struct pcrbtree_node *pcrbtree_get_parent_(
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
	return (struct pcrbtree_node*)((unsigned long long)parent_color & ~3llu &
		(((1llu << (8*sizeof(void*) - 1)) | ~(~0llu << (8*sizeof(void*) - 1))))) +
		0*sizeof(int[1-2*(255 != (unsigned char)-1)]) +
		0*sizeof(int[1-2*(sizeof(unsigned long long) < sizeof(void*))]);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}

/* clang complains about side-effects if pcrbtree_get_bits_1() is used in PCRBTREE_ASSERT(), so use this define */
#define pcrbtree_get_bits_2(parent_color) ((unsigned)(3llu & (unsigned long long)(parent_color)))

static inline unsigned pcrbtree_get_bits_1(
	const void *const parent_color/*NULL?*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	return pcrbtree_get_bits_2(parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}

static inline unsigned pcrbtree_is_right_1(
	const void *const parent_color/*NULL?*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	return (unsigned)(1llu & (unsigned long long)parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}

static inline unsigned pcrbtree_get_color_1(
	const void *const parent_color/*NULL?*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	return (unsigned)(2llu & (unsigned long long)parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}

static inline struct pcrbtree_node *pcrbtree_get_parent(
	const struct pcrbtree_node *const n/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(n);
	return pcrbtree_get_parent_(n->parent_color); /* NULL? */
}

/* returns: 0,1,2 or 3 */
static inline unsigned pcrbtree_get_bits_(
	const struct pcrbtree_node *const n/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(n);
	return pcrbtree_get_bits_1(n->parent_color);
}

/* returns: 0 or 1 */
static inline unsigned pcrbtree_is_right_(
	const struct pcrbtree_node *const n/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(n);
	return pcrbtree_is_right_1(n->parent_color);
}

/* returns: 0 or 2 */
static inline unsigned pcrbtree_get_color_(
	const struct pcrbtree_node *const n/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(n);
	return pcrbtree_get_color_1(n->parent_color);
}

static inline struct pcrbtree_node *pcrbtree_left_black_node_parent_(
	struct pcrbtree_node *const n/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(n);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	PCRBTREE_ASSERT(!pcrbtree_get_bits_2(n->parent_color));
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
	return (struct pcrbtree_node*)n->parent_color;
}

static inline void *pcrbtree_make_parent_color_(
	struct pcrbtree_node *const p/*NULL?*/,
	const unsigned bits/*0,1,2,3*/)
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
	PCRBTREE_ASSERT(bits <= 3);
	PCRBTREE_ASSERT(!(3llu & (unsigned long long)p));
	return (char*)(((unsigned long long)p | bits) &
		(((1llu << (8*sizeof(void*) - 1)) | ~(~0llu << (8*sizeof(void*) - 1))))) +
		0*sizeof(int[1-2*(255 != (unsigned char)-1)]) +
		0*sizeof(int[1-2*(sizeof(unsigned long long) < sizeof(void*))]);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}

PCRBTREE_EXPORTS void pcrbtree_rebalance(
	struct pcrbtree *const tree/*!=NULL*/,
	struct pcrbtree_node *PCRBTREE_RESTRICT p/*!=NULL*/,
	struct pcrbtree_node *PCRBTREE_RESTRICT e/*!=NULL*/,
	const int c);

/* insert new node into the tree,
  c - result of btree_search_parent():
  c  < 0: parent key  < e's key, insert e at right of the parent;
  c >= 0: parent key >= e's key, insert e at left of the parent */
/* if p is NULL, assume the tree is empty - e becomes the root node */
#if 0 /* example */
  struct btree_node *parent = pcrbtree_node_to_btree_node_(tree->root); /* NULL? */
  int c = btree_search_parent(&parent, key, key_comparator, allow_duplicates);
  pcrbtree_insert(tree, pcrbtree_node_from_btree_node_(parent), node, c);
#endif
static inline void pcrbtree_insert(
	struct pcrbtree *const tree/*!=NULL*/,
	struct pcrbtree_node *PCRBTREE_RESTRICT const p/*NULL?*/,
	struct pcrbtree_node *PCRBTREE_RESTRICT const e/*!=NULL*/,
	int c)
{
	PCRBTREE_ASSERT_PTR(tree);
	PCRBTREE_ASSERT_PTR(e);
	PCRBTREE_ASSERT_PTRS(p != e);
	pcrbtree_check_new_node(e); /* new node must have NULL children and parent */
	if (p) {
		PCRBTREE_ASSERT(!p->u.leaves[c < 0]);
		pcrbtree_rebalance(tree, p, e, c);
	}
	else {
		PCRBTREE_ASSERT(!tree->root);
		tree->root = e; /* black node */
	}
}

/* replace old node in the tree with a new one */
static inline void pcrbtree_replace_(
	struct pcrbtree_node **PCRBTREE_RESTRICT const n/*!=NULL,out*/,
	const struct pcrbtree_node *PCRBTREE_RESTRICT const o/*!=NULL,in*/,
	struct pcrbtree_node *PCRBTREE_RESTRICT const e/*!=NULL,out*/)
{
	PCRBTREE_ASSERT_PTR(n);
	PCRBTREE_ASSERT_PTR(o);
	PCRBTREE_ASSERT_PTR(e);
	PCRBTREE_ASSERT_PTRS(o != e);
	{
		struct pcrbtree_node **PCRBTREE_RESTRICT const el = &e->pcrbtree_left;
		struct pcrbtree_node **PCRBTREE_RESTRICT const er = &e->pcrbtree_right;
		struct pcrbtree_node *PCRBTREE_RESTRICT const l = o->pcrbtree_left;    /* NULL? */
		struct pcrbtree_node *PCRBTREE_RESTRICT const r = o->pcrbtree_right;   /* NULL? */
		void *const p = o->parent_color;
		PCRBTREE_ASSERT_PTRS(o != l);
		PCRBTREE_ASSERT_PTRS(o != r);
		PCRBTREE_ASSERT_PTRS(e != l);
		PCRBTREE_ASSERT_PTRS(e != r);
		PCRBTREE_ASSERT_PTRS((!l && !r) || (l != r));
		PCRBTREE_ASSERT_PTRS(n != el);
		PCRBTREE_ASSERT_PTRS(n != er);
		PCRBTREE_ASSERT_PTRS(el != er);
		*n = e;
		*el = l;
		*er = r;
		e->parent_color = p;
		if (l)
			l->parent_color = pcrbtree_make_parent_color_(e, pcrbtree_get_bits_(l));
		if (r)
			r->parent_color = pcrbtree_make_parent_color_(e, pcrbtree_get_bits_(r));
	}
}

static inline struct pcrbtree_node **pcrbtree_slot_at_parent_(
	struct pcrbtree *const tree/*!=NULL*/,
	struct pcrbtree_node *const p/*NULL?*/,
	const unsigned is_right/*0,1*/)
{
	PCRBTREE_ASSERT_PTR(tree);
	return p ? &p->u.leaves[is_right] : &tree->root;
}

/* replace old node in the tree with a new one */
static inline void pcrbtree_replace(
	struct pcrbtree *const tree/*!=NULL*/,
	const struct pcrbtree_node *PCRBTREE_RESTRICT const o/*!=NULL*/,
	struct pcrbtree_node *PCRBTREE_RESTRICT const e/*!=NULL,out*/)
{
	PCRBTREE_ASSERT_PTR(tree);
	PCRBTREE_ASSERT_PTR(o);
	PCRBTREE_ASSERT_PTR(e);
	PCRBTREE_ASSERT_PTRS(o != e);
	{
		void *const parent_color = o->parent_color;
		struct pcrbtree_node *PCRBTREE_RESTRICT const p = pcrbtree_get_parent_(parent_color); /* NULL? */
		const unsigned is_right = pcrbtree_is_right_1(parent_color);
		PCRBTREE_ASSERT_PTRS(p != o);
		PCRBTREE_ASSERT_PTRS(p != e);
		pcrbtree_replace_(pcrbtree_slot_at_parent_(tree, p, is_right), o, e);
	}
}

/* remove node from the tree */
PCRBTREE_EXPORTS void pcrbtree_remove(
	struct pcrbtree *const tree/*!=NULL*/,
	struct pcrbtree_node *PCRBTREE_RESTRICT e/*!=NULL*/);

/* non-recursive iteration over nodes of the tree */

/* find right parent */
static inline struct pcrbtree_node *pcrbtree_right_parent(
	const struct pcrbtree_node *current/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(current);
	for (;;) {
		struct pcrbtree_node *const p = pcrbtree_get_parent(current);
		if (!p || !pcrbtree_is_right_(current))
			return p; /* NULL? */
		current = p;
	}
}

/* find left parent */
static inline struct pcrbtree_node *pcrbtree_left_parent(
	const struct pcrbtree_node *current/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(current);
	for (;;) {
		struct pcrbtree_node *const p = pcrbtree_get_parent(current);
		if (!p || pcrbtree_is_right_(current))
			return p; /* NULL? */
		current = p;
	}
}

/* get next node, returns NULL for the rightmost node */
static inline struct pcrbtree_node *pcrbtree_next(
	const struct pcrbtree_node *const current/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(current);
	{
		const struct pcrbtree_node *const n = current->pcrbtree_right;
		if (n)
			return pcrbtree_node_from_btree_node_(btree_first(&n->u.n)); /* != NULL */
	}
	return pcrbtree_right_parent(current); /* NULL? */
}

/* get previous node, returns NULL for the leftmost node */
static inline struct pcrbtree_node *pcrbtree_prev(
	const struct pcrbtree_node *const current/*!=NULL*/)
{
	PCRBTREE_ASSERT_PTR(current);
	{
		const struct pcrbtree_node *const p = current->pcrbtree_left;
		if (p)
			return pcrbtree_node_from_btree_node_(btree_last(&p->u.n)); /* != NULL */
	}
	return pcrbtree_left_parent(current); /* NULL? */
}

#ifdef __cplusplus
}
#endif

#endif /* PCRBTREE_H_INCLUDED */
