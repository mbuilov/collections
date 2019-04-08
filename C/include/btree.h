#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

/**********************************************************************************
* Embedded binary tree
* Copyright (C) 2012-2019 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* btree.h */

#ifndef BTREE_ASSERT
#ifdef ASSERT
#define BTREE_ASSERT(expr) ASSERT(expr)
#else
#define BTREE_ASSERT(expr) ((void)0)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Embedded binary tree:
  one object may encapsulate multiple tree nodes - to reference it from multiple trees */

struct btree_node {
	struct btree_node *leaves[2]; /* left, right */
};

/* left/right leaves */
#define btree_left  leaves[0]
#define btree_right leaves[1]

/* check that pointer is not NULL */
#ifdef _MSC_VER
#define btree_assert_ptr_(p) BTREE_ASSERT(p)
#else
/* do not declare 'p' as non-NULL, so gcc/clang will not complain about comparison of non-NULL pointer with 0 */
#if (defined(__GNUC__) && (__GNUC__ >= 4)) || \
  (defined(__clang__) && (__clang_major__ > 3 || (3 == __clang_major__  && __clang_minor__ >= 7)))
__attribute__ ((pure))
__attribute__ ((always_inline))
#endif
static inline void btree_assert_ptr_(const void *const p)
{
	BTREE_ASSERT(p);
}
#endif

#define BTREE_KEY_COMPARATOR1_(a) BTREE_KEY_COMPARATOR1 a
#define BTREE_KEY_COMPARATOR2_(a) BTREE_KEY_COMPARATOR2 a
#define BTREE_KEY_COMPARATOR3_(a) BTREE_KEY_COMPARATOR3 a
#define BTREE_KEY_COMPARATOR4_(a) BTREE_KEY_COMPARATOR4 a
#define BTREE_KEY_COMPARATOR5_(a) BTREE_KEY_COMPARATOR5 a
#define BTREE_KEY_COMPARATOR6_(a) BTREE_KEY_COMPARATOR6 a

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_Ret_range(==,a - k)
A_Check_return
#endif
static inline int btree_key_int_diff_(
	const unsigned a,
	const unsigned k)
{
#ifdef UBSAN_UNSIGNED_OVERFLOW
	const unsigned d = (a >= k) ? a - k : ((unsigned)-1 - (k - a)) + 1u;
#else
	const unsigned d = a - k;
#endif
	return (int)d;
}

/* a and k must be of the same size */
#define BTREE_KEY_COMPARATOR1(a,k) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		((sizeof(a) <= sizeof(int) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : 0)))

