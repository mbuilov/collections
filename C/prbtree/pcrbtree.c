/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2018 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* pcrbtree.c */

#include "collections_config.h"

#ifndef ASSUME
#if defined _MSC_VER
#define ASSUME(cond) __assume(!!(cond))
#elif defined __clang_analyzer__
#define ASSUME(cond) ((void)(!(cond) ? __builtin_unreachable(), 0 : 1))
#elif defined __clang__
#define ASSUME(cond) __builtin_assume(!!(cond))
#elif defined __INTEL_COMPILER
#define ASSUME(cond) ((void)0) /* ICC compiles calls to __builtin_unreachable() as jumps somewhere... */
#elif defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define ASSUME(cond) ((void)(!(cond) ? __builtin_unreachable(), 0 : 1))
#else
#define ASSUME(cond) ((void)0) /* assume condition is always true */
#endif
#endif /* ASSUME */

#ifndef ASSERT
#define ASSERT(e) ASSUME(e)
#endif

#include "pcrbtree.h"

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

#define pcrbtree_make_parent_color_right(p, c) pcrbtree_make_parent_color_1(p, c | PCRB_RIGHT_CHILD)
#define pcrbtree_make_parent_color_left(p, c)  pcrbtree_make_parent_color_1(p, c | PCRB_LEFT_CHILD)

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Use_decl_annotations
#endif
PCRBTREE_EXPORTS void pcrbtree_rebalance(
	struct pcrbtree *A_Restrict tree/*!=NULL*/,
	struct pcrbtree_node *A_Restrict p/*!=NULL*/,
	struct pcrbtree_node *A_Restrict e/*!=NULL*/,
	int c)
{
	/* insert red node e */
	unsigned is_right = (unsigned)(c < 0);
	PCRBTREE_ASSERT(tree);
	PCRBTREE_ASSERT(p);
	PCRBTREE_ASSERT(e);
	PCRBTREE_ASSERT_PTRS(p != e);
	p->u.leaves[is_right] = e;
	e->parent_color = pcrbtree_make_parent_color_1(p, (is_right ? PCRB_RIGHT_CHILD : PCRB_LEFT_CHILD) | PCRB_RED_COLOR);
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
		struct pcrbtree_node *A_Restrict g = pcrbtree_get_parent(p); /* p is red */
		void *pc;
		PCRBTREE_ASSERT(g);
		PCRBTREE_ASSERT_PTRS(g != e);
		PCRBTREE_ASSERT_PTRS(g != p);
		for (;;) {
			struct pcrbtree_node *A_Restrict t;
			if (pcrbtree_is_right_(p)) {
				t = g->pcrbtree_left; /* may be NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(t != e);
				PCRBTREE_ASSERT_PTRS(t != g);
				PCRBTREE_ASSERT_PTRS(t != p);
				if (t && PCRB_BLACK_COLOR != pcrbtree_get_color_(t)) {
					t->parent_color = pcrbtree_make_parent_color_left(g, PCRB_BLACK_COLOR); /* recolor t: red -> black */
					p->parent_color = pcrbtree_make_parent_color_right(g, PCRB_BLACK_COLOR); /* recolor p: red -> black */
					break; /* case 3,4 */
				}
				/* cases 1,2 */
				if (!pcrbtree_is_right_(e)) {
					/* case 2 */
					t = e->pcrbtree_right;
					PCRBTREE_ASSERT_PTRS(t != e);
					PCRBTREE_ASSERT_PTRS(t != g);
					PCRBTREE_ASSERT_PTRS(t != p);
					if (t) /* NULL on first iteration, != NULL on next iterations */
						t->parent_color = pcrbtree_make_parent_color_left(p, PCRB_BLACK_COLOR);
					p->pcrbtree_left = t;
					p->parent_color = pcrbtree_make_parent_color_right(e, PCRB_RED_COLOR);
					e->pcrbtree_right = p;
					p = e;
				}
				/* case 1 */
				t = p->pcrbtree_left;
				PCRBTREE_ASSERT_PTRS(t != e);
				PCRBTREE_ASSERT_PTRS(t != g);
				PCRBTREE_ASSERT_PTRS(t != p);
				p->pcrbtree_left = g;
				g->pcrbtree_right = t;
				if (t) /* NULL on first iteration, != NULL on next iterations */
					t->parent_color = pcrbtree_make_parent_color_right(g, PCRB_BLACK_COLOR);
				pc = g->parent_color;
				g->parent_color = pcrbtree_make_parent_color_left(p, PCRB_RED_COLOR);
			}
			else {
				t = g->pcrbtree_right; /* may be NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(t != e);
				PCRBTREE_ASSERT_PTRS(t != g);
				PCRBTREE_ASSERT_PTRS(t != p);
				if (t && PCRB_BLACK_COLOR != pcrbtree_get_color_(t)) {
					t->parent_color = pcrbtree_make_parent_color_right(g, PCRB_BLACK_COLOR); /* recolor t: red -> black */
					p->parent_color = pcrbtree_make_parent_color_left(g, PCRB_BLACK_COLOR); /* recolor p: red -> black */
					break; /* case 3,4 */
				}
				/* cases 1,2 */
				if (pcrbtree_is_right_(e)) {
					/* case 2 */
					t = e->pcrbtree_left;
					PCRBTREE_ASSERT_PTRS(t != e);
					PCRBTREE_ASSERT_PTRS(t != g);
					PCRBTREE_ASSERT_PTRS(t != p);
					if (t) /* NULL on first iteration, != NULL on next iterations */
						t->parent_color = pcrbtree_make_parent_color_right(p, PCRB_BLACK_COLOR);
					p->pcrbtree_right = t;
					p->parent_color = pcrbtree_make_parent_color_left(e, PCRB_RED_COLOR);
					e->pcrbtree_left = p;
					p = e;
				}
				/* case 1 */
				t = p->pcrbtree_right;
				PCRBTREE_ASSERT_PTRS(t != e);
				PCRBTREE_ASSERT_PTRS(t != g);
				PCRBTREE_ASSERT_PTRS(t != p);
				p->pcrbtree_right = g;
				g->pcrbtree_left = t;
				if (t) /* NULL on first iteration, != NULL on next iterations */
					t->parent_color = pcrbtree_make_parent_color_left(g, PCRB_BLACK_COLOR);
				pc = g->parent_color;
				g->parent_color = pcrbtree_make_parent_color_right(p, PCRB_RED_COLOR);
			}
			is_right = pcrbtree_is_right_1(pc);
			t = pcrbtree_get_parent_(pc); /* NULL? */
			PCRBTREE_ASSERT_PTRS(t != g);
			PCRBTREE_ASSERT_PTRS(t != p);
			*pcrbtree_slot_at_parent_(tree, t, is_right) = p;
			p->parent_color = pcrbtree_make_parent_color_1(t, (is_right ? PCRB_RIGHT_CHILD : PCRB_LEFT_CHILD) | PCRB_BLACK_COLOR);
			return; /* (final) */
		}
		/* cases 3,4 */
		pc = g->parent_color;
		if (!pc)
			return;
		g->parent_color = pcrbtree_recolor_to_red(pc); /* recolor g: black -> red */
		p = pcrbtree_get_parent_(pc);
		PCRBTREE_ASSERT_PTRS(p != g);
		e = g;
	}
}

