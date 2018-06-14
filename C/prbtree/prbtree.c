/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2012-2018 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* prbtree.c */

#include "collections_config.h"
#include "prbtree.h"

/* node color is stored in the lowest bit of parent pointer */
#define PRB_RED_COLOR   1u
#define PRB_BLACK_COLOR 0u

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

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Use_decl_annotations
#endif
PRBTREE_EXPORTS void prbtree_rebalance(
	struct prbtree *tree/*!=NULL*/,
	struct prbtree_node *A_Restrict p/*!=NULL*/,
	struct prbtree_node *A_Restrict e/*!=NULL*/)
{
	/* insert red node e */
	PRBTREE_ASSERT(tree);
	PRBTREE_ASSERT(p);
	PRBTREE_ASSERT(e);
	PRBTREE_ASSERT_PTRS(p != e);
	e->parent_color = prbtree_make_parent_color_(p, PRB_RED_COLOR);
	while (PRB_BLACK_COLOR != prbtree_get_color_(p)) {
		/* there are 4 cases with red parent: (inserted node marked as * - push it up splitting the parent if necessary):
		1)
		          -----------                 -----------------
		          |      6,B|                 |      2,B      |
		          |2,R      |      rotate     |1,R         6,R|
		        ----------------     ->       --------------------     (final)
		        |1*| |4,B| |7,B|             [a][b]    |4,B| |7,B|
		        ---- ----- -----                       ----- -----
		       [a][b]
		2)
		          -----------                 -----------------
		          |      6,B|      double     |      4,B      |
		          |2,R      |      rotate     |2,R         6,R|
		       -----------------     ->    -----------------------     (final)
		       |1,B| |4*|  |7,B|           |1,B| [a]     [b] |7,B|
		       ----- ----  -----           -----             -----
		            [a][b]
		                                                ----
		                                                |6*|
		3)     -----------------            ------------------------
		       |      6,B      |            |      2,B|        |8,B|
		       |2,R         8,R|   recolor  |1,R      |        |   |
		     ----------------------  ->    --------------   -----------   (continue?)
		     |1*| |4,B| |7,B| |9,B|        [a][b]   |4,B|   |7,B| |9,B|
		     ---- ----- ----- -----                 -----   ----- -----
		    [a][b]
		                                                ----
		                                                |6*|
		4)     -----------------            ------------------------
		       |      6,B      |            |2,B      |        |8,B|
		       |2,R         8,R|   recolor  |      4,R|        |   |
		    -----------------------  ->  ---------------    -----------   (continue?)
		    |1,B| |4*|  |7,B| |9,B|      |1,B|    [a][b]    |7,B| |9,B|
		    ----- ----  ----- -----      -----              ----- -----
		         [a][b]
		*/
		struct prbtree_node *A_Restrict g = prbtree_get_parent(p); /* p is red */
		struct prbtree_node *A_Restrict t;
		PRBTREE_ASSERT(g);
		PRBTREE_ASSERT_PTRS(g != e);
		PRBTREE_ASSERT_PTRS(g != p);
		t = g->prbtree_left; /* may be NULL on first iteration */
		PRBTREE_ASSERT_PTRS(t != e);
		PRBTREE_ASSERT_PTRS(t != g);
		for (;;) {
			if (p != t) {
				if (t && PRB_BLACK_COLOR != prbtree_get_color_(t))
					break; /* case 3,4 */
				/* cases 1,2 */
				if (p->prbtree_left == e) {
					/* case 2 */
					t = e->prbtree_right;
					PRBTREE_ASSERT_PTRS(t != e);
					PRBTREE_ASSERT_PTRS(t != g);
					PRBTREE_ASSERT_PTRS(t != p);
					if (t) /* NULL on first iteration, != NULL on next iterations */
						t->parent_color = prbtree_make_parent_color_(p, PRB_BLACK_COLOR);
					p->prbtree_left = t;
					p->parent_color = prbtree_make_parent_color_(e, PRB_RED_COLOR);
					e->prbtree_right = p;
					p = e;
				}
				/* case 1 */
				t = p->prbtree_left;
				p->prbtree_left = g;
				g->prbtree_right = t;
			}
			else {
				t = g->prbtree_right; /* may be NULL on first iteration */
				PRBTREE_ASSERT_PTRS(t != e);
				PRBTREE_ASSERT_PTRS(t != g);
				PRBTREE_ASSERT_PTRS(t != p);
				if (t && PRB_BLACK_COLOR != prbtree_get_color_(t))
					break; /* case 3,4 */
				/* cases 1,2 */
				if (p->prbtree_left != e) {
					/* case 2 */
					t = e->prbtree_left;
					PRBTREE_ASSERT_PTRS(t != e);
					PRBTREE_ASSERT_PTRS(t != g);
					PRBTREE_ASSERT_PTRS(t != p);
					if (t) /* NULL on first iteration, != NULL on next iterations */
						t->parent_color = prbtree_make_parent_color_(p, PRB_BLACK_COLOR);
					p->prbtree_right = t;
					p->parent_color = prbtree_make_parent_color_(e, PRB_RED_COLOR);
					e->prbtree_left = p;
					p = e;
				}
				/* case 1 */
				t = p->prbtree_right;
				p->prbtree_right = g;
				g->prbtree_left = t;
			}
			PRBTREE_ASSERT_PTRS(t != g);
			PRBTREE_ASSERT_PTRS(t != p);
			if (t) /* NULL on first iteration, != NULL on next iterations */
				t->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR);
			t = prbtree_black_node_parent_(g); /* NULL? */
			PRBTREE_ASSERT_PTRS(t != g);
			PRBTREE_ASSERT_PTRS(t != p);
			*prbtree_slot_at_parent_(tree, t, g) = p;
			p->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
			g->parent_color = prbtree_make_parent_color_(p, PRB_RED_COLOR);
			return; /* (final) */
		}
		/* cases 3,4 */
		PRBTREE_ASSERT_PTRS(t != p);
		t->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR); /* recolor t: red -> black */
		p->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR); /* recolor p: red -> black */
		p = prbtree_black_node_parent_(g);
		if (!p)
			return;
		PRBTREE_ASSERT_PTRS(p != g);
		g->parent_color = prbtree_make_parent_color_(p, PRB_RED_COLOR); /* recolor g: black -> red */
		e = g;
	}
}

