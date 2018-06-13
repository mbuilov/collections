/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2012-2018 Michael M. Builov, https://github.com/mbuilov/collections
**********************************************************************************/

/* rbtest.cpp */

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4350)
#pragma warning (disable:4548)
#pragma warning (disable:4987)
#pragma warning (disable:4365)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <map>

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef NDEBUG
#define ASSERT(x) ((void)0)
#else
#define ASSERT(x) assert(x)
#endif

#include "prbtree.h"

#define PRB_BLACK_COLOR 0u

#define ORDER 6
#define MUL_1(n)   1e##n
#define MUL_(n)    MUL_1(n)
#define MULTIPLIER ((unsigned)MUL_(ORDER))

// cl
// PRBTREE: mt/md - 0m4,010s/0m5,780s
// STDMAP:  mt/md - 0m4,380s/0m6,190s

// gcc
// PRBTREE: 0m1,840s 0m1,825s 0m0,015s
// STDMAP:  0m1,930s 0m1,918s 0m0,015s

//#define USE_STDMAP
//#define RBTREE_CHECK
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
			a1.a < a2.a || (a1.a == a2.a && (
			a1.b < a2.b || (a1.b == a2.b && (
			a1.c < a2.c))));
	}
};
#endif

struct A {
#ifndef USE_STDMAP
	struct prbtree_node n;
	tree_key_t key;
#endif
	char c;
};

#ifndef USE_STDMAP
static inline struct A *node_to_A(const struct btree_node *node)
{
	void *n = btree_const_cast(node);
	return (struct A*)n;
}

static inline const tree_key_t *key_to_my_key(const struct btree_key *key)
{
	const void *k = key;
	return (const tree_key_t*)k;
}

static inline const struct btree_key *key_to_btree_key(const tree_key_t *key)
{
	const void *k = key;
	return (const struct btree_key*)k;
}