#define BTREE_KEY_COMPARATOR2(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) : \
			BTREE_KEY_COMPARATOR1_((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			BTREE_KEY_COMPARATOR1_((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR3(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) : \
			BTREE_KEY_COMPARATOR2_((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			BTREE_KEY_COMPARATOR2_((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR4(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) : \
			BTREE_KEY_COMPARATOR3_((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			BTREE_KEY_COMPARATOR3_((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR5(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) : \
			BTREE_KEY_COMPARATOR4_((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			BTREE_KEY_COMPARATOR4_((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR6(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) : \
			BTREE_KEY_COMPARATOR5_((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			BTREE_KEY_COMPARATOR5_((__VA_ARGS__))))

#define BTREE_KEY_COMPARATOR7(a,k,...) \
	((int)(0*sizeof(int[1-2*(sizeof(a) != sizeof(k))])) + \
		(sizeof(a) <= sizeof(int) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) ? \
			btree_key_int_diff_((unsigned)(a), (unsigned)(k)) : \
			BTREE_KEY_COMPARATOR6_((__VA_ARGS__)) : \
			(a) < (k) ? -1 : \
			(a) > (k) ? 1 : \
			BTREE_KEY_COMPARATOR6_((__VA_ARGS__))))

#define BTREENARGS_SEQ_(a1,k1,a2,k2,a3,k3,a4,k4,a5,k5,a6,k6,a7,k7,N,M,...) N
#define BTREENARGS_1(a)             BTREENARGS_SEQ_ a
#define BTREENARGS_(...)            BTREENARGS_1((__VA_ARGS__,7,0,6,0,5,0,4,0,3,0,2,0,1,0,0))
#define BTREE_KEY_COMPARATOR_2(N,a) BTREE_KEY_COMPARATOR##N a
#define BTREE_KEY_COMPARATOR_1(N,a) BTREE_KEY_COMPARATOR_2(N,a)
#define BTREE_KEY_COMPARATOR_(N,a)  BTREE_KEY_COMPARATOR_1(N,a)

/* helper macro to implement comparator callback passed to btree_search():
  BTREE_KEY_COMPARATOR(a1,k1,a2,k2,a3,k3)
  - first compare high parts a1 and k1, if they are equal, then
  - next compare lower parts a2 and k2, if they are equal, then
  - at last compare lowest parts a3 and k3 */
/* NOTE: arguments are expanded many times */
#define BTREE_KEY_COMPARATOR(...) BTREE_KEY_COMPARATOR_(BTREENARGS_(__VA_ARGS__),(__VA_ARGS__))

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_At(n, A_In_opt)
A_When(n, A_Ret_valid)
A_When(!n, A_Ret_null)
A_Ret_range(==,n)
A_Check_return
#endif
static inline struct btree_node *btree_const_cast(
	const struct btree_node *const n/*NULL?*/)
{
#ifdef __cplusplus
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:26492) /* Don't use const_cast to cast away const or volatile */
#endif
	return const_cast<struct btree_node*>(n);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#else /* !__cplusplus */
#if defined(__GNUC__) && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual" /* casting away qualifiers */
#endif
	return (struct btree_node*)n;
#if defined(__GNUC__) && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic pop
#endif
#endif /* !__cplusplus */
}

/* abstract key structure that is passed to binary tree comparator callback */
struct btree_key {
	char c_; /* placeholder, must be never accessed, char, so key may be arbitrary aligned */
};

/* binary tree comparator callback - compare node's key with given one,
  if returns 0, search stops */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
typedef int btree_comparator(
	A_In const struct btree_node *node/*!=NULL*/,
	A_In const struct btree_key *key/*!=NULL*/);
#else
typedef int btree_comparator(
	const struct btree_node *node/*!=NULL*/,
	const struct btree_key *key/*!=NULL*/);
#endif

/* check that pointer is not NULL */
#ifdef _MSC_VER
#define btree_assert_comparator_(p) BTREE_ASSERT(p)
#else
/* do not declare 'p' as non-NULL, so gcc/clang will not complain about comparison of non-NULL pointer with 0 */
#if (defined(__GNUC__) && (__GNUC__ >= 4)) || \
  (defined(__clang__) && (__clang_major__ > 3 || (3 == __clang_major__  && __clang_minor__ >= 7)))
__attribute__ ((pure))
__attribute__ ((always_inline))
#endif
static inline void btree_assert_comparator_(btree_comparator *const p)
{
	BTREE_ASSERT(p);
}
#endif

/* search node in the tree ordered by keys,
  returns NULL if node with given key was not found */
/* int comparator(node, key) - returns (node - key) difference */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_At(tree, A_In_opt)
A_At(key, A_When(tree, A_In))
A_At(comparator, A_When(tree, A_In))
A_Ret_opt_valid
A_Check_return
#endif
static inline struct btree_node *btree_search(
	const struct btree_node *tree/*NULL?*/,
	const struct btree_key *const key/*!=NULL if tree!=NULL*/,
	btree_comparator *const comparator/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || key);
	BTREE_ASSERT(!tree || comparator);
	while (tree) {
		const int c = comparator(tree, key); /* c = tree - key */
		if (c == 0)
			break;
		tree = tree->leaves[c < 0];
	}
	return btree_const_cast(tree); /* NULL? */
}

#if 0 /* example */

union my_key {
	struct {
		int a;            /* high part of the key */
		int b;            /* middle part of the key */
		int c;            /* lower part of the key */
	} s;
	struct btree_key k;
};

struct my_struct {
	int a;                /* high part of the key */
	struct btree_node n;
	int b;                /* middle part of the key */
	int my_data;
	int c;                /* lower part of the key */
};

static inline int my_comparator(
	const struct btree_node *node,
	const struct btree_key *key)
{
	const struct my_struct *s = CONTAINER_OF(node, const struct my_struct, n);
	const union my_key *k = CONTAINER_OF(key, const union my_key, k);
	return BTREE_KEY_COMPARATOR(s->a, k->s.a, s->b, k->s.b, s->c, k->s.c);
}

static const struct my_struct *my_search(
	const struct btree_node *tree/*NULL?*/,
	int a, int b, int c)
{
	union my_key key;
	key.s.a = a;
	key.s.b = b;
	key.s.c = c;
	return OPT_CONTAINER_OF(
		btree_search(tree/*NULL?*/, &key.k, my_comparator),
		const struct my_struct, n);
}

#endif /* end of example */

/* abstract object that is passed to checker callback */
struct btree_object {
	char o_; /* placeholder, must be never accessed, char, so object data may be arbitrary aligned */
};

/* binary tree walker callback - process binary tree node,
  if returns 0, walk stops */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
typedef int btree_walker(
	A_In const struct btree_node *node/*!=NULL*/,
	A_Inout struct btree_object *obj/*!=NULL*/);
#else
typedef int btree_walker(
	const struct btree_node *node/*!=NULL*/,
	struct btree_object *obj/*!=NULL*/);
#endif

/* check that pointer is not NULL */
#ifdef _MSC_VER
#define btree_assert_walker_(p) BTREE_ASSERT(p)
#else
/* do not declare 'p' as non-NULL, so gcc/clang will not complain about comparison of non-NULL pointer with 0 */
#if (defined(__GNUC__) && (__GNUC__ >= 4)) || \
  (defined(__clang__) && (__clang_major__ > 3 || (3 == __clang_major__  && __clang_minor__ >= 7)))
__attribute__ ((pure))
__attribute__ ((always_inline))
#endif
static inline void btree_assert_walker_(btree_walker *const p)
{
	BTREE_ASSERT(p);
}
#endif

/* walk over all nodes of unordered tree calling callback for each node,
  returns node on which callback has returned 0 */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_At(tree, A_In_opt)
A_At(obj, A_When(tree, A_Inout))
A_At(callback, A_When(tree, A_In))
A_Ret_opt_valid
A_Check_return
#endif
static inline struct btree_node *btree_walk_recursive(
	const struct btree_node *tree/*NULL?*/,
	struct btree_object *const obj/*!=NULL if tree!=NULL*/,
	btree_walker *const callback/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || obj);
	BTREE_ASSERT(!tree || callback);
	for (; tree && callback(tree, obj); tree = tree->btree_right) {
		struct btree_node *const left = btree_walk_recursive(tree->btree_left, obj, callback);
		if (left)
			return left;
	}
	return btree_const_cast(tree); /* NULL? */
}

/* same as btree_walk_recursive(), but in forward direction, from the leftmost to the rightmost */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_At(tree, A_In_opt)
A_At(obj, A_When(tree, A_Inout))
A_At(callback, A_When(tree, A_In))
A_Ret_opt_valid
A_Check_return
#endif
static inline struct btree_node *btree_walk_recursive_forward(
	const struct btree_node *tree/*NULL?*/,
	struct btree_object *const obj/*!=NULL if tree!=NULL*/,
	btree_walker *const callback/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || obj);
	BTREE_ASSERT(!tree || callback);
	for (; tree; tree = tree->btree_right) {
		struct btree_node *const left = btree_walk_recursive_forward(tree->btree_left, obj, callback);
		if (left)
			return left;
		if (!callback(tree, obj))
			break;
	}
	return btree_const_cast(tree); /* NULL? */
}

/* same as btree_walk_recursive(), but in backward direction, from the rightmost to the leftmost */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_At(tree, A_In_opt)
A_At(obj, A_When(tree, A_Inout))
A_At(callback, A_When(tree, A_In))
A_Ret_opt_valid
A_Check_return
#endif
static inline struct btree_node *btree_walk_recursive_backward(
	const struct btree_node *tree/*NULL?*/,
	struct btree_object *const obj/*!=NULL if tree!=NULL*/,
	btree_walker *const callback/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || obj);
	BTREE_ASSERT(!tree || callback);
	for (; tree; tree = tree->btree_left) {
		struct btree_node *const right = btree_walk_recursive_backward(tree->btree_right, obj, callback);
		if (right)
			return right;
		if (!callback(tree, obj))
			break;
	}
	return btree_const_cast(tree); /* NULL? */
}

/* get the leftmost node of the tree */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Pure_function
A_Nonnull_all_args
A_At(tree, A_In)
A_Ret_never_null
A_Ret_valid
A_Check_return
#endif
static inline struct btree_node *btree_first(
	const struct btree_node *tree/*!=NULL*/)
{
	btree_assert_ptr_(tree);
	for (;;) {
		const struct btree_node *const l = tree->btree_left;
		if (!l)
			break;
		tree = l;
	}
	return btree_const_cast(tree); /* !=NULL */
}

/* get the rightmost node of the tree */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Pure_function
A_Nonnull_all_args
A_At(tree, A_In)
A_Ret_never_null
A_Ret_valid
A_Check_return
#endif
static inline struct btree_node *btree_last(
	const struct btree_node *tree/*!=NULL*/)
{
	btree_assert_ptr_(tree);
	for (;;) {
		const struct btree_node *const r = tree->btree_right;
		if (!r)
			break;
		tree = r;
	}
	return btree_const_cast(tree); /* !=NULL */
}

#define BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback) { \
	btree_assert_ptr_(tree);              \
	btree_assert_ptr_(key);               \
	btree_assert_comparator_(comparator); \
	btree_assert_ptr_(obj);               \
	btree_assert_walker_(callback);       \
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define BTREE_SUB_WALK_ANNOTATIONS \
	A_Nonnull_all_args             \
	A_At(tree, A_In)               \
	A_At(key, A_In)                \
	A_At(comparator, A_In)         \
	A_At(obj, A_Inout)             \
	A_At(callback, A_In)           \
	A_Ret_opt_valid                \
	A_Check_return
#else
#define BTREE_SUB_WALK_ANNOTATIONS
#endif

/* walk over left branch of unordered same-key subtree of ordered tree */
/* for example, walk over nodes (1,b) and (1,a) of (1,*) subtree of ordered by integer keys tree:

                     1,d
         0,c                     2,a
   0,d         1,b         2,b         3,a
0,b   0,e   0,a   1,a   1,c   2,c   3,c   3,b

  where comparator() callback - compares only integer parts of the keys */
BTREE_SUB_WALK_ANNOTATIONS
static inline struct btree_node *btree_walk_sub_recursive_left(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj/*!=NULL*/,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback);
	for (;;) {
		for (tree = tree->btree_left;; tree = tree->btree_right) {
			if (!tree)
				return btree_const_cast(tree); /* NULL */
			if (!comparator(tree, key))
				break;
		}
		if (!callback(tree, obj))
			return btree_const_cast(tree);
		{
			struct btree_node *const n = btree_walk_recursive(tree->btree_right, obj, callback);
			if (n)
				return n;
		}
	}
}

/* walk over right branch of unordered same-key subtree of ordered tree */
/* for example, walk over node (1,c) of (1,*) subtree of ordered by integer keys tree:

                     1,d
         0,c                     2,a
   0,d         1,b         2,b         3,a
0,b   0,e   0,a   1,a   1,c   2,c   3,c   3,b

  where comparator() callback - compares only integer parts of the keys */
BTREE_SUB_WALK_ANNOTATIONS
static inline struct btree_node *btree_walk_sub_recursive_right(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj/*!=NULL*/,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback);
	for (;;) {
		for (tree = tree->btree_right;; tree = tree->btree_left) {
			if (!tree)
				return btree_const_cast(tree); /* NULL */
			if (!comparator(tree, key))
				break;
		}
		if (!callback(tree, obj))
			return btree_const_cast(tree);
		{
			struct btree_node *const n = btree_walk_recursive(tree->btree_left, obj, callback);
			if (n)
				return n;
		}
	}
}

/* walk over unordered same-key subtree of ordered tree,
  walking stops if processor callback returns 0 for processed node
  - that node is returned as result of walking */
/* assume 'tree' - result of previous btree_search() */
/* for example, walk over nodes (1,d), (1,b), (1,a) and (1,c) of (1,*) subtree of ordered by integer keys tree:

                     1,d
         0,c                     2,a
   0,d         1,b         2,b         3,a
0,b   0,e   0,a   1,a   1,c   2,c   3,c   3,b

  where comparator() callback - compares only integer parts of the keys */
BTREE_SUB_WALK_ANNOTATIONS
static inline struct btree_node *btree_walk_sub_recursive(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj/*!=NULL*/,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback);
	if (!callback(tree, obj))
		return btree_const_cast(tree);
	{
		struct btree_node *const n = btree_walk_sub_recursive_left(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	return btree_walk_sub_recursive_right(tree, key, comparator, obj, callback);
}

/* walk over left branch of unordered same-key subtree of ordered tree in forward direction */
/* note: same as btree_walk_sub_recursive_left(), but in forward direction, from the leftmost to the rightmost */
BTREE_SUB_WALK_ANNOTATIONS
static inline struct btree_node *btree_walk_sub_recursive_forward_left(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj/*!=NULL*/,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback);
	for (tree = tree->btree_left;; tree = tree->btree_right) {
		if (!tree)
			return btree_const_cast(tree); /* NULL */
		if (!comparator(tree, key))
			break;
	}
	{
		struct btree_node *const n = btree_walk_sub_recursive_forward_left(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!callback(tree, obj))
		return btree_const_cast(tree);
	return btree_walk_recursive_forward(tree->btree_right, obj, callback);
}

/* walk over right branch of unordered same-key subtree of ordered tree in forward direction */
/* note: same as btree_walk_sub_recursive_right(), but in forward direction, from the leftmost to the rightmost */
BTREE_SUB_WALK_ANNOTATIONS
static inline struct btree_node *btree_walk_sub_recursive_forward_right(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj/*!=NULL*/,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback);
	for (;;) {
		for (tree = tree->btree_right;; tree = tree->btree_left) {
			if (!tree)
				return btree_const_cast(tree); /* NULL */
			if (!comparator(tree, key))
				break;
		}
		{
			struct btree_node *const n = btree_walk_recursive_forward(tree->btree_left, obj, callback);
			if (n)
				return n;
		}
		if (!callback(tree, obj))
			return btree_const_cast(tree);
	}
}

/* walk over unordered same-key subtree of ordered tree in forward direction,
  walking stops if processor callback returns 0 for processed node
  - that node is returned as result of walking */
/* assume tree - result of previous btree_search() */
/* note: same as btree_walk_sub_recursive(), but in forward direction, from the leftmost to the rightmost */
BTREE_SUB_WALK_ANNOTATIONS
static inline struct btree_node *btree_walk_sub_recursive_forward(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj/*!=NULL*/,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback);
	{
		struct btree_node *const n = btree_walk_sub_recursive_forward_left(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!callback(tree, obj))
		return btree_const_cast(tree);
	return btree_walk_sub_recursive_forward_right(tree, key, comparator, obj, callback);
}

/* walk over right branch of unordered same-key subtree of ordered tree in backward direction */
/* note: same as btree_walk_sub_recursive_right(), but in backward direction, from the rightmost to the leftmost */
BTREE_SUB_WALK_ANNOTATIONS
static inline struct btree_node *btree_walk_sub_recursive_backward_right(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj/*!=NULL*/,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback);
	for (tree = tree->btree_right;; tree = tree->btree_left) {
		if (!tree)
			return btree_const_cast(tree); /* NULL */
		if (!comparator(tree, key))
			break;
	}
	{
		struct btree_node *const n = btree_walk_sub_recursive_backward_right(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!callback(tree, obj))
		return btree_const_cast(tree);
	return btree_walk_recursive_backward(tree->btree_left, obj, callback);
}

/* walk over left branch of unordered same-key subtree of ordered tree in backward direction */
/* note: same as btree_walk_sub_recursive_left(), but in backward direction, from the rightmost to the leftmost */
BTREE_SUB_WALK_ANNOTATIONS
static inline struct btree_node *btree_walk_sub_recursive_backward_left(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj/*!=NULL*/,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback);
	for (;;) {
		for (tree = tree->btree_left;; tree = tree->btree_right) {
			if (!tree)
				return btree_const_cast(tree); /* NULL */
			if (!comparator(tree, key))
				break;
		}
		{
			struct btree_node *const n = btree_walk_recursive_backward(tree->btree_right, obj, callback);
			if (n)
				return n;
		}
		if (!callback(tree, obj))
			return btree_const_cast(tree);
	}
}

/* walk over unordered same-key subtree of ordered tree in backward direction,
  walking stops if processor callback returns 0 for processed node
  - that node is returned as result of walking */
/* assume tree - result of previous btree_search() */
/* note: same as btree_walk_sub_recursive(), but in backward direction, from the rightmost to the leftmost */
BTREE_SUB_WALK_ANNOTATIONS
static inline struct btree_node *btree_walk_sub_recursive_backward(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj/*!=NULL*/,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_SUB_WALK_CHECK_PARAMS(tree, key, comparator, obj, callback);
	{
		struct btree_node *const n = btree_walk_sub_recursive_backward_right(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!callback(tree, obj))
		return btree_const_cast(tree);
	return btree_walk_sub_recursive_backward_left(tree, key, comparator, obj, callback);
}

#undef BTREE_SUB_WALK_ANNOTATIONS
#undef BTREE_SUB_WALK_CHECK_PARAMS

/* find leaf parent of to be inserted node in the same-key sub-tree */
/* returns:
  1 - if node should be added as left child of the parent,
 -1 - if node should be added as right child of the parent */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(p, A_In)
A_At(parent, A_Outptr)
A_Check_return
A_Ret_range(!=,0)
#endif
static inline int btree_find_leaf(
	struct btree_node *p/*!=NULL*/,
	struct btree_node **const parent/*out:!=NULL*/)
{
	btree_assert_ptr_(p);
	btree_assert_ptr_(parent);
	if (p->btree_right) {
		const struct btree_node *const left = p->btree_left;
		if (!left) {
			*parent = p;
			return 1; /* parent at right */
		}
		p = btree_last(left);
	}
	*parent = p;
	return -1; /* parent at left */
}

/* search leaf parent of to be inserted node in the tree ordered by abstract keys,
  initially parent references the root of the tree, may be NULL if tree is empty,
 returns:
  < 0 - if parent at left,
  > 0 - if parent at right,
    0 - if parent references node with the same key and 'leaf' is zero,
 NOTE: if tree allows nodes with non-unique keys, 'leaf' must be non-zero */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(parent, A_Inout)
A_At(*parent, A_In_opt
  A_When(A_Old(*parent), A_Post_notnull A_Post_valid)
  A_When(A_Old(!*parent), A_Post_null))
A_At(key, A_In)
A_At(comparator, A_In)
A_Check_return
A_When(!return, A_At(*parent, A_Post_notnull A_Post_valid))
#endif
static inline int btree_search_parent(
	struct btree_node **const parent/*in:*NULL?,out*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	const int leaf)
{
	btree_assert_ptr_(parent);
	btree_assert_ptr_(key);
	btree_assert_comparator_(comparator);
	{
		struct btree_node *p = *parent;
		if (!p)
			return 1; /* tree is empty, parent is NULL */
		for (;;) {
			struct btree_node *const p_ = p;
			const int c = comparator(p, key); /* c = p - key */
			if (c != 0) {
				p = p->leaves[c < 0];
				if (p)
					continue;
			}
			else if (leaf)
				return btree_find_leaf(p, parent);
			*parent = p_;
			return c; /* if 0, then (*parent) - references found node, else (*parent) - references leaf parent */
		}
	}
}

/* recursively count nodes in the tree */
/* Note: recursion may overflow the stack, if tree height is too big */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Pure_function
A_At(tree, A_In_opt)
A_Ret_range(>=,s)
A_Check_return
#endif
static inline size_t btree_size_(
	const struct btree_node *tree/*NULL?*/,
	size_t s)
{
	while (tree) {
		/* note: assume stack overflow will occur before integer overflow,
		  so do not check for possible integer overflow in 's + 1' */
		s = btree_size_(tree->btree_left, s + 1);
		tree = tree->btree_right;
	}
	return s;
}

/* recursively count nodes in the tree */
/* Note: recursion may overflow the stack, if tree height is too big */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Pure_function
A_At(tree, A_In_opt)
A_Check_return
#endif
static inline size_t btree_size(
	const struct btree_node *const tree/*NULL?*/)
{
	return btree_size_(tree, 0);
}

/* recursively determine tree height */
/* Note: recursion may overflow the stack, if tree height is too big */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Pure_function
A_At(tree, A_In_opt)
A_Check_return
#endif
static inline size_t btree_height(
	const struct btree_node *tree/*NULL?*/)
{
	if (!tree)
		return 0;
	{
		const size_t left_height = btree_height(tree->btree_left);
		const size_t right_height = btree_height(tree->btree_right);
		const size_t h = (left_height > right_height) ? left_height : right_height;
		/* note: assume stack overflow will occur before integer overflow,
		  so do not check for integer overflow in 'h + 1' */
		return h + 1;
	}
}

#ifdef __cplusplus
}
#endif

#endif /* BTREE_H_INCLUDED */
