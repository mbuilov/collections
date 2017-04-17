/* rbtest.cpp */

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4350)
#pragma warning (disable:4548)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <map>

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef _DEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#include "prbtree.h"

#define ORDER 2
#define __MUL(n)    1e##n
#define _MUL(n)    __MUL(n)
#define MULTIPLIER ((unsigned)_MUL(ORDER))

//#define USE_STDMAP
#define RBTREE_CHECK
//#define RBTREE_PRINT
//#define RBTREE_PRINT_REMOVE_NOT_FOUND

static FILE *out = NULL;

typedef int v_t;

typedef struct {
	v_t a;
	v_t b;
	v_t c;
} tree_key_t;

#ifdef USE_STDMAP
struct key_comp {
	bool operator() (const tree_key_t &a1, const tree_key_t &a2) const {
		return
			a1.a > a2.a || (a1.a == a2.a && (
			a1.b > a2.b || (a1.b == a2.b && (
			a1.c > a2.c))));
	}
};
#else
union my_key {
	tree_key_t k;
	struct _btree_key _bk;
};
#endif

struct A {
#ifndef USE_STDMAP
	struct _prbtree_node n;
	tree_key_t key;
#endif
	char c;
};

#ifndef USE_STDMAP
static inline const struct A *_const_node_to_A(const struct _btree_node *node)
{
	union {
		const struct _btree_node *n;
		const struct A *a;
	} u = {node};
	return u.a;
}

static inline struct A *_node_to_A(struct _btree_node *node)
{
	union {
		struct _btree_node *n;
		struct A *a;
	} u = {node};
	return u.a;
}

static inline const union my_key *_key_to_my_key(const struct _btree_key *key)
{
	union {
		const struct _btree_key *bk;
		const union my_key *k;
	} u = {key};
	return u.k;
}

static inline const struct _btree_key *_key_to_btree_key(const tree_key_t *key)
{
	union {
		const tree_key_t *tk;
		const struct _btree_key *bk;
	} u = {key};
	return u.bk;
}

static inline int key_comparator(const struct _btree_node *node, const struct _btree_key *key)
{
	const union my_key *k = _key_to_my_key(key);
	const struct A *a = _const_node_to_A(node);
	return BTREE_KEY_COMPARATOR(a->key.a, k->k.a, a->key.b, k->k.b, a->key.c, k->k.c);
}

#ifdef RBTREE_PRINT

static void print_offs(unsigned offs)
{
	while (offs--)
		fprintf(out, " ");
}

/*
       1
   1       1
 1   1   1   1
1 1 1 1 1 1 1 1
*/
static unsigned print_tree_line(const struct _btree_node *tree, unsigned l, unsigned n, unsigned offs, unsigned height, unsigned pos)
{
	if (tree) {
		if (l == n) {
			//unsigned gap = (offs - 1)*(ORDER + 6) - pos;
			unsigned gap = (offs - 1)*(ORDER + 3) - pos;
			const struct A *a = _const_node_to_A(tree);
			print_offs(gap);
			//pos += gap + fprintf(out, "{%d,%d,%d:%d:%x}",
				//a->key.k.a, a->key.k.b, a->key.k.c, (int)PRB_GET_COLOR(&a->n), 0xFFFF & (uintptr_t)&a->n);
			pos += gap + (unsigned)fprintf(out, "{%u:%x}", PRB_GET_COLOR(&a->n),
				(unsigned)(0xFFFFu & ((const char*)&a->n - (const char*)0)));
		}
		else {
			pos = print_tree_line(tree->btree_left, l + 1, n, offs - (1u << (height - 1)), height - 1, pos);
			pos = print_tree_line(tree->btree_right, l + 1, n, offs + (1u << (height - 1)), height - 1, pos);
		}
	}
	return pos;
}

static unsigned print_tree(const struct _btree_node *tree)
{
	if (tree) {
		unsigned height = (unsigned)_btree_height(tree);
		unsigned k = 0;
		for (; k < height; k++) {
			print_tree_line(tree, 0, k, (1u << (height - 1)), height - 1, 0);
			fprintf(out, "\n");
		}
		return height;
	}
	fprintf(out, "zero!\n");
	return 0;
}
#endif /* RBTREE_PRINT */