static inline void pcrbtree_replace_child(
	struct pcrbtree *A_Restrict tree/*!=NULL*/,
	void *pc,
	struct pcrbtree_node *A_Restrict e/*!=NULL*/)
{
	struct pcrbtree_node *A_Restrict p = pcrbtree_get_parent_(pc); /* may be NULL */
	PCRBTREE_ASSERT(tree);
	PCRBTREE_ASSERT(e);
	PCRBTREE_ASSERT_PTRS(e != p);
	e->parent_color = pc;
	*pcrbtree_slot_at_parent_(tree, p, pcrbtree_is_right_1(pc)) = e;
}

static inline void pcrbtree_remove_(
	struct pcrbtree *A_Restrict tree/*!=NULL*/,
	struct pcrbtree_node *A_Restrict p/*!=NULL*/,
	struct pcrbtree_node *A_Restrict e/*!=NULL*/)
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
	void *ec;
	PCRBTREE_ASSERT(tree);
	PCRBTREE_ASSERT(p);
	PCRBTREE_ASSERT(e);
	PCRBTREE_ASSERT_PTRS(p != e);
	ec = e->parent_color;
	for (;;) {
		void *pc = p->parent_color;
		if (PCRB_BLACK_COLOR == pcrbtree_get_color_1(pc)) {
			if (!pcrbtree_is_right_1(ec)) {
				struct pcrbtree_node *A_Restrict t = p->pcrbtree_right;
				PCRBTREE_ASSERT(t);
				PCRBTREE_ASSERT_PTRS(t != p);
				PCRBTREE_ASSERT_PTRS(t != e);
				e = t->pcrbtree_left; /* may be NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(e != p);
				PCRBTREE_ASSERT_PTRS(e != t);
				/* parent is black: cases 1,2,3,4,5,6 */
				if (PCRB_BLACK_COLOR == pcrbtree_get_color_(t)) {
					/* t is black: cases 1,2,3 */
					struct pcrbtree_node *A_Restrict b = t->pcrbtree_right; /* may be NULL on first iteration */
					PCRBTREE_ASSERT_PTRS(b != p);
					PCRBTREE_ASSERT_PTRS(b != t);
					if (e && PCRB_BLACK_COLOR != pcrbtree_get_color_(e)) { /* red on first iteration */
						PCRBTREE_ASSERT_PTRS(b != e);
						if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) { /* red on first iteration */
							/* case 3 */
							b->parent_color = pcrbtree_make_parent_color_right(t, PCRB_BLACK_COLOR);
							e->parent_color = pcrbtree_make_parent_color_left(t, PCRB_BLACK_COLOR);
							p->parent_color = pcrbtree_make_parent_color_left(e, PCRB_RED_COLOR);
						}
						else {
							/* case 2 */
							b = e->pcrbtree_right; /* NULL on first iteration */
							PCRBTREE_ASSERT_PTRS(b != p);
							PCRBTREE_ASSERT_PTRS(b != t);
							PCRBTREE_ASSERT_PTRS(b != e);
							if (b)
								b->parent_color = pcrbtree_make_parent_color_left(t, PCRB_BLACK_COLOR);
							t->pcrbtree_left = b;
							e->pcrbtree_right = t;
							t->parent_color = pcrbtree_make_parent_color_right(e, PCRB_BLACK_COLOR);
							t = e;
							p->parent_color = pcrbtree_make_parent_color_left(e, PCRB_BLACK_COLOR);
						}
					}
					else if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) { /* red on first iteration */
						/* case 2 */
						PCRBTREE_ASSERT_PTRS(b != e);
						b->parent_color = pcrbtree_make_parent_color_right(t, PCRB_BLACK_COLOR); /* recolor: red -> black */
						e = t;
						p->parent_color = pcrbtree_make_parent_color_left(e, PCRB_BLACK_COLOR);
					}
					else {
						/* case 1 */
						e = p;
						p = pcrbtree_get_parent_(pc); /* may be NULL */
						PCRBTREE_ASSERT_PTRS(p != e);
						PCRBTREE_ASSERT_PTRS(p != t);
						t->parent_color = pcrbtree_make_parent_color_right(e, PCRB_RED_COLOR); /* recolor brother: black -> red */
						if (!p)
							return;
						ec = pc;
						continue;
					}
				}
				else {
					/* cases 4,5,6 with black parent */
					PCRBTREE_ASSERT(e);
#ifdef _MSC_VER
					__assume(e);
#endif
					{
						struct pcrbtree_node *A_Restrict c = e->pcrbtree_left; /* may be NULL on first iteration */
						PCRBTREE_ASSERT_PTRS(c != p);
						PCRBTREE_ASSERT_PTRS(c != t);
						PCRBTREE_ASSERT_PTRS(c != e);
						if (c && PCRB_BLACK_COLOR != pcrbtree_get_color_(c)) { /* red on first iteration */
							/* cases 5,6 */
							struct pcrbtree_node *A_Restrict d = e->pcrbtree_right; /* may be NULL on first iteration */
							PCRBTREE_ASSERT_PTRS(d != p);
							PCRBTREE_ASSERT_PTRS(d != t);
							PCRBTREE_ASSERT_PTRS(d != e);
							PCRBTREE_ASSERT_PTRS(d != c);
							if (d && PCRB_BLACK_COLOR != pcrbtree_get_color_(d)) { /* red on first iteration */
								/* case 6 */
								e->parent_color = pcrbtree_make_parent_color_left(t, PCRB_RED_COLOR);
								d->parent_color = pcrbtree_make_parent_color_right(e, PCRB_BLACK_COLOR);
								c->parent_color = pcrbtree_make_parent_color_left(e, PCRB_BLACK_COLOR);
							}
							else {
								/* case 5 */
								d = c->pcrbtree_right; /* NULL on first iteration */
								PCRBTREE_ASSERT_PTRS(d != p);
								PCRBTREE_ASSERT_PTRS(d != t);
								PCRBTREE_ASSERT_PTRS(d != e);
								PCRBTREE_ASSERT_PTRS(d != c);
								if (d)
									d->parent_color = pcrbtree_make_parent_color_left(e, PCRB_BLACK_COLOR);
								e->pcrbtree_left = d;
								c->pcrbtree_right = e;
								t->pcrbtree_left = c;
								e->parent_color = pcrbtree_make_parent_color_right(c, PCRB_RED_COLOR);
								c->parent_color = pcrbtree_make_parent_color_left(t, PCRB_BLACK_COLOR);
							}
							e = c;
						}
					}
					p->parent_color = pcrbtree_make_parent_color_left(e, PCRB_RED_COLOR);
				}
				pcrbtree_replace_child(tree, pc, t);
				t = e->pcrbtree_left; /* NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(t != p);
				PCRBTREE_ASSERT_PTRS(t != e);
				if (t)
					t->parent_color = pcrbtree_make_parent_color_right(p, PCRB_BLACK_COLOR);
				p->pcrbtree_right = t;
				e->pcrbtree_left = p; /* cases 4,5 */
			}
			else {
				struct pcrbtree_node *A_Restrict t = p->pcrbtree_left;
				PCRBTREE_ASSERT(t);
				PCRBTREE_ASSERT_PTRS(t != p);
				PCRBTREE_ASSERT_PTRS(t != e);
				e = t->pcrbtree_right; /* may be NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(e != p);
				PCRBTREE_ASSERT_PTRS(e != t);
				/* parent is black: cases 1,2,3,4,5,6 */
				if (PCRB_BLACK_COLOR == pcrbtree_get_color_(t)) {
					/* t is black: cases 1,2,3 */
					struct pcrbtree_node *A_Restrict b = t->pcrbtree_left; /* may be NULL on first iteration */
					PCRBTREE_ASSERT_PTRS(b != p);
					PCRBTREE_ASSERT_PTRS(b != t);
					if (e && PCRB_BLACK_COLOR != pcrbtree_get_color_(e)) { /* red on first iteration */
						PCRBTREE_ASSERT_PTRS(b != e);
						if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) { /* red on first iteration */
							/* case 3 */
							b->parent_color = pcrbtree_make_parent_color_left(t, PCRB_BLACK_COLOR);
							e->parent_color = pcrbtree_make_parent_color_right(t, PCRB_BLACK_COLOR);
							p->parent_color = pcrbtree_make_parent_color_right(e, PCRB_RED_COLOR);
						}
						else {
							/* case 2 */
							b = e->pcrbtree_left; /* NULL on first iteration */
							PCRBTREE_ASSERT_PTRS(b != p);
							PCRBTREE_ASSERT_PTRS(b != t);
							PCRBTREE_ASSERT_PTRS(b != e);
							if (b)
								b->parent_color = pcrbtree_make_parent_color_right(t, PCRB_BLACK_COLOR);
							t->pcrbtree_right = b;
							e->pcrbtree_left = t;
							t->parent_color = pcrbtree_make_parent_color_left(e, PCRB_BLACK_COLOR);
							t = e;
							p->parent_color = pcrbtree_make_parent_color_right(e, PCRB_BLACK_COLOR);
						}
					}
					else if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) { /* red on first iteration */
						/* case 2 */
						PCRBTREE_ASSERT_PTRS(b != e);
						b->parent_color = pcrbtree_make_parent_color_left(t, PCRB_BLACK_COLOR); /* recolor: red -> black */
						e = t;
						p->parent_color = pcrbtree_make_parent_color_right(e, PCRB_BLACK_COLOR);
					}
					else {
						/* case 1 */
						e = p;
						p = pcrbtree_get_parent_(pc); /* may be NULL */
						PCRBTREE_ASSERT_PTRS(p != e);
						PCRBTREE_ASSERT_PTRS(p != t);
						t->parent_color = pcrbtree_make_parent_color_left(e, PCRB_RED_COLOR); /* recolor brother: black -> red */
						if (!p)
							return;
						ec = pc;
						continue;
					}
				}
				else {
					/* cases 4,5,6 with black parent */
					PCRBTREE_ASSERT(e);
#ifdef _MSC_VER
					__assume(e);
#endif
					{
						struct pcrbtree_node *A_Restrict c = e->pcrbtree_right; /* may be NULL on first iteration */
						PCRBTREE_ASSERT_PTRS(c != p);
						PCRBTREE_ASSERT_PTRS(c != t);
						PCRBTREE_ASSERT_PTRS(c != e);
						if (c && PCRB_BLACK_COLOR != pcrbtree_get_color_(c)) { /* red on first iteration */
							/* cases 5,6 */
							struct pcrbtree_node *A_Restrict d = e->pcrbtree_left; /* may be NULL on first iteration */
							PCRBTREE_ASSERT_PTRS(d != p);
							PCRBTREE_ASSERT_PTRS(d != t);
							PCRBTREE_ASSERT_PTRS(d != e);
							PCRBTREE_ASSERT_PTRS(d != c);
							if (d && PCRB_BLACK_COLOR != pcrbtree_get_color_(d)) { /* red on first iteration */
								/* case 6 */
								e->parent_color = pcrbtree_make_parent_color_right(t, PCRB_RED_COLOR);
								d->parent_color = pcrbtree_make_parent_color_left(e, PCRB_BLACK_COLOR);
								c->parent_color = pcrbtree_make_parent_color_right(e, PCRB_BLACK_COLOR);
							}
							else {
								/* case 5 */
								d = c->pcrbtree_left; /* NULL on first iteration */
								PCRBTREE_ASSERT_PTRS(d != p);
								PCRBTREE_ASSERT_PTRS(d != t);
								PCRBTREE_ASSERT_PTRS(d != e);
								PCRBTREE_ASSERT_PTRS(d != c);
								if (d)
									d->parent_color = pcrbtree_make_parent_color_right(e, PCRB_BLACK_COLOR);
								e->pcrbtree_right = d;
								c->pcrbtree_left = e;
								t->pcrbtree_right = c;
								e->parent_color = pcrbtree_make_parent_color_left(c, PCRB_RED_COLOR);
								c->parent_color = pcrbtree_make_parent_color_right(t, PCRB_BLACK_COLOR);
							}
							e = c;
						}
					}
					p->parent_color = pcrbtree_make_parent_color_right(e, PCRB_RED_COLOR);
				}
				pcrbtree_replace_child(tree, pc, t);
				t = e->pcrbtree_right; /* NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(t != p);
				PCRBTREE_ASSERT_PTRS(t != e);
				if (t)
					t->parent_color = pcrbtree_make_parent_color_left(p, PCRB_BLACK_COLOR);
				p->pcrbtree_left = t;
				e->pcrbtree_right = p; /* cases 4,5 */
			}
		}
		else {
			struct pcrbtree_node *A_Restrict g = pcrbtree_get_parent_(pc);
			PCRBTREE_ASSERT(g); /* p is red */
			PCRBTREE_ASSERT_PTRS(g != p);
			PCRBTREE_ASSERT_PTRS(g != e);
			if (!pcrbtree_is_right_1(ec)) {
				struct pcrbtree_node *A_Restrict t = p->pcrbtree_right;
				PCRBTREE_ASSERT(t);
				PCRBTREE_ASSERT_PTRS(t != p);
				PCRBTREE_ASSERT_PTRS(t != e);
				PCRBTREE_ASSERT_PTRS(t != g);
				e = t->pcrbtree_left; /* may be NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(e != p);
				PCRBTREE_ASSERT_PTRS(e != g);
				PCRBTREE_ASSERT_PTRS(e != t);
				{
					/* parent is red: cases 4,5,6,7,8,9 (10:4,11:5,12:6,13:7,14:8,15:9) */
					struct pcrbtree_node *A_Restrict b = t->pcrbtree_right; /* may be NULL on first iteration */
					PCRBTREE_ASSERT_PTRS(b != p);
					PCRBTREE_ASSERT_PTRS(b != g);
					PCRBTREE_ASSERT_PTRS(b != t);
					if (e && PCRB_BLACK_COLOR != pcrbtree_get_color_(e)) {
						/* cases 5,6,8,9 (11:5,12:6,14:8,15:9) */
						PCRBTREE_ASSERT_PTRS(b != e);
						if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) {
							/* cases 6,9 (12:6,15:9) */
							t->parent_color = pc;
							b->parent_color = pcrbtree_make_parent_color_right(t, PCRB_BLACK_COLOR);
							e->parent_color = pcrbtree_make_parent_color_left(t, PCRB_BLACK_COLOR);
						}
						else {
							/* cases 5,8 (11:5,14:8) */
							b = e->pcrbtree_right; /* NULL on first iteration */
							PCRBTREE_ASSERT_PTRS(b != p);
							PCRBTREE_ASSERT_PTRS(b != g);
							PCRBTREE_ASSERT_PTRS(b != t);
							PCRBTREE_ASSERT_PTRS(b != e);
							if (b)
								b->parent_color = pcrbtree_make_parent_color_left(t, PCRB_BLACK_COLOR);
							t->pcrbtree_left = b;
							e->pcrbtree_right = t;
							t->parent_color = pcrbtree_make_parent_color_right(e, PCRB_RED_COLOR);
							t = e;
							e->parent_color = pcrbtree_recolor_to_black(pc);
						}
					}
					else if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) {
						/* cases 5,8 (11:5,14:8) */
						PCRBTREE_ASSERT_PTRS(b != e);
						e = t;
						e->parent_color = pcrbtree_recolor_to_black(pc);
					}
					else {
						/* cases 4,7 (10:4,13:7) */
						t->parent_color = pcrbtree_make_parent_color_right(p, PCRB_RED_COLOR); /* recolor: black -> red */
						p->parent_color = pcrbtree_recolor_to_black(pc); /* recolor: red -> black */
						return;
					}
				}
				g->u.leaves[pcrbtree_is_right_1(pc)] = t;
				t = e->pcrbtree_left; /* NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(t != p);
				PCRBTREE_ASSERT_PTRS(t != g);
				PCRBTREE_ASSERT_PTRS(t != e);
				if (t)
					t->parent_color = pcrbtree_make_parent_color_right(p, PCRB_BLACK_COLOR);
				p->pcrbtree_right = t;
				p->parent_color = pcrbtree_make_parent_color_left(e, PCRB_RED_COLOR);
				e->pcrbtree_left = p;
			}
			else {
				struct pcrbtree_node *A_Restrict t = p->pcrbtree_left;
				PCRBTREE_ASSERT(t);
				PCRBTREE_ASSERT_PTRS(t != p);
				PCRBTREE_ASSERT_PTRS(t != e);
				PCRBTREE_ASSERT_PTRS(t != g);
				e = t->pcrbtree_right; /* may be NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(e != p);
				PCRBTREE_ASSERT_PTRS(e != g);
				PCRBTREE_ASSERT_PTRS(e != t);
				{
					/* parent is red: cases 4,5,6,7,8,9 (10:4,11:5,12:6,13:7,14:8,15:9) */
					struct pcrbtree_node *A_Restrict b = t->pcrbtree_left; /* may be NULL on first iteration */
					PCRBTREE_ASSERT_PTRS(b != p);
					PCRBTREE_ASSERT_PTRS(b != g);
					PCRBTREE_ASSERT_PTRS(b != t);
					if (e && PCRB_BLACK_COLOR != pcrbtree_get_color_(e)) {
						/* cases 5,6,8,9 (11:5,12:6,14:8,15:9) */
						PCRBTREE_ASSERT_PTRS(b != e);
						if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) {
							/* cases 6,9 (12:6,15:9) */
							t->parent_color = pc;
							b->parent_color = pcrbtree_make_parent_color_left(t, PCRB_BLACK_COLOR);
							e->parent_color = pcrbtree_make_parent_color_right(t, PCRB_BLACK_COLOR);
						}
						else {
							/* cases 5,8 (11:5,14:8) */
							b = e->pcrbtree_left; /* NULL on first iteration */
							PCRBTREE_ASSERT_PTRS(b != p);
							PCRBTREE_ASSERT_PTRS(b != g);
							PCRBTREE_ASSERT_PTRS(b != t);
							PCRBTREE_ASSERT_PTRS(b != e);
							if (b)
								b->parent_color = pcrbtree_make_parent_color_right(t, PCRB_BLACK_COLOR);
							t->pcrbtree_right = b;
							e->pcrbtree_left = t;
							t->parent_color = pcrbtree_make_parent_color_left(e, PCRB_RED_COLOR);
							t = e;
							e->parent_color = pcrbtree_recolor_to_black(pc);
						}
					}
					else if (b && PCRB_BLACK_COLOR != pcrbtree_get_color_(b)) {
						/* cases 5,8 (11:5,14:8) */
						PCRBTREE_ASSERT_PTRS(b != e);
						e = t;
						e->parent_color = pcrbtree_recolor_to_black(pc);
					}
					else {
						/* cases 4,7 (10:4,13:7) */
						t->parent_color = pcrbtree_make_parent_color_left(p, PCRB_RED_COLOR); /* recolor: black -> red */
						p->parent_color = pcrbtree_recolor_to_black(pc); /* recolor: red -> black */
						return;
					}
				}
				g->u.leaves[pcrbtree_is_right_1(pc)] = t;
				t = e->pcrbtree_right; /* NULL on first iteration */
				PCRBTREE_ASSERT_PTRS(t != p);
				PCRBTREE_ASSERT_PTRS(t != g);
				PCRBTREE_ASSERT_PTRS(t != e);
				if (t)
					t->parent_color = pcrbtree_make_parent_color_left(p, PCRB_BLACK_COLOR);
				p->pcrbtree_left = t;
				p->parent_color = pcrbtree_make_parent_color_right(e, PCRB_RED_COLOR);
				e->pcrbtree_right = p;
			}
		}
		return;
	}
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Use_decl_annotations
#endif
PCRBTREE_EXPORTS void pcrbtree_remove(
	struct pcrbtree *A_Restrict tree/*!=NULL*/,
	struct pcrbtree_node *A_Restrict e/*!=NULL*/)
{
	struct pcrbtree_node *A_Restrict t = e->pcrbtree_right;
	PCRBTREE_ASSERT_PTRS(t != e);
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
				struct pcrbtree_node *A_Restrict p = pcrbtree_left_black_node_parent_(t);
				struct pcrbtree_node *A_Restrict r = t->pcrbtree_right;
				PCRBTREE_ASSERT(p);
				PCRBTREE_ASSERT(r);
				PCRBTREE_ASSERT_PTRS(p != e);
				PCRBTREE_ASSERT_PTRS(p != t);
				PCRBTREE_ASSERT_PTRS(r != e);
				PCRBTREE_ASSERT_PTRS(r != t);
				PCRBTREE_ASSERT_PTRS(r != p);
				r->parent_color = pcrbtree_make_parent_color_left(p, PCRB_BLACK_COLOR); /* change parent & recolor node: red -> black */
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
			struct pcrbtree_node *A_Restrict r = t->pcrbtree_right;
			PCRBTREE_ASSERT(r);
			PCRBTREE_ASSERT_PTRS(r != e);
			PCRBTREE_ASSERT_PTRS(r != t);
			r->parent_color = pcrbtree_make_parent_color_right(e, PCRB_BLACK_COLOR); /* change parent & recolor node: red -> black */
			e->pcrbtree_right = r;
			goto replace_;
		}
	}
	else {
		t = e->pcrbtree_left;
		PCRBTREE_ASSERT_PTRS(t != e);
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
		struct pcrbtree_node *A_Restrict p = pcrbtree_get_parent(t);
		PCRBTREE_ASSERT_PTRS(p != t);
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
		PCRBTREE_ASSERT_PTRS(t != e);
		pcrbtree_replace(tree, e, t); /* replace e with t */
	}
}
