#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

/**********************************************************************************
* Embedded binary tree
* Copyright (C) 2012-2017 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* btree.h */

#include "sal_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _btree_node {
	struct _btree_node *leaves[2]; /* left, right */
};

#define btree_left leaves[0]
#define btree_right leaves[1]

#define _BTREE_KEY_COMPARATOR1(a) BTREE_KEY_COMPARATOR1 a
#define _BTREE_KEY_COMPARATOR2(a) BTREE_KEY_COMPARATOR2 a
#define _BTREE_KEY_COMPARATOR3(a) BTREE_KEY_COMPARATOR3 a
#define _BTREE_KEY_COMPARATOR4(a) BTREE_KEY_COMPARATOR4 a
#define _BTREE_KEY_COMPARATOR5(a) BTREE_KEY_COMPARATOR5 a
#define _BTREE_KEY_COMPARATOR6(a) BTREE_KEY_COMPARATOR6 a

A_Const_function A_Check_return A_Ret_range(==,a - k)
static inline int _btree_key_int_diff(unsigned a, unsigned k)
{
	return (int)((a < k) ? ~0u - (k - a - 1u) : a - k);
}

/* a and k must be of the same size */
#define BTREE_KEY_COMPARATOR1(a,k) \
	(sizeof(a) <= sizeof(int) ? \
		_btree_key_int_diff((unsigned)(a), (unsigned)(k)) : \
		(a) < (k) ? -1 : \
		(a) > (k) ? 1 : 0)