#ifdef RBTREE_CHECK
#ifndef _DEBUG
#ifdef __GNUC__
__attribute__ ((pure))
#endif
#endif
static void check_right_order(const struct _btree_node *left, const struct _btree_node *right)
{
	ASSERT(key_comparator(left, _key_to_btree_key(&_const_node_to_A(right)->key)) < 0);
	ASSERT(key_comparator(right, _key_to_btree_key(&_const_node_to_A(left)->key)) > 0);
	(void)left, (void)right;
}
static unsigned check_tree(struct _btree_node *tree, int parent_is_red)
{
	if (tree) {
		/* red parent must have black children */
		ASSERT(!parent_is_red || PRB_BLACK_COLOR == PRB_GET_COLOR(_prbtree_node_from_btree_node(tree)));
		/* red parent must have either both (black) children != NULL, ot both == NULL */
		ASSERT(PRB_BLACK_COLOR == PRB_GET_COLOR(_prbtree_node_from_btree_node(tree)) || (!!tree->btree_left == !!tree->btree_right));
		if (tree->btree_left) {
			ASSERT(tree == &PRB_GET_PARENT(_prbtree_node_from_btree_node(tree->btree_left))->u.n);
			check_right_order(tree->btree_left, tree);
		}
		if (tree->btree_right) {
			ASSERT(tree == &PRB_GET_PARENT(_prbtree_node_from_btree_node(tree->btree_right))->u.n);
			check_right_order(tree, tree->btree_right);
		}
		if (tree->btree_left && tree->btree_right)
			check_right_order(tree->btree_left, tree->btree_right);
		{
			/* compute number of black nodes */
			unsigned bc_left  = check_tree(tree->btree_left,  PRB_BLACK_COLOR != PRB_GET_COLOR(_prbtree_node_from_btree_node(tree)));
			unsigned bc_right = check_tree(tree->btree_right, PRB_BLACK_COLOR != PRB_GET_COLOR(_prbtree_node_from_btree_node(tree)));
			if (bc_left != bc_right)
				fprintf(out, "key = {%d,%d,%d}, bc_left = %u, bc_right = %u!\n",
					_node_to_A(tree)->key.a, _node_to_A(tree)->key.b, _node_to_A(tree)->key.c,
					bc_left, bc_right);
			ASSERT(bc_left == bc_right);
			return bc_left + (PRB_BLACK_COLOR == PRB_GET_COLOR(_prbtree_node_from_btree_node(tree)));
		}
	}
	(void)parent_is_red;
	return 0;
}
#endif /* RBTREE_CHECK */

static void clear_tree(struct _btree_node *tree)
{
	if (tree) {
		clear_tree(tree->btree_left);
		clear_tree(tree->btree_right);
		free(_node_to_A(tree));
	}
}
#endif /* !USE_STDMAP */

#ifndef USE_STDMAP
static inline int rb_insert(struct _prbtree *tree, const tree_key_t *key, unsigned count)
{
	struct A *a = (struct A*)malloc(sizeof(*a));
	if (!a) {
#ifdef RBTREE_PRINT
		fprintf(out, "----------------------------------------------failed to alloc node!\n");
#endif
		return 0;
	}
	_prbtree_init_node(&a->n);
	a->key = *key;
	a->c = 'a';
	(void)count;
	{
#ifdef RBTREE_PRINT
		unsigned height = 0;
#endif
		struct _btree_node *parent = _btree_const_cast(_prbtree_node_to_btree_node(tree->root)); /* NULL? */
		int parent_found = _btree_search_parent(&parent, _key_to_btree_key(&a->key), key_comparator, /*allow_duplicates:*/0);
		if (parent_found) {
			_prbtree_insert(tree, _prbtree_node_from_btree_node(parent/*NULL?*/), &a->n, parent_found);
#ifdef RBTREE_PRINT
			height = print_tree(&tree->root->u.n);
#endif
		}
#ifdef RBTREE_CHECK
		if (parent_found) {
			ASSERT(!tree->root || PRB_BLACK_COLOR == PRB_GET_COLOR(tree->root)); /* root must be black */
			check_tree(_prbtree_node_to_btree_node(tree->root), /*parent_is_red:*/1);
		}
#endif
		if (!parent_found) {
#ifdef RBTREE_PRINT
			fprintf(out, "------------------found existing! key={%d,%d,%d}, n={%u:%x}\n", key.a, key.b, key.c,
				PRB_GET_COLOR(_prbtree_node_from_btree_node(parent)), (unsigned)(0xFFFFu & (uintptr_t)parent));
#endif
			free(a);
			return 0;
		}
		else {
#ifdef RBTREE_PRINT
			fprintf(out, "-------------height=%u, added key={%d,%d,%d}, n={%u:%x}, count=%u\n", height,
				key.a, key.b, key.c, PRB_GET_COLOR(&a->n), (unsigned)(0xFFFFu & (uintptr_t)&a->n), count);
#endif
			return 1;
		}
	}
}

