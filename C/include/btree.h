#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

/**********************************************************************************
* Embedded binary tree
* Copyright (C) 2012-2022 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* btree.h */

/* BTREE_ASSUME - assume condition is always true */
#ifndef BTREE_ASSUME
#ifdef ASSUME
#define BTREE_ASSUME(cond) ASSUME(cond)
#elif defined _MSC_VER
#define BTREE_ASSUME(cond) __assume(!!(cond))
#elif defined __clang_analyzer__
#define BTREE_ASSUME(cond) ((void)(!(cond) ? __builtin_unreachable(), 0 : 1))
#elif defined __clang__
#define BTREE_ASSUME(cond) __builtin_assume(!!(cond))
#elif defined __INTEL_COMPILER
#define BTREE_ASSUME(cond) ((void)0) /* ICC compiles calls to __builtin_unreachable() as jumps somewhere... */
#elif defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define BTREE_ASSUME(cond) ((void)(!(cond) ? __builtin_unreachable(), 0 : 1))
#else
#define BTREE_ASSUME(cond) ((void)0) /* assume condition is always true */
#endif
#endif

/* expr - do not compares pointers */
#ifndef BTREE_ASSERT
#ifdef ASSERT
#define BTREE_ASSERT(expr) ASSERT(expr)
#else
#define BTREE_ASSERT(expr) BTREE_ASSUME(expr)
#endif
#endif

/* check that pointer is not NULL */
#ifndef BTREE_ASSERT_PTR
#ifdef ASSERT_PTR
#define BTREE_ASSERT_PTR(ptr) ASSERT_PTR(ptr)
#else
#define BTREE_ASSERT_PTR(ptr) BTREE_ASSERT(ptr)
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

#define BTREE_KEY_COMPARATOR1_(a) BTREE_KEY_COMPARATOR1 a
#define BTREE_KEY_COMPARATOR2_(a) BTREE_KEY_COMPARATOR2 a
#define BTREE_KEY_COMPARATOR3_(a) BTREE_KEY_COMPARATOR3 a
#define BTREE_KEY_COMPARATOR4_(a) BTREE_KEY_COMPARATOR4 a
#define BTREE_KEY_COMPARATOR5_(a) BTREE_KEY_COMPARATOR5 a
#define BTREE_KEY_COMPARATOR6_(a) BTREE_KEY_COMPARATOR6 a

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
#if defined __clang__ || (defined __GNUC__ && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 6)))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual" /* casting away qualifiers */
#endif
	return (struct btree_node*)n;
#if defined __clang__ || (defined __GNUC__ && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 6)))
#pragma GCC diagnostic pop
#endif
#endif /* !__cplusplus */
}

/* abstract key structure that is passed to binary tree comparator callback */
struct btree_key {
	char c_; /* placeholder, must be never accessed, char, so key may be arbitrary aligned */
};

/* binary tree comparator callback - compare node's key with given one,
  must return a difference (node - key), see BTREE_KEY_COMPARATOR() macro,
  if returns 0, search stops */
typedef int btree_comparator(
	const struct btree_node *node/*!=NULL*/,
	const struct btree_key *key/*!=NULL*/);

/* search node in the tree ordered by keys,
  returns NULL if node with given key was not found */
/* int comparator(node, key) - returns (node - key) difference */
static inline struct btree_node *btree_search(
	const struct btree_node *tree/*NULL?*/,
	const struct btree_key *const key/*!=NULL if tree!=NULL*/,
	btree_comparator *const comparator/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || key);
	BTREE_ASSERT(!tree || comparator);
	while (tree) {
		const int c = (*comparator)(tree, key); /* c = tree - key */
		if (c == 0)
			break;
		tree = tree->leaves[c < 0];
	}
	return btree_const_cast(tree); /* NULL? */
}

/* same as btree_search(), but implemented as macro:
  n   - struct btree_node *, initially root of the tree,
  cmp - arbitrary comparator expression using n, e.g.:
   BTREE_KEY_COMPARATOR(my_node_from_btree_node(n)->my_key, search_key) */
