/**********************************************************************************
* Embedded binary tree
* Copyright (C) 2022 Michael M. Builov, https://github.com/mbuilov/collections
**********************************************************************************/

/* test.c */

#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

static unsigned allocated = 0;
static unsigned test_number = 0;

#define TEST(expr) do { \
	if (!(expr)) { \
		printf("test %u failed (at line = %d)\n", test_number, __LINE__); \
		return 1; \
	} \
	printf("test %u ok\n", test_number); \
	test_number++; \
} while (0)

/*      8
   4         12
 2   6    10    14
1 3 5 7  9 11 13 15  */

struct tree_node {
	struct btree_node node;
	unsigned key;
};

static struct tree_node n1  = {{{NULL,NULL}}, 1};
static struct tree_node n3  = {{{NULL,NULL}}, 3};
static struct tree_node n5  = {{{NULL,NULL}}, 5};
static struct tree_node n7  = {{{NULL,NULL}}, 7};
static struct tree_node n9  = {{{NULL,NULL}}, 9};
static struct tree_node n11 = {{{NULL,NULL}},11};
static struct tree_node n13 = {{{NULL,NULL}},13};
static struct tree_node n15 = {{{NULL,NULL}},15};
static struct tree_node n2  = {{{&n1.node, &n3.node}},  2};
static struct tree_node n6  = {{{&n5.node, &n7.node}},  6};
static struct tree_node n10 = {{{&n9.node, &n11.node}},10};
static struct tree_node n14 = {{{&n13.node,&n15.node}},14};
static struct tree_node n4  = {{{&n2.node, &n6.node}},  4};
static struct tree_node n12 = {{{&n10.node,&n14.node}},12};
static struct tree_node n8  = {{{&n4.node, &n12.node}}, 8};
static struct btree_node *const tree = &n8.node;

static struct tree_node *alloc_node(unsigned key)
{
	struct tree_node *const n = (struct tree_node*)malloc(sizeof(*n));
	if (n) {
		allocated++;
		n->key = key;
		n->node.btree_left = NULL;
		n->node.btree_right = NULL;
	}
	return n;
}

static void free_node(struct tree_node *const n)
{
	free(n);
	allocated--;
}

static struct btree_node *dynamic_tree(void)
{
	struct tree_node *nodes[15];
	unsigned i = 0;
	for (; i < sizeof(nodes)/sizeof(nodes[0]); i++) {
		if (NULL == (nodes[i] = alloc_node(i + 1)))
			return NULL;
	}
	nodes[1]->node.btree_left   = &nodes[0]->node;
	nodes[1]->node.btree_right  = &nodes[2]->node;
	nodes[5]->node.btree_left   = &nodes[4]->node;
	nodes[5]->node.btree_right  = &nodes[6]->node;
	nodes[9]->node.btree_left   = &nodes[8]->node;
	nodes[9]->node.btree_right  = &nodes[10]->node;
	nodes[13]->node.btree_left  = &nodes[12]->node;
	nodes[13]->node.btree_right = &nodes[14]->node;
	nodes[3]->node.btree_left   = &nodes[1]->node;
	nodes[3]->node.btree_right  = &nodes[5]->node;
	nodes[11]->node.btree_left  = &nodes[9]->node;
	nodes[11]->node.btree_right = &nodes[13]->node;
	nodes[7]->node.btree_left   = &nodes[3]->node;
	nodes[7]->node.btree_right  = &nodes[11]->node;
	return &nodes[7]->node;
}

static inline struct tree_node *tree_node_from_btree_node(const struct btree_node *const n)
{
	void *const n_ = btree_const_cast(n);
	return (struct tree_node*)n_;
}

union walk_test1_data {
	struct {
		unsigned size;
		unsigned filled;
		unsigned keys[1];
	} s;
	struct btree_object obj;
};

static inline int test1_walker(
	const struct btree_node *node/*!=NULL*/,
	struct btree_object *obj)
{
	void *const o_ = obj;
	const struct tree_node *const n = tree_node_from_btree_node(node);
	union walk_test1_data *const d = (union walk_test1_data*)o_;
	if (d->s.filled == d->s.size) {
		printf("out of array: filled = %u!\n", d->s.filled);
		return 0; /* stop */
	}
	d->s.keys[d->s.filled++] = n->key;
	return 1;
}

