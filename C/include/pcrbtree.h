#ifndef PCRBTREE_H_INCLUDED
#define PCRBTREE_H_INCLUDED

/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2018-2019 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* pcrbtree.h */

#include "btree.h"

/* declaration for exported functions, such as:
  __declspec(dllexport)/__declspec(dllimport) or __attribute__((visibility("default"))) */
#ifndef PCRBTREE_EXPORTS
#define PCRBTREE_EXPORTS
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

#ifndef PCRBTREE_ASSERT
#ifdef ASSERT
#define PCRBTREE_ASSERT(expr) ASSERT(expr)
#else
#define PCRBTREE_ASSERT(expr) ((void)0)
#endif
#endif

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

/* check that pointer is not NULL */
#ifdef _MSC_VER
#define pcrbtree_assert_ptr_(p) PCRBTREE_ASSERT(p)
#else
/* do not declare 'p' as non-NULL, so gcc/clang will not complain about comparison of non-NULL pointer with 0 */
static inline void pcrbtree_assert_ptr_(const void *p)
{
	PCRBTREE_ASSERT(p);
}
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_At(n, A_In_opt)
A_Check_return
A_When(n, A_Ret_notnull)
A_When(!n, A_Ret_null)
A_Ret_range(==,n)
#endif
static inline struct pcrbtree_node *pcrbtree_node_from_btree_node_(
	const struct btree_node *n/*NULL?*/)
{
	void *p = btree_const_cast(n/*NULL?*/);
	return (struct pcrbtree_node*)p;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_At(pn, A_In_opt)
A_Check_return
A_When(pn, A_Ret_notnull)
A_When(!pn, A_Ret_null)
A_Ret_range(==,pn)
#endif
static inline struct btree_node *pcrbtree_node_to_btree_node_(
	const struct pcrbtree_node *pn/*NULL?*/)
{
	const void *p = pn;
	return btree_const_cast((const struct btree_node*)p/*NULL?*/);
}

/* tree - just a pointer to the root node */
struct pcrbtree {
	struct pcrbtree_node *root; /* NULL if tree is empty */
};

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(tree, A_Out)
#endif
static inline void pcrbtree_init(
	struct pcrbtree *tree/*!=NULL,out*/)
{
	pcrbtree_assert_ptr_(tree);
	tree->root = (struct pcrbtree_node*)0;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(e, A_Out)
#endif
static inline void pcrbtree_init_node(
	struct pcrbtree_node *e/*!=NULL,out*/)
{
	pcrbtree_assert_ptr_(e);
	e->pcrbtree_left  = (struct pcrbtree_node*)0;
	e->pcrbtree_right = (struct pcrbtree_node*)0;
	e->parent_color   = (void*)0;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(e, A_In)
A_Pre_satisfies(!e->pcrbtree_left)
A_Pre_satisfies(!e->pcrbtree_right)
A_Pre_satisfies(!e->parent_color)
#endif
static inline void pcrbtree_check_new_node(
	const struct pcrbtree_node *e/*!=NULL*/)
{
	pcrbtree_assert_ptr_(e);
	PCRBTREE_ASSERT(!e->pcrbtree_left);
	PCRBTREE_ASSERT(!e->pcrbtree_right);
	PCRBTREE_ASSERT(!e->parent_color);
	(void)e;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_Ret_maybenull
A_Check_return
#endif
static inline struct pcrbtree_node *pcrbtree_get_parent_(
	void *parent_color/*NULL?*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#pragma warning(disable:4305) /* 'type cast': truncation from 'unsigned __int64' to 'void *' */
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast" /* warning: cast from pointer to integer of different size */
#endif
	return (struct pcrbtree_node*)(
		(((1llu << (8*sizeof(void*) - 1)) | ~(~0llu << (8*sizeof(void*) - 1))) + 0*sizeof(int[1-2*(255 != (unsigned char)-1)])) &
		~3llu & (unsigned long long)parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic pop
#endif
}

/* clang complains about side-effects if pcrbtree_get_bits_1() is used in PCRBTREE_ASSERT(), so use this define */
#define pcrbtree_get_bits_2(parent_color) ((unsigned)(3llu & (unsigned long long)parent_color))

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_Ret_range(0,3)
A_Check_return
#endif
static inline unsigned pcrbtree_get_bits_1(
	const void *parent_color/*NULL?*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	return pcrbtree_get_bits_2(parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic pop
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_Ret_range(0,1)
A_Check_return
#endif
static inline unsigned pcrbtree_is_right_1(
	const void *parent_color/*NULL?*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	return (unsigned)(1llu & (unsigned long long)parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic pop
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_Ret_range(0,2)
A_Check_return
#endif
static inline unsigned pcrbtree_get_color_1(
	const void *parent_color/*NULL?*/)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	return (unsigned)(2llu & (unsigned long long)parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic pop
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Const_function
A_At(n, A_In)
A_Ret_maybenull
A_Check_return
#endif
static inline struct pcrbtree_node *pcrbtree_get_parent(
	const struct pcrbtree_node *n/*!=NULL*/)
{
	pcrbtree_assert_ptr_(n);
	return pcrbtree_get_parent_(n->parent_color);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Const_function
A_At(n, A_In)
A_Ret_range(0,3)
A_Check_return
#endif
static inline unsigned pcrbtree_get_bits_(
	const struct pcrbtree_node *n/*!=NULL*/)
{
	pcrbtree_assert_ptr_(n);
	return pcrbtree_get_bits_1(n->parent_color);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Const_function
A_At(n, A_In)
A_Ret_range(0,1)
A_Check_return
#endif
static inline unsigned pcrbtree_is_right_(
	const struct pcrbtree_node *n/*!=NULL*/)
{
	pcrbtree_assert_ptr_(n);
	return pcrbtree_is_right_1(n->parent_color);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Const_function
A_At(n, A_In)
A_Ret_range(0,2)
A_Check_return
#endif
static inline unsigned pcrbtree_get_color_(
	const struct pcrbtree_node *n/*!=NULL*/)
{
	pcrbtree_assert_ptr_(n);
	return pcrbtree_get_color_1(n->parent_color);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Const_function
A_At(n, A_In)
A_Ret_range(==,n->parent_color)
A_Ret_maybenull
A_Check_return
#endif
static inline struct pcrbtree_node *pcrbtree_left_black_node_parent_(
	struct pcrbtree_node *n/*!=NULL*/)
{
	pcrbtree_assert_ptr_(n);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#endif
	PCRBTREE_ASSERT(!pcrbtree_get_bits_2(n->parent_color));
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic pop
#endif
	return (struct pcrbtree_node*)n->parent_color;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_At(p, A_In_opt)
A_At(bits, A_In_range(0,3))
A_Check_return
#endif
static inline void *pcrbtree_make_parent_color_1(
	struct pcrbtree_node *p/*NULL?*/,
	unsigned bits)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#pragma warning(disable:4305) /* 'type cast': truncation from 'unsigned __int64' to 'void *' */
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast" /* warning: cast from pointer to integer of different size */
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast" /* warning: cast from pointer to integer of different size */
#endif
	PCRBTREE_ASSERT(bits <= 3);
	PCRBTREE_ASSERT(!(3llu & (unsigned long long)p));
	return (void*)(
		(((1llu << (8*sizeof(void*) - 1)) | ~(~0llu << (8*sizeof(void*) - 1))) + 0*sizeof(int[1-2*(255 != (unsigned char)-1)])) &
		((unsigned long long)p | bits));
#ifdef _MSC_VER
#pragma warning(pop)
#elif !defined __cplusplus && defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#pragma GCC diagnostic pop
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(tree, A_Inout)
A_At(p, A_Inout)
A_At(e, A_Inout)
#endif
PCRBTREE_EXPORTS void pcrbtree_rebalance(
	struct pcrbtree *A_Restrict tree/*!=NULL*/,
	struct pcrbtree_node *A_Restrict p/*!=NULL*/,
	struct pcrbtree_node *A_Restrict e/*!=NULL*/,
	int c);

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
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
A_Nonnull_arg(3)
A_At(tree, A_Inout)
A_At(p, A_Inout_opt)
A_At(e, A_Inout)
#endif
static inline void pcrbtree_insert(
	struct pcrbtree *A_Restrict tree/*!=NULL*/,
	struct pcrbtree_node *A_Restrict p/*NULL?*/,
	struct pcrbtree_node *A_Restrict e/*!=NULL*/,
	int c)
{
	pcrbtree_assert_ptr_(tree);
	pcrbtree_assert_ptr_(e);
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
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(n, A_Out)
A_At(o, A_In)
A_At(e, A_Out)
#endif
static inline void pcrbtree_replace_(
	struct pcrbtree_node **A_Restrict n/*!=NULL,out*/,
	const struct pcrbtree_node *A_Restrict o/*!=NULL,in*/,
	struct pcrbtree_node *A_Restrict e/*!=NULL,out*/)
{
	pcrbtree_assert_ptr_(n);
	pcrbtree_assert_ptr_(o);
	pcrbtree_assert_ptr_(e);
	PCRBTREE_ASSERT_PTRS(o != e);
	{
		struct pcrbtree_node **A_Restrict el = &e->pcrbtree_left;
		struct pcrbtree_node **A_Restrict er = &e->pcrbtree_right;
		struct pcrbtree_node *A_Restrict l = o->pcrbtree_left;
		struct pcrbtree_node *A_Restrict r = o->pcrbtree_right;
		void *p = o->parent_color;
		PCRBTREE_ASSERT_PTRS(o != l);
		PCRBTREE_ASSERT_PTRS(o != r);
		PCRBTREE_ASSERT_PTRS(e != l);
		PCRBTREE_ASSERT_PTRS(e != r);
		PCRBTREE_ASSERT_PTRS(n != el);
		PCRBTREE_ASSERT_PTRS(n != er);
		PCRBTREE_ASSERT_PTRS(el != er);
		*n = e;
		*el = l;
		*er = r;
		e->parent_color = p;
		if (l) {
			PCRBTREE_ASSERT_PTRS(l != r);
			l->parent_color = pcrbtree_make_parent_color_1(e, pcrbtree_get_bits_(l));
		}
		if (r) {
			PCRBTREE_ASSERT_PTRS(l != r);
			r->parent_color = pcrbtree_make_parent_color_1(e, pcrbtree_get_bits_(r));
		}
	}
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
A_At(tree, A_Inout)
A_At(p, A_Inout_opt)
A_At(is_right, A_In_range(0,1))
A_Ret_never_null
A_Check_return
#endif
static inline struct pcrbtree_node **pcrbtree_slot_at_parent_(
	struct pcrbtree *A_Restrict tree/*!=NULL*/,
	struct pcrbtree_node *A_Restrict p/*NULL?*/,
	unsigned is_right)
{
	pcrbtree_assert_ptr_(tree);
	return p ? &p->u.leaves[is_right] : &tree->root;
}

/* replace old node in the tree with a new one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(tree, A_Inout)
A_At(o, A_In)
A_At(e, A_Out)
#endif
static inline void pcrbtree_replace(
	struct pcrbtree *A_Restrict tree/*!=NULL*/,
	const struct pcrbtree_node *A_Restrict o/*!=NULL*/,
	struct pcrbtree_node *A_Restrict e/*!=NULL,out*/)
{
	pcrbtree_assert_ptr_(tree);
	pcrbtree_assert_ptr_(o);
	pcrbtree_assert_ptr_(e);
	PCRBTREE_ASSERT_PTRS(o != e);
	{
		void *parent_color = o->parent_color;
		struct pcrbtree_node *A_Restrict p = pcrbtree_get_parent_(parent_color); /* NULL? */
		unsigned is_right = pcrbtree_is_right_1(parent_color);
		PCRBTREE_ASSERT_PTRS(p != o);
		PCRBTREE_ASSERT_PTRS(p != e);
		pcrbtree_replace_(pcrbtree_slot_at_parent_(tree, p, is_right), o, e);
	}
}

/* remove node from the tree */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(tree, A_Inout)
A_At(e, A_Inout)
#endif
PCRBTREE_EXPORTS void pcrbtree_remove(
	struct pcrbtree *A_Restrict tree/*!=NULL*/,
	struct pcrbtree_node *A_Restrict e/*!=NULL*/);

/* non-recursive iteration over nodes of the tree */

/* find right parent */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Pure_function
A_At(current, A_In)
A_Ret_maybenull
A_Check_return
#endif
static inline struct pcrbtree_node *pcrbtree_right_parent(
	const struct pcrbtree_node *current/*!=NULL*/)
{
	pcrbtree_assert_ptr_(current);
	for (;;) {
		struct pcrbtree_node *p = pcrbtree_get_parent(current);
		if (!p || !pcrbtree_is_right_(current))
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
static inline struct pcrbtree_node *pcrbtree_left_parent(
	const struct pcrbtree_node *current/*!=NULL*/)
{
	pcrbtree_assert_ptr_(current);
	for (;;) {
		struct pcrbtree_node *p = pcrbtree_get_parent(current);
		if (!p || pcrbtree_is_right_(current))
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
static inline struct pcrbtree_node *pcrbtree_next(
	const struct pcrbtree_node *current/*!=NULL*/)
{
	pcrbtree_assert_ptr_(current);
	{
		const struct pcrbtree_node *n = current->pcrbtree_right;
		if (n)
			return pcrbtree_node_from_btree_node_(btree_first(&n->u.n)); /* != NULL */
	}
	return pcrbtree_right_parent(current); /* NULL? */
}

/* get previous node, returns NULL for the leftmost node */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Pure_function
A_At(current, A_In)
A_Ret_maybenull
A_Check_return
#endif
static inline struct pcrbtree_node *pcrbtree_prev(
	const struct pcrbtree_node *current/*!=NULL*/)
{
	pcrbtree_assert_ptr_(current);
	{
		const struct pcrbtree_node *p = current->pcrbtree_left;
		if (p)
			return pcrbtree_node_from_btree_node_(btree_last(&p->u.n)); /* != NULL */
	}
	return pcrbtree_left_parent(current); /* NULL? */
}

#ifdef __cplusplus
}
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#undef A_Restrict
#endif

#endif /* PCRBTREE_H_INCLUDED */