static inline int key_comparator(const struct btree_node *node, const struct btree_key *key)
{
	const tree_key_t *k = key_to_my_key(key);
	const struct A *a = node_to_A(node);
	return BTREE_KEY_COMPARATOR(a->key.a, k->a, a->key.b, k->b, a->key.c, k->c);
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
static unsigned print_tree_line(const struct btree_node *tree, unsigned l, unsigned n, unsigned offs, unsigned height, unsigned pos)
{
	if (tree) {
		if (l == n) {
			//unsigned gap = (offs - 1)*(ORDER + 6) - pos;
			unsigned gap = (offs - 1)*(ORDER + 3) - pos;
			const struct A *a = node_to_A(tree);
			print_offs(gap);
			//pos += gap + fprintf(out, "{%d,%d,%d:%d:%x}",
				//a->key.k.a, a->key.k.b, a->key.k.c, (int)prbtree_get_color_(&a->n), 0xFFFF & (uintptr_t)&a->n);
			pos += gap + (unsigned)fprintf(out, "{%u:%x}", prbtree_get_color_(&a->n),
				(unsigned)(0xFFFFu & ((const char*)&a->n - (const char*)0)));
		}
		else {
			pos = print_tree_line(tree->btree_left, l + 1, n, offs - (1u << (height - 1)), height - 1, pos);
			pos = print_tree_line(tree->btree_right, l + 1, n, offs + (1u << (height - 1)), height - 1, pos);
		}
	}
	return pos;
}

static unsigned print_tree(const struct btree_node *tree)
{
	if (tree) {
		unsigned height = (unsigned)btree_height(tree);
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
/*__attribute__ ((pure))*/
#endif
#endif
static void check_right_order(const struct btree_node *left, const struct btree_node *right)
{
	ASSERT(key_comparator(left, key_to_btree_key(&node_to_A(right)->key)) < 0);
	ASSERT(key_comparator(right, key_to_btree_key(&node_to_A(left)->key)) > 0);
	(void)left, (void)right;
}
static unsigned check_tree(struct btree_node *tree, int parent_is_red)
{
	if (tree) {
		/* red parent must have black children */
		ASSERT(!parent_is_red || PRB_BLACK_COLOR == prbtree_get_color_(prbtree_node_from_btree_node_(tree)));
		/* red parent must have either both (black) children != NULL, ot both == NULL */
		ASSERT(PRB_BLACK_COLOR == prbtree_get_color_(prbtree_node_from_btree_node_(tree)) || (!!tree->btree_left == !!tree->btree_right));
		if (tree->btree_left) {
			ASSERT(tree == &prbtree_get_parent(prbtree_node_from_btree_node_(tree->btree_left))->u.n);
			check_right_order(tree->btree_left, tree);
		}
		if (tree->btree_right) {
			ASSERT(tree == &prbtree_get_parent(prbtree_node_from_btree_node_(tree->btree_right))->u.n);
			check_right_order(tree, tree->btree_right);
		}
		if (tree->btree_left && tree->btree_right)
			check_right_order(tree->btree_left, tree->btree_right);
		{
			/* compute number of black nodes */
			unsigned bc_left  = check_tree(tree->btree_left,  PRB_BLACK_COLOR != prbtree_get_color_(prbtree_node_from_btree_node_(tree)));
			unsigned bc_right = check_tree(tree->btree_right, PRB_BLACK_COLOR != prbtree_get_color_(prbtree_node_from_btree_node_(tree)));
			if (bc_left != bc_right) {
				fprintf(out, "key = {%d,%d,%d}, bc_left = %u, bc_right = %u!\n",
					node_to_A(tree)->key.a, node_to_A(tree)->key.b, node_to_A(tree)->key.c,
					bc_left, bc_right);
			}
			ASSERT(bc_left == bc_right);
			return bc_left + (PRB_BLACK_COLOR == prbtree_get_color_(prbtree_node_from_btree_node_(tree)));
		}
	}
	(void)parent_is_red;
	return 0;
}
#endif /* RBTREE_CHECK */

static void clear_tree(struct btree_node *tree)
{
	if (tree) {
		clear_tree(tree->btree_left);
		clear_tree(tree->btree_right);
		free(node_to_A(tree));
	}
}
#endif /* !USE_STDMAP */

#ifndef USE_STDMAP
static inline int rb_insert(struct prbtree *tree, const tree_key_t *key, unsigned count)
{
	struct A *a = (struct A*)malloc(sizeof(*a));
	if (!a) {
#ifdef RBTREE_PRINT
		fprintf(out, "----------------------------------------------failed to alloc node!\n");
#endif
		return 0;
	}
	prbtree_init_node(&a->n);
	a->key = *key;
	a->c = 'a';
	(void)count;
	{
#ifdef RBTREE_PRINT
		unsigned height = 0;
#endif
		struct btree_node *parent = prbtree_node_to_btree_node_(tree->root); /* NULL? */
		int parent_found = btree_search_parent(&parent, key_to_btree_key(&a->key), key_comparator, /*allow_duplicates:*/0);
		if (parent_found) {
			prbtree_insert(tree, prbtree_node_from_btree_node_(parent/*NULL?*/), &a->n, parent_found);
#ifdef RBTREE_PRINT
			height = print_tree(&tree->root->u.n);
#endif
		}
#ifdef RBTREE_CHECK
		if (parent_found) {
			ASSERT(!tree->root || PRB_BLACK_COLOR == prbtree_get_color_(tree->root)); /* root must be black */
			check_tree(prbtree_node_to_btree_node_(tree->root), /*parent_is_red:*/1);
		}
#endif
		if (!parent_found) {
#ifdef RBTREE_PRINT
			fprintf(out, "------------------found existing! key={%d,%d,%d}, n={%u:%x}\n", key->a, key->b, key->c,
				prbtree_get_color_(prbtree_node_from_btree_node_(parent)), (unsigned)(0xFFFFu & (uintptr_t)parent));
#endif
			free(a);
			return 0;
		}
		else {
#ifdef RBTREE_PRINT
			fprintf(out, "-------------height=%u, added key={%d,%d,%d}, n={%u:%x}, count=%u\n", height,
				key->a, key->b, key->c, prbtree_get_color_(&a->n), (unsigned)(0xFFFFu & (uintptr_t)&a->n), count);
#endif
			return 1;
		}
	}
}

static inline int rb_remove(struct prbtree *tree, const tree_key_t *key, unsigned count)
{
	(void)count;
	{
#ifdef RBTREE_PRINT
		unsigned height = 0;
#endif
		struct btree_node *n = btree_search(
			prbtree_node_to_btree_node_(tree->root/*NULL?*/), key_to_btree_key(key), key_comparator);
		if (n) {
#ifdef RBTREE_PRINT
			fprintf(out, "--------------------removing key={%d,%d,%d}, n={%u:%x}, count=%u\n", key->a, key->b, key->c,
				prbtree_get_color_(prbtree_node_from_btree_node_(n)), (unsigned)(0xFFFFu & (uintptr_t)n), count);
#endif
			prbtree_remove(tree, prbtree_node_from_btree_node_(n));
		}
#ifdef RBTREE_PRINT
#ifndef RBTREE_PRINT_REMOVE_NOT_FOUND
		if (n)
#endif
			height = print_tree(prbtree_node_to_btree_node_(tree->root));
#endif
#ifdef RBTREE_CHECK
		if (n) {
			ASSERT(!tree->root || PRB_BLACK_COLOR == prbtree_get_color_(tree->root)); /* root must be black */
			check_tree(prbtree_node_to_btree_node_(tree->root), /*parent_is_red:*/1);
		}
#endif
		if (n) {
			struct A *a = node_to_A(n);
#ifdef RBTREE_PRINT
			fprintf(out, "--------------------height=%u, removed key={%d,%d,%d}, n={%u:%x}, count=%u\n", height,
				key->a, key->b, key->c, prbtree_get_color_(&a->n), (unsigned)(0xFFFFu & (uintptr_t)&a->n), count);
#endif
			free(a);
			return 1;
		}
		else {
#ifdef RBTREE_PRINT
#ifdef RBTREE_PRINT_REMOVE_NOT_FOUND
			fprintf(out, "----------------------------------------------not found! key={%d,%d,%d}\n", key->a, key->b, key->c);
#endif
#endif
			return 0;
		}
	}
}
#endif /* !USE_STDMAP */

static int rrr(void)
{
	return rand();
}

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
	struct prbtree tree;
	prbtree_init(&tree);
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
			key.a = (v_t)(((65535u & (unsigned)rrr()) + i*3) % (2*MULTIPLIER + 3));
			if (!(key.a % 2) || !(key.a % 3))
				key.a = 0;
			key.b = (v_t)(((65535u & (unsigned)rrr()) + i*3) % (2*MULTIPLIER + 3));
			if (!(key.b % 2) || !(key.b % 3))
				key.b = 0;
			key.c = (v_t)(((65535u & (unsigned)rrr()) + i*3) % (2*MULTIPLIER + 3));
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
				unsigned fails = 0;
				unsigned j = ((65535u & (unsigned)rrr()) + i*3)%15;
				while (count && (!(clear % (16 << ORDER)) || j)) {
					if (fails < 16) {
						key.a = (v_t)(((65535u & (unsigned)rrr()) + i*3) % (2*MULTIPLIER + 7));
						if (!(key.a % 2) || !(key.a % 3))
							key.a = 0;
						key.b = (v_t)(((65535u & (unsigned)rrr()) + i*3) % (2*MULTIPLIER + 7));
						if (!(key.b % 2) || !(key.b % 3))
							key.b = 0;
						key.c = (v_t)(((65535u & (unsigned)rrr()) + i*3) % (2*MULTIPLIER + 7));
						if (!(key.c % 2) || !(key.c % 3))
							key.c = 0;
					}
					else {
						unsigned pos = (unsigned)rrr() % count;
#ifdef USE_STDMAP
						std::map<tree_key_t, struct A, key_comp>::iterator it = std_tree.begin();
						while (pos) {
							it++;
							pos--;
						}
						key.a = it->first.a;
						key.b = it->first.b;
						key.c = it->first.c;
#else
						struct btree_node *n = btree_first(&tree.root->u.n);
						while (pos) {
							n = &prbtree_next(&node_to_A(n)->n)->u.n;
							pos--;
						}
						{
							struct A *a = node_to_A(n);
							key.a = a->key.a;
							key.b = a->key.b;
							key.c = a->key.c;
						}
#endif
						fails = 0;
					}
					remove_count++;
#ifdef USE_STDMAP
					if (std_tree.erase(key))
#else
					if (rb_remove(&tree, &key, count))
#endif
					{
						count--;
					}
					else
						fails++;
					if (j)
						j--;
				}
			}
			clear++;
		}
#ifndef USE_STDMAP
		clear_tree(prbtree_node_to_btree_node_(tree.root));
#endif
		fprintf(out, "max_count=%u\n", max_count);
	}
	if (out != stdout)
		fclose(out);
	printf("insert_count=%u\n", insert_count);
	printf("remove_count=%u\n", remove_count);
	return 0;
}