static inline void test_deleter1(
	struct btree_node *node/*!=NULL*/,
	struct btree_object *obj)
{
	void *const o_ = obj;
	struct tree_node *const n = tree_node_from_btree_node(node);
	union walk_test1_data *const d = (union walk_test1_data*)o_;
	if (d->s.filled == d->s.size)
		printf("out of array: filled = %u!\n", d->s.filled);
	else
		d->s.keys[d->s.filled++] = n->key;
	free_node(n);
}

union search_test1_key {
	unsigned k;
	struct btree_key key;
};

static inline int test1_comparator(
	const struct btree_node *node/*!=NULL*/,
	const struct btree_key *key/*!=NULL*/)
{
	const void *const k_ = key;
	const struct tree_node *const n = tree_node_from_btree_node(node);
	const union search_test1_key *const k = (const union search_test1_key*)k_;
	const int c = BTREE_KEY_COMPARATOR(n->key, k->k);
	return c;
}

union range_search_test1_key {
	struct {
		unsigned start;
		unsigned end;
	} s;
	struct btree_key key;
};

static inline int test1_range_comparator(
	const struct btree_node *node/*!=NULL*/,
	const struct btree_key *key/*!=NULL*/)
{
	const void *const k_ = key;
	const struct tree_node *const n = tree_node_from_btree_node(node);
	const union range_search_test1_key *const k = (const union range_search_test1_key*)k_;
	return n->key < k->s.start ? -1 : n->key > k->s.end ? 1 : 0;
}

/* used to suppress gcc warning about "comparison is always false" */
static unsigned zero(void)
{
	return 0;
}

union walk_test1_data_15 {
	struct {
		unsigned size;
		unsigned filled;
		unsigned keys[15];
	} s;
	union walk_test1_data d;
};

union walk_test1_data_4 {
	struct {
		unsigned size;
		unsigned filled;
		unsigned keys[4];
	} s;
	union walk_test1_data d;
};