#define BTREE_KEY_COMPARATOR2(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) : \
			_BTREE_KEY_COMPARATOR1((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			_BTREE_KEY_COMPARATOR1((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR3(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) : \
			_BTREE_KEY_COMPARATOR2((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			_BTREE_KEY_COMPARATOR2((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR4(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) : \
			_BTREE_KEY_COMPARATOR3((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			_BTREE_KEY_COMPARATOR3((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR5(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) : \
			_BTREE_KEY_COMPARATOR4((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			_BTREE_KEY_COMPARATOR4((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR6(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) : \
			_BTREE_KEY_COMPARATOR5((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			_BTREE_KEY_COMPARATOR5((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR7(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) ? \
			_btree_key_int_diff((unsigned)(a), (unsigned)(k)) : \
			_BTREE_KEY_COMPARATOR6((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			_BTREE_KEY_COMPARATOR6((__VA_ARGS__))))

#define _BTREENARGS_SEQ(a1,k1,a2,k2,a3,k3,a4,k4,a5,k5,a6,k6,a7,k7,N,M,...) N
#define __BTREENARGS(a) _BTREENARGS_SEQ a
#define _BTREENARGS(...) __BTREENARGS((__VA_ARGS__,7,0,6,0,5,0,4,0,3,0,2,0,1,0,0))
#define ___BTREE_KEY_COMPARATOR(N,a) BTREE_KEY_COMPARATOR##N a
#define __BTREE_KEY_COMPARATOR(N,a) ___BTREE_KEY_COMPARATOR(N,a)
#define _BTREE_KEY_COMPARATOR(N,a) __BTREE_KEY_COMPARATOR(N,a)

/* helper macro to implement comparator callback passed to _btree_search():
  BTREE_KEY_COMPARATOR(a1,k1,a2,k2,a3,k3)
  - first compare high parts a1 and k1, if they are equal, then
  - next compare lower parts a2 and k2, if they are equal, then
  - at last compere lowest parts a3 and k3 */
/* NOTE: arguments are expanded many times */
#define BTREE_KEY_COMPARATOR(...) _BTREE_KEY_COMPARATOR(_BTREENARGS(__VA_ARGS__),(__VA_ARGS__))

/* search key structure that is passed to comparator callback */
struct _btree_key {
	char __c; /* key pointer may be arbitrary aligned */
};

A_Const_function A_Check_return A_Ret_range(==,n)
static inline struct _btree_node *_btree_const_cast(const struct _btree_node *n/*NULL?*/)
{
	union {
		const struct _btree_node *ct;
		struct _btree_node *t;
	} u;
	u.ct = n;
	return u.t; /* NULL? */
}

/* search node in the tree ordered by keys,
  returns NULL if node with given key was not found */
/* int comparator(node, key) - returns (node - key) difference */
A_Nonnull_arg(2)
A_Nonnull_arg(3)
A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_search(
	A_In_opt const struct _btree_node *tree/*NULL?*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key))
{
	while (tree) {
		int c = comparator(tree, key);
		if (c == 0)
			break;
		tree = tree->leaves[c < 0];
	}
	return tree; /* NULL? */
}

#if 0 /* example */

union my_key {
	struct {
		int a;            /* high part of the key */
		int b;            /* middle part of the key */
		int c;            /* lower part of the key */
	} s;
	struct _btree_key k;
};

struct my_struct {
	int a;                /* high part of the key */
	struct _btree_node n;
	int b;                /* middle part of the key */
	int my_data;
	int c;                /* lower part of the key */
};

static inline A_Nonnull_all_args int my_comparator(A_In const struct _btree_node *node, A_In const struct _btree_key *key)
{
	const struct my_struct *s = CONTAINER_OF(node, const struct my_struct, n);
	const union my_key *k = CONTAINER_OF(key, const union my_key, k);
	return BTREE_KEY_COMPARATOR(s->a, k->s.a, s->b, k->s.b, s->c, k->s.c);
}

static A_Ret_maybenull const struct my_struct *my_search(A_In_opt const struct _btree_node *tree/*NULL?*/, int a, int b, int c)
{
	union my_key key;
	key.s.a = a;
	key.s.b = b;
	key.s.c = c;
	return OPT_CONTAINER_OF(_btree_search(tree/*NULL?*/, &key.k, my_comparator), const struct my_struct, n); /* returns NULL? */
}

#endif /* end of example */

/* object that is passed to checker callback */
struct _btree_object {
	char __c; /* object pointer may be arbitrary aligned */
};

/* iterate over the tree calling callback for each node,
  returns node on which callback has returned 0 */
/* int callback(tree, obj) - returns 0 to stop iteration */
A_Nonnull_arg(2)
A_Nonnull_arg(3)
A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_recursive(
	A_In_opt const struct _btree_node *tree/*NULL?*/,
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	for (; tree && callback(tree, obj); tree = tree->btree_right) {
		const struct _btree_node *left = _btree_walk_recursive(tree->btree_left, obj, callback);
		if (left)
			return left;
	}
	return tree; /* NULL? */
}

/* same as _btree_walk_recursive(), but in forward direction */
A_Nonnull_arg(2)
A_Nonnull_arg(3)
A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_recursive_forward(
	A_In_opt const struct _btree_node *tree/*NULL?*/,
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	for (; tree; tree = tree->btree_right) {
		const struct _btree_node *left = _btree_walk_recursive_forward(tree->btree_left, obj, callback);
		if (left)
			return left;
		if (!callback(tree, obj))
			break;
	}
	return tree; /* NULL? */
}

/* same as _btree_walk_recursive(), but in backward direction */
A_Nonnull_arg(2)
A_Nonnull_arg(3)
A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_recursive_backward(
	A_In_opt const struct _btree_node *tree/*NULL?*/,
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	for (; tree; tree = tree->btree_left) {
		const struct _btree_node *right = _btree_walk_recursive_backward(tree->btree_right, obj, callback);
		if (right)
			return right;
		if (!callback(tree, obj))
			break;
	}
	return tree; /* NULL? */
}

/* get leftmost node of the tree */
A_Nonnull_all_args A_Pure_function A_Check_return A_Ret_never_null
static inline const struct _btree_node *_btree_first(A_In const struct _btree_node *tree/*!=NULL*/)
{
	for (;;) {
		const struct _btree_node *l = tree->btree_left;
		if (!l)
			break;
		tree = l;
	}
	return tree; /* !=NULL */
}

/* get rightmost node of the tree */
A_Nonnull_all_args A_Pure_function A_Check_return A_Ret_never_null
static inline const struct _btree_node *_btree_last(A_In const struct _btree_node *tree/*!=NULL*/)
{
	for (;;) {
		const struct _btree_node *r = tree->btree_right;
		if (!r)
			break;
		tree = r;
	}
	return tree; /* !=NULL */
}

/* walk over left branch of same-key subtree */
/* for example, walk over nodes (b,1) and (b,2) of (b,*) subtree:

                     b,3
         a,4                     c,3
   a,2         b,1         c,1         d,2
a,1   a,3   a,5   b,2   b,4   c,2   d,1   d,3

  where comparator() callback - compares high parts of keys (a, b, c and d) */
A_Nonnull_all_args A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_sub_recursive_left(
	A_In const struct _btree_node *tree/*!=NULL*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	for (;;) {
		for (tree = tree->btree_left;; tree = tree->btree_right) {
			if (!tree)
				return tree; /* NULL */
			if (!comparator(tree, key))
				break;
		}
		if (!callback(tree, obj))
			return tree;
		{
			const struct _btree_node *n = _btree_walk_recursive(tree->btree_right, obj, callback);
			if (n)
				return n;
		}
	}
}

/* walk over right branch of same-key subtree */
/* for example, walk over node (b,4) of (b,*) subtree:

                     b,3
         a,4                     c,3
   a,2         b,1         c,1         d,2
a,1   a,3   a,5   b,2   b,4   c,2   d,1   d,3

  where comparator() callback - compares high parts of keys (a, b, c and d) */
A_Nonnull_all_args A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_sub_recursive_right(
	A_In const struct _btree_node *tree/*!=NULL*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	for (;;) {
		for (tree = tree->btree_right;; tree = tree->btree_left) {
			if (!tree)
				return tree; /* NULL */
			if (!comparator(tree, key))
				break;
		}
		if (!callback(tree, obj))
			return tree;
		{
			const struct _btree_node *n = _btree_walk_recursive(tree->btree_left, obj, callback);
			if (n)
				return n;
		}
	}
}

/* walk over same-key subtree,
  walking stops if processor callback returns 0 for processed node
  - that node is returned as result of walking */
/* tree - result of previous _btree_search() */
/* for example, walk over nodes (b,3), (b,1), (b,2) and (b,4) of (b,*) subtree:

                     b,3
         a,4                     c,3
   a,2         b,1         c,1         d,2
a,1   a,3   a,5   b,2   b,4   c,2   d,1   d,3

  where comparator() callback - compares high parts of keys (a, b, c and d) */
A_Nonnull_all_args A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_sub_recursive(
	A_In const struct _btree_node *tree/*!=NULL*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	if (!callback(tree, obj))
		return tree;
	{
		const struct _btree_node *n = _btree_walk_sub_recursive_left(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	return _btree_walk_sub_recursive_right(tree, key, comparator, obj, callback);
}

/* walk over left branch of same-key subtree in forward direction */
A_Nonnull_all_args A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_sub_recursive_forward_left(
	A_In const struct _btree_node *tree/*!=NULL*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	for (tree = tree->btree_left;; tree = tree->btree_right) {
		if (!tree)
			return tree; /* NULL */
		if (!comparator(tree, key))
			break;
	}
	{
		const struct _btree_node *n = _btree_walk_sub_recursive_forward_left(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!callback(tree, obj))
		return tree;
	return _btree_walk_recursive_forward(tree->btree_right, obj, callback);
}

/* walk over right branch of same-key subtree in forward direction */
A_Nonnull_all_args A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_sub_recursive_forward_right(
	A_In const struct _btree_node *tree/*!=NULL*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	for (;;) {
		for (tree = tree->btree_right;; tree = tree->btree_left) {
			if (!tree)
				return tree; /* NULL */
			if (!comparator(tree, key))
				break;
		}
		{
			const struct _btree_node *n = _btree_walk_recursive_forward(tree->btree_left, obj, callback);
			if (n)
				return n;
		}
		if (!callback(tree, obj))
			return tree;
	}
}

/* walk over same-key subtree in forward direction,
  walking stops if processor callback returns 0 for processed node
  - that node is returned as result of walking */
/* tree - result of previous _btree_search() */
A_Nonnull_all_args A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_sub_recursive_forward(
	A_In const struct _btree_node *tree/*!=NULL*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	{
		const struct _btree_node *n = _btree_walk_sub_recursive_forward_left(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!callback(tree, obj))
		return tree;
	return _btree_walk_sub_recursive_forward_right(tree, key, comparator, obj, callback);
}

/* walk over right branch of same-key subtree in backward direction */
A_Nonnull_all_args A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_sub_recursive_backward_right(
	A_In const struct _btree_node *tree/*!=NULL*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	for (tree = tree->btree_right;; tree = tree->btree_left) {
		if (!tree)
			return tree; /* NULL */
		if (!comparator(tree, key))
			break;
	}
	{
		const struct _btree_node *n = _btree_walk_sub_recursive_backward_right(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!callback(tree, obj))
		return tree;
	return _btree_walk_recursive_backward(tree->btree_left, obj, callback);
}

/* walk over left branch of same-key subtree in backward direction */
A_Nonnull_all_args A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_sub_recursive_backward_left(
	A_In const struct _btree_node *tree/*!=NULL*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	for (;;) {
		for (tree = tree->btree_left;; tree = tree->btree_right) {
			if (!tree)
				return tree; /* NULL */
			if (!comparator(tree, key))
				break;
		}
		{
			const struct _btree_node *n = _btree_walk_recursive_backward(tree->btree_right, obj, callback);
			if (n)
				return n;
		}
		if (!callback(tree, obj))
			return tree;
	}
}

/* walk over same-key subtree in backward direction,
  walking stops if processor callback returns 0 for processed node
  - that node is returned as result of walking */
/* tree - result of previous _btree_search() */
A_Nonnull_all_args A_Check_return A_Ret_maybenull
static inline const struct _btree_node *_btree_walk_sub_recursive_backward(
	A_In const struct _btree_node *tree/*!=NULL*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	A_Inout struct _btree_object *obj,
	A_In int (*callback)(
		A_In const struct _btree_node *node,
		A_Inout struct _btree_object *obj))
{
	{
		const struct _btree_node *n = _btree_walk_sub_recursive_backward_right(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!callback(tree, obj))
		return tree;
	return _btree_walk_sub_recursive_backward_left(tree, key, comparator, obj, callback);
}

/* find leaf parent of to be inserted node in same-key sub tree */
A_Nonnull_all_args A_Check_return
static inline int _btree_find_leaf(
	A_In struct _btree_node *p,
	A_Outptr struct _btree_node **parent/*out:!=NULL*/)
{
	if (p->btree_right) {
		struct _btree_node *left = p->btree_left;
		if (!left) {
			*parent = p;
			return 1; /* parent at right */
		}
		p = _btree_const_cast(_btree_last(left));
	}
	*parent = p;
	return -1; /* parent at left */
}

/* search leaf parent of to be inserted node in the tree ordered by abstract keys,
  initially parent references the root of the tree, may be NULL if tree is empty,
  returns:
  < 0 - if parent at left,
  > 0 - if parent at right,
    0 - if parent references node with the same key and leaf is zero,
  NOTE: if tree allows nodes with non-unique keys, leaf must be non-zero */
A_Nonnull_all_args A_Check_return
static inline int _btree_search_parent(
	A_Inout struct _btree_node **parent/*in:NULL?,out*/,
	A_In const struct _btree_key *key,
	A_In int (*comparator)(
		A_In const struct _btree_node *node,
		A_In const struct _btree_key *key),
	int leaf)
{
	struct _btree_node *p = *parent;
	if (!p)
		return 1; /* tree is empty, parent is NULL */
	for (;;) {
		struct _btree_node *_p = p;
		int c = comparator(p, key); /* c = p - key */
		if (c != 0) {
			p = p->leaves[c < 0];
			if (p)
				continue;
		}
		else if (leaf)
			return _btree_find_leaf(p, parent);
		*parent = _p;
		return c; /* if 0, then (*parent) - references found node, else (*parent) - references leaf parent */
	}
}

A_Pure_function A_Check_return
static inline size_t _btree_size_(A_In_opt const struct _btree_node *tree/*NULL?*/, size_t s)
{
	while (tree) {
		s = _btree_size_(tree->btree_left, s + 1);
		tree = tree->btree_right;
	}
	return s;
}

/* recursively count nodes in the tree */
A_Pure_function A_Check_return
static inline size_t _btree_size(A_In_opt const struct _btree_node *tree/*NULL?*/)
{
	return _btree_size_(tree, 0);
}

/* recursively determine tree height */
A_Pure_function A_Check_return
static inline size_t _btree_height(A_In_opt const struct _btree_node *tree/*NULL?*/)
{
	if (!tree)
		return 0;
	{
		size_t left_height = _btree_height(tree->btree_left);
		size_t right_height = _btree_height(tree->btree_right);
		return 1 + (left_height > right_height ? left_height : right_height);
	}
}

#ifdef __cplusplus
}
#endif

#endif /* BTREE_H_INCLUDED */