static inline void prbtree_remove_(
	struct prbtree *tree/*!=NULL*/,
	struct prbtree_node *p/*!=NULL*/,
	struct prbtree_node *e/*!=NULL*/)
{
	/* after removing leaf black node, tree is disbalanced - balance the tree by moving black node from sibling side:

	1)      -----                        --------
	        |3,B|                        |3,B   |
	    ------------                     |   7,R|
	    |1,B*| |7,B|       recolor   ------------    -> continue
	    ------ -----         ->      |1,B*|
	                                 ------
	                                                                  -----
	2)      -----                        -----                        |7,B|
	        |3,B|                        |3,B|                     -----------
	    ---------------              ---------------               |3,B| |8,B|
	    |1,B*| |   8,B|    rotate    |1,B*| |7,B   |   rotate  --------- -----
	    ------ |7,R   |      ->      ------ |   8,R|     ->    |1,B*|
	           --------                     --------           ------

	3)      -----                        ---------------                     -----
	        |3,B|                        |     7,B     |                     |7,B|
	    ------------------  rotate       |3,R       8,R|   recolor     --------------
	    |1,B*| |   7,B   |    ->     -------------------     ->        |3,B   | |8,B|
	    ------ |4,R   8,R|           |1,B*| |4,B|                      |   4,R| -----
	           -----------           ------ -----                  ------------
	                                                               |1,B*|
	                                                               ------
	4)      -----------                  -----------                     -----
	        |3,B      |                  |      7,B|                     |7,B|
	        |      7,R|     rotate       |3,R      |   recolor     ------------
	    ----------------      ->     ----------------    ->        |3,B   | [x]
	    |1,B*| |4,B| [x]             |1,B*| |4,B| [x]              |   4,R|
	    ------ -----                 ------ -----              ------------
	                                                           |1,B*|
	                                                           ------
	5)      --------------                  --------------                 --------------                       -----
	        |3,?      7,?|                  |3,B         |                 |         7,B|                       |7,B|
	        |            |    rotate        |         7,R|    rotate       |3,R         |    rotate    ---------------
	    -------------------     ->      -------------------     ->     -------------------     ->      |   4,B   | [x]
	    |1,B*| |   5,B| [x]             |1,B*| |4,B   | [x]            |1,B*| |4,B   | [x]             |3,R   5,R|
	    ------ |4,R   |                 ------ |   5,R|                ------ |   5,R|             ---------------
	           --------                        --------                       --------             |1,B*|
	                                                                                               ------

	6)      -----------------                 -----------------                           -----                   -----------
	        |3,B            |                 |            7,B|                           |7,B|                   |      7,B|
	        |            7,R|   rotate        |3,R            |   rotate      ------------------   recolor        |5,R      |
	    ----------------------    ->      ----------------------    ->        |      5,B   | [x]     ->     ------------------
	    |1,B*| |   5,B   | [x]            |1,B*| |   5,B   | [x]              |3,R      6,R|                |3,B   | |6,B| [x]
	    ------ |4,R   6,R|                ------ |4,R   6,R|              ------------------                |   4,R| -----
	           -----------                       -----------              |1,B*| |4,B|                   -----------
	                                                                      ------ -----                   |1,B*|
	                                                                                                     ------
	7)   ------------                ------------                -----
	     |       7,B|                |3,B       |                |3,B|
	     |3,R       |      rotate    |       7,R|      recolor  -----------------
	    ----------------     ->     ----------------     ->     [x]      |7,B   |
	    [x] |4,B*| |8,B|            [x] |4,B*| |8,B|                     |   8,R|
	        ------ -----                ------ -----                 ------------
	                                                                 |4,B*|
	                                                                 ------

	8)  ------------                   ------------                   ------------                   -----
	    |3,?    7,?|                   |       7,B|                   |3,B       |                   |3,B|
	    |          |         rotate    |3,R       |         rotate    |       7,R|         rotate   -------------------
	   -------------------     ->     -------------------     ->     -------------------     ->     [x]     |   8,B   |
	   [x] |4,B*| |   9,B|            [x] |4,B*| |8,B   |            [x] |4,B*| |8,B   |                    |7,R   9,R|
	       ------ |8,R   |                ------ |   9,R|                ------ |   9,R|                ---------------
	              --------                       --------                       --------                |4,B*|
	                                                                                                    ------

	9)                                                                  ------------
	    ------------                    ------------                    |0,B       |                     -------------                 ----------------
	    |       3,B|                    |0,B       |                    |       3,R|                     |0,B        |                 |0,B           |
	    |0,R       |           rotate   |       3,R|           rotate  ----------------         rotate   |        6,R|        rotate   |           7,R|
	   ----------------------   ->     ----------------------   ->     [x] |1,B*| |6,B|           ->    --------------------    ->    --------------------
	   [x] |1,B*| |   7,B   |          [x] |1,B*| |   7,B   |              ------ -----------           [x]   |3,B| |7,B   |          [x]   |   6,B| |8,B|
	       ------ |6,R   8,R|              ------ |6,R   8,R|                        |7,B   |              -------- |   8,R|                |3,R   | -----
	              -----------                     -----------                        |   8,R|              |1,B*|   --------              ----------
	                                                                                 --------              ------                         |1,B*|
	                                                                                                                                      ------
	10)  (see case 4)      ---------------                      ---------
	                       |      7,B    |                      |7,B    |
	                       |3,R       8,R|    recolor           |    8,R|
	                   --------------------     ->        ----------------
	                   |1,B*| |4,B|  [a][b]               |3,B   |  [a][b]
	                   ------ -----                       |   4,R|
	                                                  ------------
	                                                  |1,B*|
	                                                  ------

	11)  (see case 5)      ------------------                ------------------                        ---------
	                       |         7,B    |                |         7,B    |                        |7,B    |
	                       |3,R          8,R|    rotate      |3,R          8,R|    rotate              |    8,R|
	                   -----------------------     ->    -----------------------     ->       -------------------
	                   |1,B*| |   5,B|  [a][b]           |1,B*| |4,B   |  [a][b]              |   4,B   |  [a][b]
	                   ------ |4,R   |                   ------ |   5,R|                      |3,R   5,R|
	                          --------                          --------                  ---------------
	                                                                                      |1,B*|
	                                                                                      ------
	12)  (see case 6)      ---------------------                           ---------                     ---------------
	                       |            7,B    |                           |7,B    |                     |      7,B    |
	                       |3,R             8,R|    rotate                 |    8,R|   recolor           |5,R       8,R|
	                   --------------------------     ->       ----------------------    ->        ----------------------
	                   |1,B*| |   5,B   |  [a][b]              |      5,B   |  [a][b]              |   4,B| |6,B|  [a][b]
	                   ------ |4,R   6,R|                      |3,R      6,R|                      |3,R   | -----
	                          -----------                  ------------------                  ------------
	                                                       |1,B*| |4,B|                        |1,B*|
	                                                       ------ -----                        ------

	13)  (see case 7)      ----------------                        ---------
	                       |       7,B    |                        |7,B    |
	                       |3,R        8,R|    recolor             |    8,R|
	                    --------------------     ->       ------------------
	                    |1,B| |4,B*|  [a][b]              |   3,B|     [a][b]
	                    ----- ------                      |1,R   |
	                                                      ------------
	                                                            |4,B*|
	                                                            ------

	14)  (see case 8)      ----------------                        ---------
	                       |       7,B    |                        |7,B    |
	                       |3,R        8,R|    rotate              |    8,R|
	                 -----------------------     ->    ---------------------
	                 |   1,B| |4,B*|  [a][b]           |   1,B   |     [a][b]
	                 |0,R   | ------                   |0,R   3,R|
	                 --------                          ---------------
	                                                            |4,B*|
	                                                            ------

	                                                               ----------------                                                     ---------------------
	15)  (see case 9)         ----------------                     |       7,B    |                  -------------------                |            7,B    |
	                          |       7,B    |                     |3,R        8,R|                  |          7,B    |                |1,B             8,R|
	                          |3,R        8,R|    rotate        --------------------   rotate        |2,R           8,R|    rotate   -------------------------
	                 --------------------------     ->          |2,B| |4,B*|  [a][b]     ->     -------------------------     ->     |0,B| |2,B   |     [a][b]
	                 |   1,B   | |4,B*|  [a][b]           ----------- ------                    |   1,B| |3,B|     [a][b]            ----- |   3,R|
	                 |0,R   2,R| ------                   |   1,B|                              |0,R   | ---------                         ------------
	                 -----------                          |0,R   |                              --------    |4,B*|                               |4,B*|
	                                                      --------                                          ------                               ------
	*/
	for (;;) {
		struct prbtree_node *a;
		struct prbtree_node *t = p->prbtree_left;
		void *p_;
		if (PRB_BLACK_COLOR == prbtree_get_color_(p)) {
			if (e == t) {
				t = p->prbtree_right;
				a = t->prbtree_left;
				/* parent is black: cases 1,2,3,4,5,6 */
				if (PRB_BLACK_COLOR == prbtree_get_color_(t)) {
					/* t is black: cases 1,2,3 */
					struct prbtree_node *b = t->prbtree_right;
					if (a && PRB_BLACK_COLOR != prbtree_get_color_(a)) { /* red on first iteration */
						if (b && PRB_BLACK_COLOR != prbtree_get_color_(b)) { /* red on first iteration */
							/* case 3 */
							b->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							a->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
						}
						else {
							/* case 2 */
							if (a->prbtree_right) /* NULL on first iteration */
								a->prbtree_right->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							t->prbtree_left = a->prbtree_right;
							a->prbtree_right = t;
							t->parent_color = prbtree_make_parent_color_(a, PRB_BLACK_COLOR);
							t = a;
						}
					}
					else if (b && PRB_BLACK_COLOR != prbtree_get_color_(b)) { /* red on first iteration */
						/* case 2 */
						b->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR); /* recolor: red -> black */
						a = t;
					}
					else {
						/* case 1 */
						t->parent_color = prbtree_make_parent_color_(p, PRB_RED_COLOR); /* recolor brother: black -> red */
						e = p;
						p = prbtree_black_node_parent_(p);
						if (!p)
							return;
						continue;
					}
				}
				else {
					/* cases 4,5,6 with black parent */
#ifdef _MSC_VER
					__assume(a);
#endif
					{
						struct prbtree_node *c = a->prbtree_left;
						if (c && PRB_BLACK_COLOR != prbtree_get_color_(c)) { /* red on first iteration */
							/* cases 5,6 */
							struct prbtree_node *d = a->prbtree_right;
							if (d && PRB_BLACK_COLOR != prbtree_get_color_(d)) { /* red on first iteration */
								/* case 6 */
								a->parent_color = prbtree_make_parent_color_(t, PRB_RED_COLOR);
								d->parent_color = prbtree_make_parent_color_(a, PRB_BLACK_COLOR);
								c->parent_color = prbtree_make_parent_color_(a, PRB_BLACK_COLOR);
							}
							else {
								/* case 5 */
								if (c->prbtree_right) /* NULL on first iteration */
									c->prbtree_right->parent_color = prbtree_make_parent_color_(a, PRB_BLACK_COLOR);
								a->prbtree_left = c->prbtree_right;
								c->prbtree_right = a;
								t->prbtree_left = c;
								a->parent_color = prbtree_make_parent_color_(c, PRB_RED_COLOR);
								c->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							}
							a = c;
						}
					}
				}
				if (a->prbtree_left) /* NULL on first iteration */
					a->prbtree_left->parent_color = prbtree_make_parent_color_(p, PRB_BLACK_COLOR);
				p->prbtree_right = a->prbtree_left;
				a->prbtree_left = p; /* cases 4,5 */
			}
			else {
				a = t->prbtree_right;
				/* parent is black: cases 1,2,3,4,5,6 */
				if (PRB_BLACK_COLOR == prbtree_get_color_(t)) {
					/* t is black: cases 1,2,3 */
					struct prbtree_node *b = t->prbtree_left;
					if (a && PRB_BLACK_COLOR != prbtree_get_color_(a)) { /* red on first iteration */
						if (b && PRB_BLACK_COLOR != prbtree_get_color_(b)) { /* red on first iteration */
							/* case 3 */
							b->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							a->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
						}
						else {
							/* case 2 */
							if (a->prbtree_left) /* NULL on first iteration */
								a->prbtree_left->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							t->prbtree_right = a->prbtree_left;
							a->prbtree_left = t;
							t->parent_color = prbtree_make_parent_color_(a, PRB_BLACK_COLOR);
							t = a;
						}
					}
					else if (b && PRB_BLACK_COLOR != prbtree_get_color_(b)) { /* red on first iteration */
						/* case 2 */
						b->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR); /* recolor: red -> black */
						a = t;
					}
					else {
						/* case 1 */
						t->parent_color = prbtree_make_parent_color_(p, PRB_RED_COLOR); /* recolor brother: black -> red */
						e = p;
						p = prbtree_black_node_parent_(p);
						if (!p)
							return;
						continue;
					}
				}
				else {
					/* cases 4,5,6 with black parent */
#ifdef _MSC_VER
					__assume(a);
#endif
					{
						struct prbtree_node *c = a->prbtree_right;
						if (c && PRB_BLACK_COLOR != prbtree_get_color_(c)) { /* red on first iteration */
							/* cases 5,6 */
							struct prbtree_node *d = a->prbtree_left;
							if (d && PRB_BLACK_COLOR != prbtree_get_color_(d)) { /* red on first iteration */
								/* case 6 */
								a->parent_color = prbtree_make_parent_color_(t, PRB_RED_COLOR);
								d->parent_color = prbtree_make_parent_color_(a, PRB_BLACK_COLOR);
								c->parent_color = prbtree_make_parent_color_(a, PRB_BLACK_COLOR);
							}
							else {
								/* case 5 */
								if (c->prbtree_left) /* NULL on first iteration */
									c->prbtree_left->parent_color = prbtree_make_parent_color_(a, PRB_BLACK_COLOR);
								a->prbtree_right = c->prbtree_left;
								c->prbtree_left = a;
								t->prbtree_right = c;
								a->parent_color = prbtree_make_parent_color_(c, PRB_RED_COLOR);
								c->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							}
							a = c;
						}
					}
				}
				if (a->prbtree_right) /* NULL on first iteration */
					a->prbtree_right->parent_color = prbtree_make_parent_color_(p, PRB_BLACK_COLOR);
				p->prbtree_left = a->prbtree_right;
				a->prbtree_right = p; /* cases 4,5 */
			}
			{
				struct prbtree_node *g = prbtree_black_node_parent_(p);
				t->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR);
				*prbtree_slot_at_parent_(tree, g, p) = t;
			}
			p_ = prbtree_make_parent_color_(a, t != a ? PRB_RED_COLOR : PRB_BLACK_COLOR);
		}
		else {
			struct prbtree_node *g = prbtree_get_parent(p);
			if (e == t) {
				t = p->prbtree_right;
				a = t->prbtree_left;
				{
					/* parent is red: cases 4,5,6,7,8,9 (10:4,11:5,12:6,13:7,14:8,15:9) */
					struct prbtree_node *b = t->prbtree_right;
					if (a && PRB_BLACK_COLOR != prbtree_get_color_(a)) {
						/* cases 5,6,8,9 (11:5,12:6,14:8,15:9) */
						if (b && PRB_BLACK_COLOR != prbtree_get_color_(b)) {
							/* cases 6,9 (12:6,15:9) */
							t->parent_color = prbtree_make_parent_color_(g, PRB_RED_COLOR);
							b->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							a->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
						}
						else {
							/* cases 5,8 (11:5,14:8) */
							if (a->prbtree_right) /* NULL on first iteration */
								a->prbtree_right->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							t->prbtree_left = a->prbtree_right;
							a->prbtree_right = t;
							t->parent_color = prbtree_make_parent_color_(a, PRB_RED_COLOR);
							t = a;
							a->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR);
						}
					}
					else if (b && PRB_BLACK_COLOR != prbtree_get_color_(b)) {
						/* cases 5,8 (11:5,14:8) */
						a = t;
						a->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR);
					}
					else {
						/* cases 4,7 (10:4,13:7) */
						t->parent_color = prbtree_make_parent_color_(p, PRB_RED_COLOR);   /* recolor: black -> red */
						p->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR); /* recolor: red -> black */
						return;
					}
				}
				if (a->prbtree_left) /* NULL on first iteration */
					a->prbtree_left->parent_color = prbtree_make_parent_color_(p, PRB_BLACK_COLOR);
				p->prbtree_right = a->prbtree_left;
				a->prbtree_left = p;
			}
			else {
				a = t->prbtree_right;
				{
					/* parent is red: cases 4,5,6,7,8,9 (10:4,11:5,12:6,13:7,14:8,15:9) */
					struct prbtree_node *b = t->prbtree_left;
					if (a && PRB_BLACK_COLOR != prbtree_get_color_(a)) {
						/* cases 5,6,8,9 (11:5,12:6,14:8,15:9) */
						if (b && PRB_BLACK_COLOR != prbtree_get_color_(b)) {
							/* cases 6,9 (12:6,15:9) */
							t->parent_color = prbtree_make_parent_color_(g, PRB_RED_COLOR);
							b->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							a->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
						}
						else {
							/* cases 5,8 (11:5,14:8) */
							if (a->prbtree_left) /* NULL on first iteration */
								a->prbtree_left->parent_color = prbtree_make_parent_color_(t, PRB_BLACK_COLOR);
							t->prbtree_right = a->prbtree_left;
							a->prbtree_left = t;
							t->parent_color = prbtree_make_parent_color_(a, PRB_RED_COLOR);
							t = a;
							a->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR);
						}
					}
					else if (b && PRB_BLACK_COLOR != prbtree_get_color_(b)) {
						/* cases 5,8 (11:5,14:8) */
						a = t;
						a->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR);
					}
					else {
						/* cases 4,7 (10:4,13:7) */
						t->parent_color = prbtree_make_parent_color_(p, PRB_RED_COLOR);   /* recolor: black -> red */
						p->parent_color = prbtree_make_parent_color_(g, PRB_BLACK_COLOR); /* recolor: red -> black */
						return;
					}
				}
				if (a->prbtree_right) /* NULL on first iteration */
					a->prbtree_right->parent_color = prbtree_make_parent_color_(p, PRB_BLACK_COLOR);
				p->prbtree_left = a->prbtree_right;
				a->prbtree_right = p;
			}
			if (p == g->prbtree_left)
				g->prbtree_left = t; /* case 5,6 (11:5,12:6) */
			else
				g->prbtree_right = t; /* case 8,9 (14:8,15:9) */
			p_ = prbtree_make_parent_color_(a, PRB_RED_COLOR);
		}
		p->parent_color = p_;
		return;
	}
}