static int check_tree(const struct btree_node *const t)
{
	{
		size_t s, i = 0;
		const struct btree_node *stack[5], *n;
		unsigned keys[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		struct btree_node red_zone = {{NULL,NULL}};
		stack[4] = &red_zone;
		btree_walk_stack(t, stack, s, n)
			keys[i++] = tree_node_from_btree_node(n)->key;
		TEST(stack[4] == &red_zone);
		TEST(i == sizeof(keys)/sizeof(keys[0]));
		TEST(keys[0] == 8);
		TEST(keys[1] == 4);
		TEST(keys[2] == 2);
		TEST(keys[3] == 1);
		TEST(keys[4] == 3);
		TEST(keys[5] == 6);
		TEST(keys[6] == 5);
		TEST(keys[7] == 7);
		TEST(keys[8] == 12);
		TEST(keys[9] == 10);
		TEST(keys[10] == 9);
		TEST(keys[11] == 11);
		TEST(keys[12] == 14);
		TEST(keys[13] == 13);
		TEST(keys[14] == 15);
	}
	{
		size_t s, i = 0;
		const struct btree_node *stack[5], *n;
		unsigned keys[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		struct btree_node red_zone = {{NULL,NULL}};
		stack[4] = &red_zone;
		btree_walk_stack_forward(t, stack, s, n)
			keys[i++] = tree_node_from_btree_node(n)->key;
		TEST(stack[4] == &red_zone);
		TEST(i == sizeof(keys)/sizeof(keys[0]));
		for (i = 0; i < sizeof(keys)/sizeof(keys[0]); i++)
			TEST(keys[i] == i + 1);
	}
	{
		size_t s, i = 0;
		const struct btree_node *stack[5], *n;
		unsigned keys[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		struct btree_node red_zone = {{NULL,NULL}};
		stack[4] = &red_zone;
		btree_walk_stack_backward(t, stack, s, n)
			keys[i++] = tree_node_from_btree_node(n)->key;
		TEST(stack[4] == &red_zone);
		TEST(i == sizeof(keys)/sizeof(keys[0]));
		for (i = 0; i < sizeof(keys)/sizeof(keys[0]); i++)
			TEST(keys[i] == sizeof(keys)/sizeof(keys[0]) - i);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	(void)argc, (void)argv;
	TEST(btree_first(tree) == &n1.node);
	TEST(btree_last(tree) == &n15.node);
	TEST(btree_first(tree->btree_right) == &n9.node);
	TEST(btree_last(tree->btree_left) == &n7.node);
	TEST(btree_size(tree) == 15);
	TEST(btree_height(tree) == 4);
	TEST(btree_size(tree->btree_left) == 7);
	TEST(btree_height(tree->btree_right) == 3);
	{
		union walk_test1_data_15 data = {{15,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
		struct btree_node *const n = btree_walk_recursive_forward(tree, &data.d.obj, test1_walker);
		TEST(!n);
		TEST(data.s.filled == sizeof(data.s.keys)/sizeof(data.s.keys[0]));
		{
			unsigned i = 0;
			for (; i < data.s.filled; i++)
				TEST(data.s.keys[i] == i + 1);
		}
	}
	{
		union walk_test1_data_15 data = {{15,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
		struct btree_node *const n = btree_walk_recursive_backward(tree, &data.d.obj, test1_walker);
		TEST(!n);
		TEST(data.s.filled == sizeof(data.s.keys)/sizeof(data.s.keys[0]));
		{
			unsigned i = 0;
			for (; i < data.s.filled; i++)
				TEST(data.s.keys[i] == data.s.filled - i);
		}
	}
	{
		union walk_test1_data_15 data = {{15,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
		struct btree_node *const n = btree_walk_recursive(tree, &data.d.obj, test1_walker);
		TEST(!n);
		TEST(data.s.filled == sizeof(data.s.keys)/sizeof(data.s.keys[0]));
		TEST(data.s.keys[0] == 8);
		TEST(data.s.keys[1] == 4);
		TEST(data.s.keys[2] == 2);
		TEST(data.s.keys[3] == 1);
		TEST(data.s.keys[4] == 3);
		TEST(data.s.keys[5] == 6);
		TEST(data.s.keys[6] == 5);
		TEST(data.s.keys[7] == 7);
		TEST(data.s.keys[8] == 12);
		TEST(data.s.keys[9] == 10);
		TEST(data.s.keys[10] == 9);
		TEST(data.s.keys[11] == 11);
		TEST(data.s.keys[12] == 14);
		TEST(data.s.keys[13] == 13);
		TEST(data.s.keys[14] == 15);
	}
	{
		union search_test1_key k = {13};
		const struct btree_node *const n = btree_search(tree, &k.key, test1_comparator);
		TEST(n == &n13.node);
	}
	{
		const struct btree_node *n = tree;
		BTREE_SEARCH(n, BTREE_KEY_COMPARATOR(tree_node_from_btree_node(n)->key, 10));
		TEST(n == &n10.node);
	}
	{
		union search_test1_key k = {2};
		struct btree_node *parent = tree;
		const int c = btree_search_parent(&parent, &k.key, test1_comparator, /*leaf:*/0);
		TEST(!c);
		TEST(parent == &n2.node);
	}
	{
		struct btree_node *parent = tree;
		int c;
		BTREE_SEARCH_PARENT(c, parent, BTREE_KEY_COMPARATOR(tree_node_from_btree_node(parent)->key, 2), /*leaf:*/0);
		TEST(!c);
		TEST(parent == &n2.node);
	}
	{
		union search_test1_key k = {2};
		struct btree_node *parent = tree;
		const int c = btree_search_parent(&parent, &k.key, test1_comparator, /*leaf:*/1);
		TEST(c);
		TEST(parent == &n1.node || parent == &n2.node);
	}
	{
		struct btree_node *parent = tree;
		int c;
		BTREE_SEARCH_PARENT(c, parent, BTREE_KEY_COMPARATOR(tree_node_from_btree_node(parent)->key, 2), /*leaf:*/1);
		TEST(c);
		TEST(parent == &n1.node || parent == &n2.node);
	}
	{
		union search_test1_key k = {22};
		struct btree_node *parent = tree;
		const int c = btree_search_parent(&parent, &k.key, test1_comparator, /*leaf:*/0);
		TEST(c < 0);
		TEST(parent == &n15.node);
	}
	{
		struct btree_node *parent = tree;
		int c;
		BTREE_SEARCH_PARENT(c, parent, BTREE_KEY_COMPARATOR(tree_node_from_btree_node(parent)->key, 22), /*leaf:*/0);
		TEST(c < 0);
		TEST(parent == &n15.node);
	}
	{
		union search_test1_key k = {0};
		struct btree_node *parent = tree;
		const int c = btree_search_parent(&parent, &k.key, test1_comparator, /*leaf:*/0);
		TEST(c > 0);
		TEST(parent == &n1.node);
	}
	{
		struct btree_node *parent = tree;
		int c;
		BTREE_SEARCH_PARENT(c, parent, BTREE_KEY_COMPARATOR(tree_node_from_btree_node(parent)->key, zero()), /*leaf:*/0);
		TEST(c > 0);
		TEST(parent == &n1.node);
	}
	{
		/* find any node in range [2..5] */
		const struct btree_node *n = tree;
		BTREE_SEARCH(n, tree_node_from_btree_node(n)->key < 2 ? -1 : tree_node_from_btree_node(n)->key > 5 ? 1 : 0);
		TEST(n == &n4.node);
	}
	{
		/* find any node in range [2..5] */
		union range_search_test1_key k = {{2,5}};
		const struct btree_node *const n = btree_search(tree, &k.key, test1_range_comparator);
		TEST(n == &n4.node);
		{
			union walk_test1_data_4 data = {{4,0,{0,0,0,0}}};
			struct btree_node *const x = btree_walk_sub_recursive(n, &k.key, test1_range_comparator, &data.d.obj, test1_walker);
			TEST(!x);
			TEST(data.s.filled == sizeof(data.s.keys)/sizeof(data.s.keys[0]));
			{
				unsigned i = 2;
				for (; i <= 5; i++) {
					unsigned j = 0;
					for (; j < data.s.filled; j++)
						if (data.s.keys[j] == i)
							break;
					TEST(j != data.s.filled);
				}
			}
		}
		{
			union walk_test1_data_4 data = {{4,0,{0,0,0,0}}};
			struct btree_node *const x = btree_walk_sub_recursive_forward(&n4.node, &k.key, test1_range_comparator, &data.d.obj, test1_walker);
			TEST(!x);
			TEST(data.s.filled == sizeof(data.s.keys)/sizeof(data.s.keys[0]));
			{
				unsigned i = 2;
				for (; i <= 5; i++)
					TEST(data.s.keys[i - 2] == i);
			}
		}
		{
			union walk_test1_data_4 data = {{4,0,{0,0,0,0}}};
			struct btree_node *const x = btree_walk_sub_recursive_backward(&n4.node, &k.key, test1_range_comparator, &data.d.obj, test1_walker);
			TEST(!x);
			TEST(data.s.filled == sizeof(data.s.keys)/sizeof(data.s.keys[0]));
			{
				unsigned i = 2;
				for (; i <= 5; i++)
					TEST(data.s.keys[i - 2] == 5 - (i - 2));
			}
		}
	}
	TEST(0 == check_tree(tree));
	{
		union walk_test1_data_15 data = {{15,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
		unsigned const allocated_ = allocated;
		struct btree_node *dyn_tree = dynamic_tree();
		TEST(dyn_tree);
		TEST(0 == check_tree(dyn_tree));
		btree_delete_recursive(dyn_tree, &data.d.obj, test_deleter1);
		TEST(allocated_ == allocated);
		TEST(data.s.filled == sizeof(data.s.keys)/sizeof(data.s.keys[0]));
		TEST(data.s.keys[0] == 8);
		TEST(data.s.keys[1] == 4);
		TEST(data.s.keys[2] == 2);
		TEST(data.s.keys[3] == 1);
		TEST(data.s.keys[4] == 3);
		TEST(data.s.keys[5] == 6);
		TEST(data.s.keys[6] == 5);
		TEST(data.s.keys[7] == 7);
		TEST(data.s.keys[8] == 12);
		TEST(data.s.keys[9] == 10);
		TEST(data.s.keys[10] == 9);
		TEST(data.s.keys[11] == 11);
		TEST(data.s.keys[12] == 14);
		TEST(data.s.keys[13] == 13);
		TEST(data.s.keys[14] == 15);
	}
	{
		union walk_test1_data_15 data = {{15,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
		unsigned const allocated_ = allocated;
		struct btree_node *dyn_tree = dynamic_tree();
		TEST(dyn_tree);
		btree_delete_recursive_forward(dyn_tree, &data.d.obj, test_deleter1);
		TEST(allocated_ == allocated);
		TEST(data.s.filled == sizeof(data.s.keys)/sizeof(data.s.keys[0]));
		{
			unsigned i = 0;
			for (; i < data.s.filled; i++)
				TEST(data.s.keys[i] == i + 1);
		}
	}
	{
		union walk_test1_data_15 data = {{15,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
		unsigned const allocated_ = allocated;
		struct btree_node *dyn_tree = dynamic_tree();
		TEST(dyn_tree);
		btree_delete_recursive_backward(dyn_tree, &data.d.obj, test_deleter1);
		TEST(allocated_ == allocated);
		TEST(data.s.filled == sizeof(data.s.keys)/sizeof(data.s.keys[0]));
		{
			unsigned i = 0;
			for (; i < data.s.filled; i++)
				TEST(data.s.keys[i] == data.s.filled - i);
		}
	}
	{
		size_t s, i = 0;
		struct btree_node *stack[5], *n, *next;
		unsigned keys[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		struct btree_node red_zone = {{NULL,NULL}};
		unsigned const allocated_ = allocated;
		struct btree_node *dyn_tree = dynamic_tree();
		stack[4] = &red_zone;
		TEST(dyn_tree);
		btree_delete_stack(dyn_tree, stack, s, n, next) {
			struct tree_node *const x = tree_node_from_btree_node(n);
			keys[i++] = x->key;
			free_node(x);
		}
		TEST(allocated_ == allocated);
		TEST(stack[4] == &red_zone);
		TEST(i == sizeof(keys)/sizeof(keys[0]));
		TEST(keys[0] == 8);
		TEST(keys[1] == 4);
		TEST(keys[2] == 2);
		TEST(keys[3] == 1);
		TEST(keys[4] == 3);
		TEST(keys[5] == 6);
		TEST(keys[6] == 5);
		TEST(keys[7] == 7);
		TEST(keys[8] == 12);
		TEST(keys[9] == 10);
		TEST(keys[10] == 9);
		TEST(keys[11] == 11);
		TEST(keys[12] == 14);
		TEST(keys[13] == 13);
		TEST(keys[14] == 15);
	}
	{
		size_t s, i = 0;
		struct btree_node *stack[5], *n, *next;
		unsigned keys[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		struct btree_node red_zone = {{NULL,NULL}};
		unsigned const allocated_ = allocated;
		struct btree_node *dyn_tree = dynamic_tree();
		stack[4] = &red_zone;
		TEST(dyn_tree);
		btree_delete_stack_forward(dyn_tree, stack, s, n, next) {
			struct tree_node *const x = tree_node_from_btree_node(n);
			keys[i++] = x->key;
			free_node(x);
		}
		TEST(allocated_ == allocated);
		TEST(stack[4] == &red_zone);
		TEST(i == sizeof(keys)/sizeof(keys[0]));
		for (i = 0; i < sizeof(keys)/sizeof(keys[0]); i++)
			TEST(keys[i] == i + 1);
	}
	{
		size_t s, i = 0;
		struct btree_node *stack[5], *n, *next;
		unsigned keys[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		struct btree_node red_zone = {{NULL,NULL}};
		unsigned const allocated_ = allocated;
		struct btree_node *dyn_tree = dynamic_tree();
		stack[4] = &red_zone;
		TEST(dyn_tree);
		btree_delete_stack_backward(dyn_tree, stack, s, n, next) {
			struct tree_node *const x = tree_node_from_btree_node(n);
			keys[i++] = x->key;
			free_node(x);
		}
		TEST(allocated_ == allocated);
		TEST(stack[4] == &red_zone);
		TEST(i == sizeof(keys)/sizeof(keys[0]));
		for (i = 0; i < sizeof(keys)/sizeof(keys[0]); i++)
			TEST(keys[i] == sizeof(keys)/sizeof(keys[0]) - i);
	}
	TEST(!allocated);
	printf("all tests OK\n");
	return 0;
}