static inline int rb_remove(struct _prbtree *tree, const tree_key_t *key, unsigned count)
{
	(void)count;
	{
#ifdef RBTREE_PRINT
		unsigned height = 0;
#endif
		struct _btree_node *n = _btree_const_cast(_btree_search(
			_prbtree_node_to_btree_node(tree->root/*NULL?*/), _key_to_btree_key(key), key_comparator));
		if (n) {
#ifdef RBTREE_PRINT
			fprintf(out, "--------------------removing key={%d,%d,%d}, n={%u:%x}, count=%u\n", key.a, key.b, key.c,
				PRB_GET_COLOR(_prbtree_node_from_btree_node(n)), (unsigned)(0xFFFFu & (uintptr_t)n), count);
#endif
			_prbtree_remove(tree, _prbtree_node_from_btree_node(n));
		}
#ifdef RBTREE_PRINT
#ifndef RBTREE_PRINT_REMOVE_NOT_FOUND
		if (n)
#endif
			height = print_tree(_prbtree_node_to_btree_node(tree->root));
#endif
#ifdef RBTREE_CHECK
		if (n) {
			ASSERT(!tree->root || PRB_BLACK_COLOR == PRB_GET_COLOR(tree->root)); /* root must be black */
			check_tree(_prbtree_node_to_btree_node(tree->root), /*parent_is_red:*/1);
		}
#endif
		if (n) {
			struct A *a = _node_to_A(n);
#ifdef RBTREE_PRINT
			fprintf(out, "--------------------height=%u, removed key={%d,%d,%d}, n={%u:%x}, count=%u\n", height,
				key.a, key.b, key.c, PRB_GET_COLOR(&a->n), (unsigned)(0xFFFFu & (uintptr_t)&a->n), count);
#endif
			free(a);
			return 1;
		}
		else {
#ifdef RBTREE_PRINT
#ifdef RBTREE_PRINT_REMOVE_NOT_FOUND
			fprintf(out, "----------------------------------------------not found! key={%d,%d,%d}\n", key.a, key.b, key.c);
#endif
#endif
			return 0;
		}
	}
}
#endif /* !USE_STDMAP */

int main(int argc, char *argv[])
{
	unsigned insert_count = 0;
	unsigned remove_count = 0;
	if (argc > 1) {
		out = fopen(argv[1], "w");
		if (!out) {
			fprintf(out, "cannot open '%s' for writing\n", argv[1]);
			return -1;
		}
	}
	else
		out = stdout;
#ifdef USE_STDMAP
	std::map<tree_key_t, struct A, key_comp> std_tree;
#else
	struct _prbtree tree;
	_prbtree_init(&tree);
#endif
	srand(0);
#ifdef USE_STDMAP
	fprintf(out, "stdmap\n");
#else
	fprintf(out, "rbtree\n");
#endif
	{
		unsigned i = 0;
		unsigned count = 0;
		unsigned max_count = 0;
		unsigned clear = 1;
		for (; i < 2*MULTIPLIER; i++) {
			tree_key_t key;
			key.a = (v_t)(((65535u & (unsigned)rand()) + i*3) % (2*MULTIPLIER + 3));
			if (!(key.a % 2) || !(key.a % 3))
				key.a = 0;
			key.b = (v_t)(((65535u & (unsigned)rand()) + i*3) % (2*MULTIPLIER + 3));
			if (!(key.b % 2) || !(key.b % 3))
				key.b = 0;
			key.c = (v_t)(((65535u & (unsigned)rand()) + i*3) % (2*MULTIPLIER + 3));
			if (!(key.c % 2) || !(key.c % 3))
				key.c = 0;
			insert_count++;
#ifdef USE_STDMAP
			struct A a = {'a'};
			std::pair<std::map<tree_key_t, struct A, key_comp>::iterator, bool> p =
				std_tree.insert(std::pair<tree_key_t, struct A>(key, a));
			if (p.second)
#else
			if (rb_insert(&tree, &key, count))
#endif
			{
				count++;
				if (count > max_count)
					max_count = count;
			}
			{
				unsigned j = ((65535u & (unsigned)rand()) + i*3)%15;
				while (count && (!(clear % (16 << ORDER)) || j)) {
					key.a = (v_t)(((65535u & (unsigned)rand()) + i*3) % (2*MULTIPLIER + 7));
					if (!(key.a % 2) || !(key.a % 3))
						key.a = 0;
					key.b = (v_t)(((65535u & (unsigned)rand()) + i*3) % (2*MULTIPLIER + 7));
					if (!(key.b % 2) || !(key.b % 3))
						key.b = 0;
					key.c = (v_t)(((65535u & (unsigned)rand()) + i*3) % (2*MULTIPLIER + 7));
					if (!(key.c % 2) || !(key.c % 3))
						key.c = 0;
					remove_count++;
#ifdef USE_STDMAP
					if (std_tree.erase(key))
#else
					if (rb_remove(&tree, &key, count))
#endif
					{
						count--;
					}
					if (j)
						j--;
				}
			}
			clear++;
		}
#ifndef USE_STDMAP
		clear_tree(_prbtree_node_to_btree_node(tree.root));
#endif
		fprintf(out, "max_count=%u\n", max_count);
	}
	if (out != stdout)
		fclose(out);
	printf("insert_count=%u\n", insert_count);
	printf("remove_count=%u\n", remove_count);
	return 0;
}