PRBTREE_EXPORTS void prbtree_remove(
	struct prbtree *tree/*!=NULL*/,
	struct prbtree_node *e/*!=NULL*/)
{
	struct prbtree_node *t = e->prbtree_right;
	if (t) {
		if (t->prbtree_left) {
			do {
				t = t->prbtree_left;
			} while (t->prbtree_left);
			if (t->prbtree_right) {
				/* t is black (because tree is black-node balanced):
				         -----                    -----
				         |4,B|      rotate        |4,B|
				  ---------------     ->       -----------
				  |2,B*   | |5,B|              |3,B| |5,B|
				  |    3,R| -----          --------- -----
				  ---------                |2,B*|
				                           ------           */
				struct prbtree_node *p = prbtree_black_node_parent_(t);
				struct prbtree_node *r = t->prbtree_right;
				r->parent_color = prbtree_make_parent_color_(p, PRB_BLACK_COLOR); /* change parent & recolor node: red -> black */
				p->prbtree_left = r;
				goto replace_;
			}
		}
		else if (t->prbtree_right) {
			/* t is black (because tree is black-node balanced):
			    -----                      -----
			    |1,B|          rotate      |1,B|
			 ---------------     ->     ---------------
			 |0,B| |2,B*   |            |0,B|     |3,B|
			 ----- |    3,R|            ----- ---------
			       ---------                  |2,B*|
			                                  ------        */
			struct prbtree_node *r = t->prbtree_right;
			r->parent_color = prbtree_make_parent_color_(e, PRB_BLACK_COLOR); /* change parent & recolor node: red -> black */
			e->prbtree_right = r;
			goto replace_;
		}
	}
	else {
		t = e->prbtree_left;
		if (t) {
			/* t is red (because tree is black-node balanced):
			 ---------
			 |    3,B|
			 |2,R*   |
			 --------- */
			e->prbtree_left = (struct prbtree_node*)0;
			goto replace_;
		}
		t = e;
	}
	{
		/* remove t (leaf) from parent */
		struct prbtree_node *p = prbtree_get_parent(t);
		if (!p) {
			tree->root = (struct prbtree_node*)0;
			return;
		}
		if (PRB_BLACK_COLOR == prbtree_get_color_(t))
			prbtree_remove_(tree, p, t); /* t - leaf black node */
		p->u.leaves[t != p->prbtree_left] = (struct prbtree_node*)0;
	}
	if (t != e) {
replace_:
		prbtree_replace(tree, e, t); /* replace e with t */
	}
}