#define BTREE_SEARCH(n/*NULL?*/, cmp) do { \
	while (n) {                            \
		const int c = cmp;                 \
		if (c == 0)                        \
			break;                         \
		n = n->leaves[c < 0];              \
	}                                      \
} while (0)

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
typedef int btree_walker(
	const struct btree_node *node/*!=NULL*/,
	struct btree_object *obj);

/* walk over all nodes of unordered tree calling callback for each node,
  returns node on which callback has returned 0 */
static inline struct btree_node *btree_walk_recursive(
	const struct btree_node *tree/*NULL?*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || callback);
	for (; tree && (*callback)(tree, obj); tree = tree->btree_right) {
		struct btree_node *const left = btree_walk_recursive(tree->btree_left, obj, callback);
		if (left)
			return left;
	}
	return btree_const_cast(tree); /* NULL? */
}

/* same as btree_walk_recursive(), but in forward direction, from the leftmost to the rightmost */
static inline struct btree_node *btree_walk_recursive_forward(
	const struct btree_node *tree/*NULL?*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || callback);
	for (; tree; tree = tree->btree_right) {
		struct btree_node *const left = btree_walk_recursive_forward(tree->btree_left, obj, callback);
		if (left)
			return left;
		if (!(*callback)(tree, obj))
			break;
	}
	return btree_const_cast(tree); /* NULL? */
}

/* same as btree_walk_recursive(), but in backward direction, from the rightmost to the leftmost */
static inline struct btree_node *btree_walk_recursive_backward(
	const struct btree_node *tree/*NULL?*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || callback);
	for (; tree; tree = tree->btree_left) {
		struct btree_node *const right = btree_walk_recursive_backward(tree->btree_right, obj, callback);
		if (right)
			return right;
		if (!(*callback)(tree, obj))
			break;
	}
	return btree_const_cast(tree); /* NULL? */
}

/* binary tree deleter callback - delete binary tree node */
typedef void btree_deleter(
	struct btree_node *node/*!=NULL*/,
	struct btree_object *obj);

/* walk over all nodes of unordered tree calling callback to delete each node */
static inline void btree_delete_recursive(
	struct btree_node *tree/*NULL?*/,
	struct btree_object *const obj,
	btree_deleter *const deleter/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || deleter);
	while (tree) {
		struct btree_node *const right = tree->btree_right;
		struct btree_node *const left = tree->btree_left;
		(*deleter)(tree, obj);
		btree_delete_recursive(left, obj, deleter);
		tree = right;
	}
}

/* same as btree_delete_recursive(), but in forward direction, from the leftmost to the rightmost */
static inline void btree_delete_recursive_forward(
	struct btree_node *tree/*NULL?*/,
	struct btree_object *const obj,
	btree_deleter *const deleter/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || deleter);
	while (tree) {
		btree_delete_recursive_forward(tree->btree_left, obj, deleter);
		{
			struct btree_node *const right = tree->btree_right;
			(*deleter)(tree, obj);
			tree = right;
		}
	}
}

/* same as btree_delete_recursive(), but in backward direction, from the rightmost to the leftmost */
static inline void btree_delete_recursive_backward(
	struct btree_node *tree/*NULL?*/,
	struct btree_object *const obj,
	btree_deleter *const deleter/*!=NULL if tree!=NULL*/)
{
	BTREE_ASSERT(!tree || deleter);
	while (tree) {
		btree_delete_recursive_backward(tree->btree_right, obj, deleter);
		{
			struct btree_node *const left = tree->btree_left;
			(*deleter)(tree, obj);
			tree = left;
		}
	}
}

/* get the leftmost node of the tree */
static inline struct btree_node *btree_first(
	const struct btree_node *tree/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	for (;;) {
		const struct btree_node *const l = tree->btree_left;
		if (!l)
			break;
		tree = l;
	}
	return btree_const_cast(tree); /* !=NULL */
}

/* get the rightmost node of the tree */
static inline struct btree_node *btree_last(
	const struct btree_node *tree/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	for (;;) {
		const struct btree_node *const r = tree->btree_right;
		if (!r)
			break;
		tree = r;
	}
	return btree_const_cast(tree); /* !=NULL */
}

