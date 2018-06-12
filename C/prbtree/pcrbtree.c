/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2018 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* pcrbtree.c */

#include "collections_config.h"
#include "pcrbtree.h"

/* node left/right child flag is stored in the lowest bit of parent pointer */
#define PCRB_RIGHT_CHILD 1u
#define PCRB_LEFT_CHILD  0u

/* node color is stored in the second lower bit of parent pointer */
#define PCRB_RED_COLOR   2u
#define PCRB_BLACK_COLOR 0u

static inline void *pcrbtree_recolor_to_red(void *parent_color)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#endif
	return (void*)(2llu | (unsigned long long)parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

static inline void *pcrbtree_recolor_to_black(void *parent_color)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4826) /* Conversion from 'const char *' to 'unsigned __int64' is sign-extended */
#endif
	return (void*)(~2llu & (unsigned long long)parent_color);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Use_decl_annotations
#endif
PRBTREE_EXPORTS void pcrbtree_rebalance(
	struct pcrbtree *tree/*!=NULL*/,
	struct pcrbtree_node *p/*!=NULL*/,
	struct pcrbtree_node *e/*!=NULL*/,
	int c)
{
	/* insert red node e */
	unsigned right = (unsigned)(c < 0);
	p->u.leaves[right] = e;
	e->parent_color = pcrbtree_make_parent_color_(p, right | PCRB_RED_COLOR);
	while (PCRB_BLACK_COLOR != pcrbtree_get_color_(p)) {
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
		struct pcrbtree_node *g = pcrbtree_get_parent(p); /* p is red */
		struct pcrbtree_node *t;
		void *parent_color;
		for (;;) {
			if (pcrbtree_is_right_(p)) {
				t = g->pcrbtree_left;
				if (t && PCRB_BLACK_COLOR != pcrbtree_get_color_(t)) {
					t->parent_color = pcrbtree_make_parent_color_(g, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR); /* recolor t: red -> black */
					p->parent_color = pcrbtree_make_parent_color_(g, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR); /* recolor p: red -> black */
					break; /* case 3,4 */
				}
				/* cases 1,2 */
				if (!pcrbtree_is_right_(e)) {
					/* case 2 */
					t = e->pcrbtree_right;
					if (t) /* NULL on first iteration, != NULL on next iterations */
						t->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
					p->pcrbtree_left = t;
					p->parent_color = pcrbtree_make_parent_color_(e, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
					e->pcrbtree_right = p;
					p = e;
				}
				/* case 1 */
				t = p->pcrbtree_left;
				p->pcrbtree_left = g;
				g->pcrbtree_right = t;
				if (t) /* NULL on first iteration, != NULL on next iterations */
					t->parent_color = pcrbtree_make_parent_color_(g, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
				parent_color = g->parent_color;
				g->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
			}
			else {
				t = g->pcrbtree_right;
				if (t && PCRB_BLACK_COLOR != pcrbtree_get_color_(t)) {
					t->parent_color = pcrbtree_make_parent_color_(g, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR); /* recolor t: red -> black */
					p->parent_color = pcrbtree_make_parent_color_(g, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR); /* recolor p: red -> black */
					break; /* case 3,4 */
				}
				/* cases 1,2 */
				if (pcrbtree_is_right_(e)) {
					/* case 2 */
					t = e->pcrbtree_left;
					if (t) /* NULL on first iteration, != NULL on next iterations */
						t->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
					p->pcrbtree_right = t;
					p->parent_color = pcrbtree_make_parent_color_(e, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
					e->pcrbtree_left = p;
					p = e;
				}
				/* case 1 */
				t = p->pcrbtree_right;
				p->pcrbtree_right = g;
				g->pcrbtree_left = t;
				if (t) /* NULL on first iteration, != NULL on next iterations */
					t->parent_color = pcrbtree_make_parent_color_(g, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				parent_color = g->parent_color;
				g->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
			}
			right = pcrbtree_is_right_1(parent_color);
			t = pcrbtree_get_parent_(parent_color); /* NULL? */
			*pcrbtree_slot_at_parent_(tree, t, right) = p;
			p->parent_color = pcrbtree_make_parent_color_(t, right | PCRB_BLACK_COLOR);
			return; /* (final) */
		}
		/* cases 3,4 */
		parent_color = g->parent_color;
		if (!parent_color)
			return;
		g->parent_color = pcrbtree_recolor_to_red(parent_color); /* recolor g: black -> red */
		p = pcrbtree_get_parent_(parent_color);
		e = g;
	}
}

static inline void *pcrbtree_replace_child(
	struct pcrbtree *tree/*!=NULL*/,
	void *parent_color,
	struct pcrbtree_node *e/*!=NULL*/)
{
	e->parent_color = parent_color;
	*pcrbtree_slot_at_parent_(tree, pcrbtree_get_parent_(parent_color), pcrbtree_is_right_1(parent_color)) = e;
}

static inline void pcrbtree_remove_(
	struct pcrbtree *tree/*!=NULL*/,
	struct pcrbtree_node *p/*!=NULL*/,
	struct pcrbtree_node *e/*!=NULL*/)
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
	void *ec = e->parent_color;
	for (;;) {
		struct pcrbtree_node *a, *t;
//		struct pcrbtree_node *t = p->pcrbtree_left;
		void *p_;
		void *pc = p->parent_color;
		if (PCRB_BLACK_COLOR == pcrbtree_get_color_1(pc)) {
			if (!pcrbtree_is_right_1(ec)) {
				t = p->pcrbtree_right;
				a = t->pcrbtree_left;
				/* parent is black: cases 1,2,3,4,5,6 */
				if (PCRB_BLACK_COLOR == pcrbtree_get_color_(t)) {
					/* t is black: cases 1,2,3 */
					struct pcrbtree_node *b = t->pcrbtree_right;
					if (a && PCRB_BLACK_COLOR != pcrbtree_get_color_(a)) { /* red on first iteration */
						if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) { /* red on first iteration */
							/* case 3 */
							b->parent_color = pcrbtree_make_parent_color_(t, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
							a->parent_color = pcrbtree_make_parent_color_(t, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
							pcrbtree_replace_child(tree, pc, t);
							p->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
				if (a->pcrbtree_left) /* NULL on first iteration */
					a->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_right = a->pcrbtree_left;
				a->pcrbtree_left = p; /* cases 4,5 */
				return;
						}
						else {
							/* case 2 */
							if (a->pcrbtree_right) /* NULL on first iteration */
								a->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(t, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
							t->pcrbtree_left = a->pcrbtree_right;
							a->pcrbtree_right = t;
							t->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
							pcrbtree_replace_child(tree, pc, a);
							p->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				if (a->pcrbtree_left) /* NULL on first iteration */
					a->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_right = a->pcrbtree_left;
				a->pcrbtree_left = p; /* cases 4,5 */
				return;
						}
					}
					else if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) { /* red on first iteration */
						/* case 2 */
						b->parent_color = pcrbtree_make_parent_color_(t, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR); /* recolor: red -> black */
						a = t;
						pcrbtree_replace_child(tree, pc, a);
						p->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				if (a->pcrbtree_left) /* NULL on first iteration */
					a->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_right = a->pcrbtree_left;
				a->pcrbtree_left = p; /* cases 4,5 */
				return;
					}
					else {
						/* case 1 */
						/* recolor brother: black -> red */
						t->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
						e = p;
						p = pcrbtree_get_parent_(p);
						if (!p)
							return;
						ec = pc;
						continue;
					}
				}
				else {
					/* cases 4,5,6 with black parent */
#ifdef _MSC_VER
					__assume(a);
#endif
					{
						struct pcrbtree_node *c = a->pcrbtree_left;
						if (c && PCRB_BLACK_COLOR != pcrbtree_get_color_(c)) { /* red on first iteration */
							/* cases 5,6 */
							struct pcrbtree_node *d = a->pcrbtree_right;
							if (d && PCRB_BLACK_COLOR != pcrbtree_get_color_(d)) { /* red on first iteration */
								/* case 6 */
								a->parent_color = pcrbtree_make_parent_color_(t, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
								d->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
								c->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
							}
							else {
								/* case 5 */
								if (c->pcrbtree_right) /* NULL on first iteration */
									c->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
								a->pcrbtree_left = c->pcrbtree_right;
								c->pcrbtree_right = a;
								t->pcrbtree_left = c;
								a->parent_color = pcrbtree_make_parent_color_(c, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
								c->parent_color = pcrbtree_make_parent_color_(t, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
							}
							a = c;
						}
					}
					pcrbtree_replace_child(tree, pc, t);
					p->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
					if (a->pcrbtree_left) /* NULL on first iteration */
						a->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
					p->pcrbtree_right = a->pcrbtree_left;
					a->pcrbtree_left = p; /* cases 4,5 */
					return;
				}
			}
			else {
				t = p->pcrbtree_left;
				a = t->pcrbtree_right;
				/* parent is black: cases 1,2,3,4,5,6 */
				if (PCRB_BLACK_COLOR == pcrbtree_get_color_(t)) {
					/* t is black: cases 1,2,3 */
					struct pcrbtree_node *b = t->pcrbtree_left;
					if (a && PCRB_BLACK_COLOR != pcrbtree_get_color_(a)) { /* red on first iteration */
						if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) { /* red on first iteration */
							/* case 3 */
							b->parent_color = pcrbtree_make_parent_color_(t, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
							a->parent_color = pcrbtree_make_parent_color_(t, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
							pcrbtree_replace_child(tree, pc, t);
							p->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
				if (a->pcrbtree_right) /* NULL on first iteration */
					a->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_left = a->pcrbtree_right;
				a->pcrbtree_right = p; /* cases 4,5 */
				return;
						}
						else {
							/* case 2 */
							if (a->pcrbtree_left) /* NULL on first iteration */
								a->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(t, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
							t->pcrbtree_right = a->pcrbtree_left;
							a->pcrbtree_left = t;
							t->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
							pcrbtree_replace_child(tree, pc, a);
							p->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
				if (a->pcrbtree_right) /* NULL on first iteration */
					a->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_left = a->pcrbtree_right;
				a->pcrbtree_right = p; /* cases 4,5 */
				return;
						}
					}
					else if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) { /* red on first iteration */
						/* case 2 */
						b->parent_color = pcrbtree_make_parent_color_(t, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR); /* recolor: red -> black */
						a = t;
						pcrbtree_replace_child(tree, pc, a);
						p->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
				if (a->pcrbtree_right) /* NULL on first iteration */
					a->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_left = a->pcrbtree_right;
				a->pcrbtree_right = p; /* cases 4,5 */
				return;
					}
					else {
						/* case 1 */
						/* recolor brother: black -> red */
						t->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
						e = p;
						p = pcrbtree_get_parent_(p);
						if (!p)
							return;
						ec = pc;
						continue;
					}
				}
				else {
					/* cases 4,5,6 with black parent */
#ifdef _MSC_VER
					__assume(a);
#endif
					{
						struct pcrbtree_node *c = a->pcrbtree_right;
						if (c && PCRB_BLACK_COLOR != pcrbtree_get_color_(c)) { /* red on first iteration */
							/* cases 5,6 */
							struct pcrbtree_node *d = a->pcrbtree_left;
							if (d && PCRB_BLACK_COLOR != pcrbtree_get_color_(d)) { /* red on first iteration */
								/* case 6 */
								a->parent_color = pcrbtree_make_parent_color_(t, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
								d->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
								c->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
							}
							else {
								/* case 5 */
								if (c->pcrbtree_left) /* NULL on first iteration */
									c->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
								a->pcrbtree_right = c->pcrbtree_left;
								c->pcrbtree_left = a;
								t->pcrbtree_right = c;
								a->parent_color = pcrbtree_make_parent_color_(c, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
								c->parent_color = pcrbtree_make_parent_color_(t, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
							}
							a = c;
						}
					}
					pcrbtree_replace_child(tree, pc, t);
					p->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
					if (a->pcrbtree_right) /* NULL on first iteration */
						a->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
					p->pcrbtree_left = a->pcrbtree_right;
					a->pcrbtree_right = p; /* cases 4,5 */
					return;
				}
			}
		}
		else {
			struct pcrbtree_node *g = pcrbtree_get_parent_(pc);
			if (!pcrbtree_is_right_1(ec)) {
				t = p->pcrbtree_right;
				a = t->pcrbtree_left;
				{
					/* parent is red: cases 4,5,6,7,8,9 (10:4,11:5,12:6,13:7,14:8,15:9) */
					struct pcrbtree_node *b = t->pcrbtree_right;
					if (a && PCRB_BLACK_COLOR != pcrbtree_get_color_(a)) {
						/* cases 5,6,8,9 (11:5,12:6,14:8,15:9) */
						if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) {
							/* cases 6,9 (12:6,15:9) */
							t->parent_color = pc;
							b->parent_color = pcrbtree_make_parent_color_(t, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
							a->parent_color = pcrbtree_make_parent_color_(t, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
							g->u.leaves[pcrbtree_is_right_1(pc)] = t;
							p->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
				if (a->pcrbtree_left) /* NULL on first iteration */
					a->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_right = a->pcrbtree_left;
				a->pcrbtree_left = p;
				return;
						}
						else {
							/* cases 5,8 (11:5,14:8) */
							if (a->pcrbtree_right) /* NULL on first iteration */
								a->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(t, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
							t->pcrbtree_left = a->pcrbtree_right;
							a->pcrbtree_right = t;
							t->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
							a->parent_color = pcrbtree_recolor_to_black(pc);
							g->u.leaves[pcrbtree_is_right_1(pc)] = a;
							p->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
				if (a->pcrbtree_left) /* NULL on first iteration */
					a->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_right = a->pcrbtree_left;
				a->pcrbtree_left = p;
				return;
						}
					}
					else if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) {
						/* cases 5,8 (11:5,14:8) */
						a = t;
						a->parent_color = pcrbtree_recolor_to_black(pc);
						g->u.leaves[pcrbtree_is_right_1(pc)] = a;
						p->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
				if (a->pcrbtree_left) /* NULL on first iteration */
					a->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_right = a->pcrbtree_left;
				a->pcrbtree_left = p;
				return;
					}
					else {
						/* cases 4,7 (10:4,13:7) */
						t->parent_color = pcrbtree_make_parent_color_(p, PCRB_RIGHT_CHILD | PCRB_RED_COLOR); /* recolor: black -> red */
						p->parent_color = pcrbtree_recolor_to_black(pc); /* recolor: red -> black */
						return;
					}
				}
			}
			else {
				t = p->pcrbtree_left;
				a = t->pcrbtree_right;
				{
					/* parent is red: cases 4,5,6,7,8,9 (10:4,11:5,12:6,13:7,14:8,15:9) */
					struct pcrbtree_node *b = t->pcrbtree_left;
					if (a && PCRB_BLACK_COLOR != pcrbtree_get_color_(a)) {
						/* cases 5,6,8,9 (11:5,12:6,14:8,15:9) */
						if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) {
							/* cases 6,9 (12:6,15:9) */
							t->parent_color = pcrbtree_make_parent_color_(g, PCRB_RED_COLOR);
							b->parent_color = pcrbtree_make_parent_color_(t, PCRB_BLACK_COLOR);
							a->parent_color = pcrbtree_make_parent_color_(t, PCRB_BLACK_COLOR);

							t->parent_color = pc;
							b->parent_color = pcrbtree_make_parent_color_(t, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
							a->parent_color = pcrbtree_make_parent_color_(t, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
							g->u.leaves[pcrbtree_is_right_1(pc)] = t;
							p->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
				if (a->pcrbtree_right) /* NULL on first iteration */
					a->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_left = a->pcrbtree_right;
				a->pcrbtree_right = p;
				return;
						}
						else {
							/* cases 5,8 (11:5,14:8) */
							if (a->pcrbtree_left) /* NULL on first iteration */
								a->pcrbtree_left->parent_color = pcrbtree_make_parent_color_(t, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
							t->pcrbtree_right = a->pcrbtree_left;
							a->pcrbtree_left = t;
							t->parent_color = pcrbtree_make_parent_color_(a, PCRB_LEFT_CHILD | PCRB_RED_COLOR);
							a->parent_color = pcrbtree_recolor_to_black(pc);
							g->u.leaves[pcrbtree_is_right_1(pc)] = a;
							p->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
				if (a->pcrbtree_right) /* NULL on first iteration */
					a->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_left = a->pcrbtree_right;
				a->pcrbtree_right = p;
				return;
						}
					}
					else if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) {
						/* cases 5,8 (11:5,14:8) */
						a = t;
						a->parent_color = pcrbtree_recolor_to_black(pc);
						g->u.leaves[pcrbtree_is_right_1(pc)] = a;
						p->parent_color = pcrbtree_make_parent_color_(a, PCRB_RIGHT_CHILD | PCRB_RED_COLOR);
				if (a->pcrbtree_right) /* NULL on first iteration */
					a->pcrbtree_right->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_left = a->pcrbtree_right;
				a->pcrbtree_right = p;
				return;
					}
					else {
						/* cases 4,7 (10:4,13:7) */
						t->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_RED_COLOR); /* recolor: black -> red */
						p->parent_color = pcrbtree_recolor_to_black(pc); /* recolor: red -> black */
						return;
					}
				}
			}
		}
		return;
	}
}

PRBTREE_EXPORTS void pcrbtree_remove(
	struct pcrbtree *tree/*!=NULL*/,
	struct pcrbtree_node *e/*!=NULL*/)
{
	struct pcrbtree_node *t = e->pcrbtree_right;
	if (t) {
		if (t->pcrbtree_left) {
			do {
				t = t->pcrbtree_left;
			} while (t->pcrbtree_left);
			if (t->pcrbtree_right) {
				/* t is black (because tree is black-node balanced):
						 -----                    -----
						 |4,B|      rotate        |4,B|
				  ---------------     ->       -----------
				  |2,B*   | |5,B|              |3,B| |5,B|
				  |    3,R| -----          --------- -----
				  ---------                |2,B*|
										   ------           */
				struct pcrbtree_node *p = pcrbtree_left_black_node_parent_(t);
				struct pcrbtree_node *r = t->pcrbtree_right;
				/* change parent & recolor node: red -> black */
				r->parent_color = pcrbtree_make_parent_color_(p, PCRB_LEFT_CHILD | PCRB_BLACK_COLOR);
				p->pcrbtree_left = r;
				goto replace_;
			}
		}
		else if (t->pcrbtree_right) {
			/* t is black (because tree is black-node balanced):
			    -----                      -----
			    |1,B|          rotate      |1,B|
			 ---------------     ->     ---------------
			 |0,B| |2,B*   |            |0,B|     |3,B|
			 ----- |    3,R|            ----- ---------
			       ---------                  |2,B*|
		                                      ------        */
			struct pcrbtree_node *r = t->pcrbtree_right;
			/* change parent & recolor node: red -> black */
			r->parent_color = pcrbtree_make_parent_color_(e, PCRB_RIGHT_CHILD | PCRB_BLACK_COLOR);
			e->pcrbtree_right = r;
			goto replace_;
		}
	}
	else {
		t = e->pcrbtree_left;
		if (t) {
			/* t is red (because tree is black-node balanced):
			 ---------
			 |    3,B|
			 |2,R*   |
			 --------- */
			e->pcrbtree_left = (struct pcrbtree_node*)0;
			goto replace_;
		}
		t = e;
		e = (struct pcrbtree_node*)0;
	}
	{
		/* remove t (leaf) from parent */
		struct pcrbtree_node *p = pcrbtree_get_parent(t);
		if (!p) {
			tree->root = (struct pcrbtree_node*)0;
			return;
		}
		p->u.leaves[pcrbtree_is_right_(t)] = (struct pcrbtree_node*)0;
		if (PCRB_BLACK_COLOR == pcrbtree_get_color_(t))
			pcrbtree_remove_(tree, p, t); /* t - leaf black node */
	}
	if (e) {
replace_:
		pcrbtree_replace(tree, e, t); /* replace e with t */
	}
}