/* walk over left branch of unordered same-key subtree of ordered tree */
/* for example, walk over nodes (1,b) and (1,a) of (1,*) subtree of ordered by integer keys tree:

                     1,d
         0,c                     2,a
   0,d         1,b         2,b         3,a
0,b   0,e   0,a   1,a   1,c   2,c   3,c   3,b

  where comparator() callback - compares only integer parts of the keys */
static inline struct btree_node *btree_walk_sub_recursive_left(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	BTREE_ASSERT_PTR(callback);
	for (;;) {
		for (tree = tree->btree_left;; tree = tree->btree_right) {
			if (!tree)
				return btree_const_cast(tree); /* NULL */
			if (!(*comparator)(tree, key))
				break;
		}
		if (!(*callback)(tree, obj))
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
static inline struct btree_node *btree_walk_sub_recursive_right(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	BTREE_ASSERT_PTR(callback);
	for (;;) {
		for (tree = tree->btree_right;; tree = tree->btree_left) {
			if (!tree)
				return btree_const_cast(tree); /* NULL */
			if (!(*comparator)(tree, key))
				break;
		}
		if (!(*callback)(tree, obj))
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
static inline struct btree_node *btree_walk_sub_recursive(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	BTREE_ASSERT_PTR(callback);
	if (!(*callback)(tree, obj))
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
static inline struct btree_node *btree_walk_sub_recursive_forward_left(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	BTREE_ASSERT_PTR(callback);
	for (tree = tree->btree_left;; tree = tree->btree_right) {
		if (!tree)
			return btree_const_cast(tree); /* NULL */
		if (!(*comparator)(tree, key))
			break;
	}
	{
		struct btree_node *const n = btree_walk_sub_recursive_forward_left(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!(*callback)(tree, obj))
		return btree_const_cast(tree);
	return btree_walk_recursive_forward(tree->btree_right, obj, callback);
}

/* walk over right branch of unordered same-key subtree of ordered tree in forward direction */
/* note: same as btree_walk_sub_recursive_right(), but in forward direction, from the leftmost to the rightmost */
static inline struct btree_node *btree_walk_sub_recursive_forward_right(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	BTREE_ASSERT_PTR(callback);
	for (;;) {
		for (tree = tree->btree_right;; tree = tree->btree_left) {
			if (!tree)
				return btree_const_cast(tree); /* NULL */
			if (!(*comparator)(tree, key))
				break;
		}
		{
			struct btree_node *const n = btree_walk_recursive_forward(tree->btree_left, obj, callback);
			if (n)
				return n;
		}
		if (!(*callback)(tree, obj))
			return btree_const_cast(tree);
	}
}

/* walk over unordered same-key subtree of ordered tree in forward direction,
  walking stops if processor callback returns 0 for processed node
  - that node is returned as result of walking */
/* assume tree - result of previous btree_search() */
/* note: same as btree_walk_sub_recursive(), but in forward direction, from the leftmost to the rightmost */
static inline struct btree_node *btree_walk_sub_recursive_forward(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	BTREE_ASSERT_PTR(callback);
	{
		struct btree_node *const n = btree_walk_sub_recursive_forward_left(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!(*callback)(tree, obj))
		return btree_const_cast(tree);
	return btree_walk_sub_recursive_forward_right(tree, key, comparator, obj, callback);
}

/* walk over right branch of unordered same-key subtree of ordered tree in backward direction */
/* note: same as btree_walk_sub_recursive_right(), but in backward direction, from the rightmost to the leftmost */
static inline struct btree_node *btree_walk_sub_recursive_backward_right(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	BTREE_ASSERT_PTR(callback);
	for (tree = tree->btree_right;; tree = tree->btree_left) {
		if (!tree)
			return btree_const_cast(tree); /* NULL */
		if (!(*comparator)(tree, key))
			break;
	}
	{
		struct btree_node *const n = btree_walk_sub_recursive_backward_right(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!(*callback)(tree, obj))
		return btree_const_cast(tree);
	return btree_walk_recursive_backward(tree->btree_left, obj, callback);
}

/* walk over left branch of unordered same-key subtree of ordered tree in backward direction */
/* note: same as btree_walk_sub_recursive_left(), but in backward direction, from the rightmost to the leftmost */
static inline struct btree_node *btree_walk_sub_recursive_backward_left(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	BTREE_ASSERT_PTR(callback);
	for (;;) {
		for (tree = tree->btree_left;; tree = tree->btree_right) {
			if (!tree)
				return btree_const_cast(tree); /* NULL */
			if (!(*comparator)(tree, key))
				break;
		}
		{
			struct btree_node *const n = btree_walk_recursive_backward(tree->btree_right, obj, callback);
			if (n)
				return n;
		}
		if (!(*callback)(tree, obj))
			return btree_const_cast(tree);
	}
}

/* walk over unordered same-key subtree of ordered tree in backward direction,
  walking stops if processor callback returns 0 for processed node
  - that node is returned as result of walking */
/* assume tree - result of previous btree_search() */
/* note: same as btree_walk_sub_recursive(), but in backward direction, from the rightmost to the leftmost */
static inline struct btree_node *btree_walk_sub_recursive_backward(
	const struct btree_node *tree/*!=NULL*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	struct btree_object *const obj,
	btree_walker *const callback/*!=NULL*/)
{
	BTREE_ASSERT_PTR(tree);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	BTREE_ASSERT_PTR(callback);
	{
		struct btree_node *const n = btree_walk_sub_recursive_backward_right(tree, key, comparator, obj, callback);
		if (n)
			return n;
	}
	if (!(*callback)(tree, obj))
		return btree_const_cast(tree);
	return btree_walk_sub_recursive_backward_left(tree, key, comparator, obj, callback);
}

/* find leaf parent of to be inserted node in the same-key sub-tree */
/* returns:
  1 - if node should be added as left child of the parent,
 -1 - if node should be added as right child of the parent */
static inline int btree_find_leaf(
	struct btree_node *p/*!=NULL*/,
	struct btree_node **const parent/*out:!=NULL*/)
{
	BTREE_ASSERT_PTR(p);
	BTREE_ASSERT_PTR(parent);
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
static inline int btree_search_parent(
	struct btree_node **const parent/*in:*NULL?,out*/,
	const struct btree_key *const key/*!=NULL*/,
	btree_comparator *const comparator/*!=NULL*/,
	const int leaf)
{
	BTREE_ASSERT_PTR(parent);
	BTREE_ASSERT_PTR(key);
	BTREE_ASSERT_PTR(comparator);
	{
		struct btree_node *p = *parent;
		if (!p)
			return 1; /* tree is empty, parent is NULL */
		for (;;) {
			const int c = (*comparator)(p, key); /* c = p - key */
			struct btree_node *const p_ = p;
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

/* same as btree_search_parent(), but implemented as macro:
  c   - int, result of this search,
  p   - struct btree_node *, initially root of the tree,
  cmp - arbitrary comparator expression using p, e.g.:
   BTREE_KEY_COMPARATOR(my_node_from_btree_node(p)->my_key, search_key) */
/* NOTE: if tree allows nodes with non-unique keys, 'leaf' must be non-zero */
#define BTREE_SEARCH_PARENT(c, p, cmp, leaf) do {                                               \
	if (!p)                                                                                     \
		c = 1; /* tree is empty, parent is NULL */                                              \
	else for (;;) {                                                                             \
		c = cmp; /* c = (*parent) - key */                                                      \
		if (c != 0) {                                                                           \
			struct btree_node *const p_ = p->leaves[c < 0];                                     \
			if (p_) {                                                                           \
				p = p_;                                                                         \
				continue;                                                                       \
			}                                                                                   \
		}                                                                                       \
		else if (leaf) {                                                                        \
			struct btree_node *p_ = p;                                                          \
			c = btree_find_leaf(p_, &p_);                                                       \
			p = p_;                                                                             \
		}                                                                                       \
		break; /* if c == 0, then p - references found node, else p - references leaf parent */ \
	}                                                                                           \
} while (0)

/* recursively count nodes in the tree */
/* Note: recursion may overflow the stack, if tree height is too big */
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
static inline size_t btree_size(
	const struct btree_node *const tree/*NULL?*/)
{
	return btree_size_(tree, 0);
}

/* recursively determine tree height */
/* Note: recursion may overflow the stack, if tree height is too big */
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

/* maximum height of red-back tree containing 2^n nodes, e.g.:
  for 256   nodes -> max height 2*8  + 1 = 17,
  for 65536 nodes -> max height 2*16 + 1 = 33, etc. */
#define rbtree_height(n)   (2*(n) + 1)

/* walk over all nodes of unordered tree using stack, e.g.:
  size_t s;
  struct btree_node *stack[tree_height], *n;
  btree_walk_stack(tree, stack, s, n) {
    process(n);
  }
*/
#define btree_walk_stack(tree, stack, s, n) \
	for (s = 0, n = (tree); n; n = ( \
		n->btree_left ? (n->btree_right ? stack[s++] = n->btree_right : NULL), n->btree_left : \
		n->btree_right ? n->btree_right : s ? stack[--s] : NULL))

static inline struct btree_node *btree_fill_stack_left(
	const struct btree_node *tree/*!=NULL*/,
	const void *const stack,
	size_t *const s)
{
	while (tree->btree_left) {
		((const struct btree_node**)stack)[(*s)++] = tree;
		tree = tree->btree_left;
	}
	return btree_const_cast(tree);
}

static inline struct btree_node *btree_fill_stack_right(
	const struct btree_node *tree/*!=NULL*/,
	const void *const stack,
	size_t *const s)
{
	while (tree->btree_right) {
		((const struct btree_node**)stack)[(*s)++] = tree;
		tree = tree->btree_right;
	}
	return btree_const_cast(tree);
}

/* same as btree_walk_stack(), but in forward direction, from the leftmost to the rightmost */
#define btree_walk_stack_forward(tree, stack, s, n) \
	for (s = 0, n = (tree), n = n ? btree_fill_stack_left(n, stack, &s) : NULL; n; n = ( \
		n->btree_right ? btree_fill_stack_left(n->btree_right, stack, &s) : s ? stack[--s] : NULL))

/* same as btree_walk_stack(), but in backward direction, from the rightmost to the leftmost */
#define btree_walk_stack_backward(tree, stack, s, n) \
	for (s = 0, n = (tree), n = n ? btree_fill_stack_right(n, stack, &s) : NULL; n; n = ( \
		n->btree_left ? btree_fill_stack_right(n->btree_left, stack, &s) : s ? stack[--s] : NULL))

/* delete all nodes of unordered tree using stack, e.g.:
  size_t s;
  struct btree_node *stack[tree_height], *n, *next;
  btree_delete_stack(tree, stack, s, n, next) {
    delete(n);
  }
*/
#define btree_delete_stack(tree, stack, s, n, next) \
	for (s = 0, n = (tree); n ? next = ( \
		n->btree_left ? (n->btree_right ? stack[s++] = n->btree_right : NULL), n->btree_left : \
		n->btree_right ? n->btree_right : s ? stack[--s] : NULL), n : (next = NULL); n = next)

/* same as btree_delete_stack(), but in forward direction, from the leftmost to the rightmost */
#define btree_delete_stack_forward(tree, stack, s, n, next) \
	for (s = 0, n = (tree), n = n ? btree_fill_stack_left(n, stack, &s) : NULL; n ? next = ( \
		n->btree_right ? btree_fill_stack_left(n->btree_right, stack, &s) : s ? stack[--s] : NULL), n : (next = NULL); n = next)

/* same as btree_delete_stack(), but in backward direction, from the rightmost to the leftmost */
#define btree_delete_stack_backward(tree, stack, s, n, next) \
	for (s = 0, n = (tree), n = n ? btree_fill_stack_right(n, stack, &s) : NULL; n ? next = ( \
		n->btree_left ? btree_fill_stack_right(n->btree_left, stack, &s) : s ? stack[--s] : NULL), n : (next = NULL); n = next)

#ifdef __cplusplus
}
#endif

#endif /* BTREE_H_INCLUDED */
