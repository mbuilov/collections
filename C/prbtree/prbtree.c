/**********************************************************************************
* Embedded red-black binary tree of nodes with parent pointers
* Copyright (C) 2012-2017 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* prbtree.c */

#include "collections_config.h"
#include "prbtree.h"

PRBTREE_EXPORTS void __prbtree_insert(
	A_In struct _prbtree *tree/*!=NULL*/,
	A_In struct _prbtree_node *p/*!=NULL*/,
	A_In struct _prbtree_node *e/*!=NULL*/)
{
	/* insert red node e */
	PRB_SET_PARENT_COLOR(e, p, PRB_RED_COLOR);
	while (PRB_BLACK_COLOR != PRB_GET_COLOR(p)) {
		/* there are 4 cases with red parent: (inserted node marked as * - push it up splitting the parent if neccessary):
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
		struct _prbtree_node *g = PRB_GET_PARENT(p); /* p is red */
		struct _prbtree_node *t = g->prbtree_left;
		for (;;) {
			if (p != t) {
				if (t && PRB_BLACK_COLOR != PRB_GET_COLOR(t))
					break; /* case 3,4 */
				/* cases 1,2 */
				if (p->prbtree_left == e) {
					/* case 2 */
					t = e->prbtree_right;
					if (t) /* NULL on first iteration, != on next iterations */
						PRB_SET_PARENT_COLOR(t, p, PRB_BLACK_COLOR);
					p->prbtree_left = t;
					PRB_SET_PARENT_COLOR(p, e, PRB_RED_COLOR);
					e->prbtree_right = p;
					p = e;
				}
				/* case 1 */
				t = p->prbtree_left;
				g->prbtree_right = t;
				p->prbtree_left = g;
			}
			else {
				t = g->prbtree_right;
				if (t && PRB_BLACK_COLOR != PRB_GET_COLOR(t))
					break; /* case 3,4 */
				/* cases 1,2 */
				if (p->prbtree_left != e) {
					/* case 2 */
					t = e->prbtree_left;
					if (t) /* NULL on first iteration, != on next iterations */
						PRB_SET_PARENT_COLOR(t, p, PRB_BLACK_COLOR);
					p->prbtree_right = t;
					PRB_SET_PARENT_COLOR(p, e, PRB_RED_COLOR);
					e->prbtree_left = p;
					p = e;
				}
				/* case 1 */
				t = p->prbtree_right;
				g->prbtree_left = t;
				p->prbtree_right = g;
			}
			if (t) /* NULL on first iteration, != on next iterations */
				PRB_SET_PARENT_COLOR(t, g, PRB_BLACK_COLOR);
			t = PRB_BLACK_NODE_PARENT(g); /* NULL? */
			__prbtree_replace_at_parent(tree, t, g, p);
			PRB_SET_PARENT_COLOR(p, t, PRB_BLACK_COLOR);
			PRB_SET_PARENT_COLOR(g, p, PRB_RED_COLOR);
			return; /* (final) */
		}
		/* cases 3,4 */
		PRB_SET_PARENT_COLOR(t, g, PRB_BLACK_COLOR); /* recolor t: red -> black */
		PRB_SET_PARENT_COLOR(p, g, PRB_BLACK_COLOR); /* recolor p: red -> black */
		p = PRB_BLACK_NODE_PARENT(g);
		if (!p)
			return;
		PRB_SET_PARENT_COLOR(g, p, PRB_RED_COLOR); /* recolor g: black -> red */
		e = g;
	}
}

static inline void __prbtree_remove(
	A_In struct _prbtree *tree/*!=NULL*/,
	A_In struct _prbtree_node *p/*!=NULL*/,
	A_In struct _prbtree_node *e/*!=NULL*/)
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

	3)      -----                        -----------                     -----
	        |3,B|                        |   7,B   |                     |7,B|
	    ------------------  rotate       |3,R   8,R|   recolor     --------------
	    |1,B*| |   7,B   |    ->     ---------------     ->        |   4,B| |8,B|
	    ------ |4,R   8,R|           |1,B*| |4,B|                  |3,R   | -----
	           -----------           ------ -----              ------------
	                                                           |1,B*|
	                                                           ------
	4)      -----------                  -----------                     -----
	        |3,B      |                  |      7,B|                     |7,B|
	        |      7,R|     rotate       |3,R      |   recolor     ------------
	    ----------------      ->     ----------------    ->        |   4,B| [x]
	    |1,B*| |4,B| [x]             |1,B*| |4,B| [x]              |3,R   |
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
	    |1,B*| |   5,B   | [x]            |1,B*| |   5,B   | [x]              |3,R      6,R|                |   4,B| |6,B| [x]
	    ------ |4,R   6,R|                ------ |4,R   6,R|              ------------------                |3,R   | -----
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
		struct _prbtree_node *a;
		struct _prbtree_node *t = p->prbtree_left;
		char *_p;
		if (PRB_BLACK_COLOR == PRB_GET_COLOR(p)) {
			if (e == t) {
				t = p->prbtree_right;
				a = t->prbtree_left;
				/* parent is black: cases 1,2,3,4,5,6 */
				if (PRB_BLACK_COLOR == PRB_GET_COLOR(t)) {
					/* t is black: cases 1,2,3 */
					struct _prbtree_node *A_Restrict b = t->prbtree_right;
					if (a && PRB_BLACK_COLOR != PRB_GET_COLOR(a)) { /* red on first iteration */
						if (b && PRB_BLACK_COLOR != PRB_GET_COLOR(b)) { /* red on first iteration */
							/* case 3 */
							PRB_SET_PARENT_COLOR(b, t, PRB_BLACK_COLOR);
							PRB_SET_PARENT_COLOR(a, t, PRB_BLACK_COLOR);
						}
						else {
							/* case 2 */
							PRB_SET_PARENT_COLOR(t, a, PRB_BLACK_COLOR);
							t->prbtree_left = a->prbtree_right;
							if (a->prbtree_right) /* NULL on first iteration */
								PRB_SET_PARENT_COLOR(a->prbtree_right, t, PRB_BLACK_COLOR);
							a->prbtree_right = t;
							t = a;
						}
					}
					else if (b && PRB_BLACK_COLOR != PRB_GET_COLOR(b)) { /* red on first iteration */
						/* case 2 */
						PRB_SET_PARENT_COLOR(b, t, PRB_BLACK_COLOR);
						a = t;
					}
					else {
						/* case 1 */
						PRB_SET_PARENT_COLOR(t, p, PRB_RED_COLOR); /* recolor brother: black -> red */
						e = p;
						p = PRB_BLACK_NODE_PARENT(p);
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
						struct _prbtree_node *c = a->prbtree_left;
						if (c && PRB_BLACK_COLOR != PRB_GET_COLOR(c)) { /* red on first iteration */
							/* cases 5,6 */
							struct _prbtree_node *d = a->prbtree_right;
							if (d && PRB_BLACK_COLOR != PRB_GET_COLOR(d)) { /* red on first iteration */
								/* case 6 */
								PRB_SET_PARENT_COLOR(a, t, PRB_RED_COLOR);
								PRB_SET_PARENT_COLOR(d, a, PRB_BLACK_COLOR);
								PRB_SET_PARENT_COLOR(c, a, PRB_BLACK_COLOR);
							}
							else {
								/* case 5 */
								t->prbtree_left = c;
								if (c->prbtree_right) /* NULL on first iteration */
									PRB_SET_PARENT_COLOR(c->prbtree_right, a, PRB_BLACK_COLOR);
								a->prbtree_left = c->prbtree_right;
								PRB_SET_PARENT_COLOR(a, c, PRB_RED_COLOR);
								PRB_SET_PARENT_COLOR(c, t, PRB_BLACK_COLOR);
								c->prbtree_right = a;
							}
							a = c;
						}
					}
				}
				if (a->prbtree_left) /* NULL on first iteration */
					PRB_SET_PARENT_COLOR(a->prbtree_left, p, PRB_BLACK_COLOR);
				p->prbtree_right = a->prbtree_left;
				a->prbtree_left = p; /* cases 4,5 */
			}
			else {
				a = t->prbtree_right;
				/* parent is black: cases 1,2,3,4,5,6 */
				if (PRB_BLACK_COLOR == PRB_GET_COLOR(t)) {
					/* t is black: cases 1,2,3 */
					struct _prbtree_node *A_Restrict b = t->prbtree_left;
					if (a && PRB_BLACK_COLOR != PRB_GET_COLOR(a)) { /* red on first iteration */
						if (b && PRB_BLACK_COLOR != PRB_GET_COLOR(b)) { /* red on first iteration */
							/* case 3 */
							PRB_SET_PARENT_COLOR(b, t, PRB_BLACK_COLOR);
							PRB_SET_PARENT_COLOR(a, t, PRB_BLACK_COLOR);
						}
						else {
							/* case 2 */
							PRB_SET_PARENT_COLOR(t, a, PRB_BLACK_COLOR);
							t->prbtree_right = a->prbtree_left;
							if (a->prbtree_left) /* NULL on first iteration */
								PRB_SET_PARENT_COLOR(a->prbtree_left, t, PRB_BLACK_COLOR);
							a->prbtree_left = t;
							t = a;
						}
					}
					else if (b && PRB_BLACK_COLOR != PRB_GET_COLOR(b)) { /* red on first iteration */
						/* case 2 */
						PRB_SET_PARENT_COLOR(b, t, PRB_BLACK_COLOR);
						a = t;
					}
					else {
						/* case 1 */
						PRB_SET_PARENT_COLOR(t, p, PRB_RED_COLOR); /* recolor brother: black -> red */
						e = p;
						p = PRB_BLACK_NODE_PARENT(p);
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
						struct _prbtree_node *c = a->prbtree_right;
						if (c && PRB_BLACK_COLOR != PRB_GET_COLOR(c)) {
							/* cases 5,6 */
							struct _prbtree_node *d = a->prbtree_left;
							if (d && PRB_BLACK_COLOR != PRB_GET_COLOR(d)) {
								/* case 6 */
								PRB_SET_PARENT_COLOR(a, t, PRB_RED_COLOR);
								PRB_SET_PARENT_COLOR(d, a, PRB_BLACK_COLOR);
								PRB_SET_PARENT_COLOR(c, a, PRB_BLACK_COLOR);
							}
							else {
								/* case 5 */
								t->prbtree_right = c;
								if (c->prbtree_left) /* NULL on first iteration */
									PRB_SET_PARENT_COLOR(c->prbtree_left, a, PRB_BLACK_COLOR);
								a->prbtree_right = c->prbtree_left;
								PRB_SET_PARENT_COLOR(a, c, PRB_RED_COLOR);
								PRB_SET_PARENT_COLOR(c, t, PRB_BLACK_COLOR);
								c->prbtree_left = a;
							}
							a = c;
						}
					}
				}
				if (a->prbtree_right) /* NULL on first iteration */
					PRB_SET_PARENT_COLOR(a->prbtree_right, p, PRB_BLACK_COLOR);
				p->prbtree_left = a->prbtree_right;
				a->prbtree_right = p; /* cases 4,5 */
			}
			{
				struct _prbtree_node *g = PRB_BLACK_NODE_PARENT(p);
				PRB_SET_PARENT_COLOR(t, g, PRB_BLACK_COLOR);
				__prbtree_replace_at_parent(tree, g, p, t);
			}
			_p = _PRB_MAKE_PARENT_COLOR(a, t == a ? PRB_BLACK_COLOR : PRB_RED_COLOR);
		}
		else {
			struct _prbtree_node *g = PRB_GET_PARENT(p);
			if (e == t) {
				t = p->prbtree_right;
				a = t->prbtree_left;
				{
					/* parent is red: cases 4,5,6,7,8,9 (10:4,11:5,12:6,13:7,14:8,15:9) */
					struct _prbtree_node *A_Restrict b = t->prbtree_right;
					if (a && PRB_BLACK_COLOR != PRB_GET_COLOR(a)) {
						/* cases 5,6,8,9 (11:5,12:6,14:8,15:9) */
						if (b && PRB_BLACK_COLOR != PRB_GET_COLOR(b)) {
							/* cases 6,9 (12:6,15:9) */
							PRB_SET_PARENT_COLOR(t, g, PRB_RED_COLOR);
							PRB_SET_PARENT_COLOR(b, t, PRB_BLACK_COLOR);
							PRB_SET_PARENT_COLOR(a, t, PRB_BLACK_COLOR);
						}
						else {
							/* cases 5,8 (11:5,14:8) */
							PRB_SET_PARENT_COLOR(t, a, PRB_RED_COLOR);
							t->prbtree_left = a->prbtree_right;
							if (a->prbtree_right) /* NULL on first iteration */
								PRB_SET_PARENT_COLOR(a->prbtree_right, t, PRB_BLACK_COLOR);
							a->prbtree_right = t;
							PRB_SET_PARENT_COLOR(a, g, PRB_BLACK_COLOR);
							t = a;
						}
					}
					else if (b && PRB_BLACK_COLOR != PRB_GET_COLOR(b)) {
						/* cases 5,8 (11:5,14:8) */
						PRB_SET_PARENT_COLOR(t, g, PRB_BLACK_COLOR);
						a = t;
					}
					else {
						/* cases 4,7 (10:4,13:7) */
						PRB_SET_PARENT_COLOR(t, p, PRB_RED_COLOR);   /* recolor: black -> red */
						PRB_SET_PARENT_COLOR(p, g, PRB_BLACK_COLOR); /* recolor: red -> black */
						return;
					}
				}
				if (a->prbtree_left) /* NULL on first iteration */
					PRB_SET_PARENT_COLOR(a->prbtree_left, p, PRB_BLACK_COLOR);
				p->prbtree_right = a->prbtree_left;
				a->prbtree_left = p;
			}
			else {
				a = t->prbtree_right;
				{
					/* parent is red: cases 4,5,6,7,8,9 (10:4,11:5,12:6,13:7,14:8,15:9) */
					struct _prbtree_node *A_Restrict b = t->prbtree_left;
					if (a && PRB_BLACK_COLOR != PRB_GET_COLOR(a)) {
						/* cases 5,6,8,9 (11:5,12:6,14:8,15:9) */
						if (b && PRB_BLACK_COLOR != PRB_GET_COLOR(b)) {
							/* cases 6,9 (12:6,15:9) */
							PRB_SET_PARENT_COLOR(t, g, PRB_RED_COLOR);
							PRB_SET_PARENT_COLOR(b, t, PRB_BLACK_COLOR);
							PRB_SET_PARENT_COLOR(a, t, PRB_BLACK_COLOR);
						}
						else {
							/* cases 5,8 (11:5,14:8) */
							PRB_SET_PARENT_COLOR(t, a, PRB_RED_COLOR);
							t->prbtree_right = a->prbtree_left;
							if (a->prbtree_left) /* NULL on first iteration */
								PRB_SET_PARENT_COLOR(a->prbtree_left, t, PRB_BLACK_COLOR);
							a->prbtree_left = t;
							PRB_SET_PARENT_COLOR(a, g, PRB_BLACK_COLOR);
							t = a;
						}
					}
					else if (b && PRB_BLACK_COLOR != PRB_GET_COLOR(b)) {
						/* cases 5,8 (11:5,14:8) */
						PRB_SET_PARENT_COLOR(t, g, PRB_BLACK_COLOR);
						a = t;
					}
					else {
						/* cases 4,7 (10:4,13:7) */
						PRB_SET_PARENT_COLOR(t, p, PRB_RED_COLOR);   /* recolor: black -> red */
						PRB_SET_PARENT_COLOR(p, g, PRB_BLACK_COLOR); /* recolor: red -> black */
						return;
					}
				}
				if (a->prbtree_right) /* NULL on first iteration */
					PRB_SET_PARENT_COLOR(a->prbtree_right, p, PRB_BLACK_COLOR);
				p->prbtree_left = a->prbtree_right;
				a->prbtree_right = p;
			}
			if (p == g->prbtree_left)
				g->prbtree_left = t; /* case 5,6 (11:5,12:6) */
			else
				g->prbtree_right = t; /* case 8,9 (14:8,15:9) */
			_p = _PRB_MAKE_PARENT_COLOR(a, PRB_RED_COLOR);
		}
		_PRB_SET_PARENT_COLOR(p, _p);
		return;
	}
}

PRBTREE_EXPORTS void _prbtree_remove(
	A_In struct _prbtree *tree/*!=NULL*/,
	A_In struct _prbtree_node *e/*!=NULL*/)
{
	struct _prbtree_node *t = e->prbtree_right;
	if (t) {
		if (t->prbtree_left) {
			do {
				t = t->prbtree_left;
			} while (t->prbtree_left);
			if (t->prbtree_right) {
				/* t is black:
						 -----                    -----
						 |4,B|      rotate        |4,B|
				  ---------------     ->       -----------
				  |2,B*   | |5,B|              |3,B| |5,B|
				  |    3,R| -----          --------- -----
				  ---------                |2,B*|
										   ------           */
				struct _prbtree_node *A_Restrict p = PRB_BLACK_NODE_PARENT(t);
				struct _prbtree_node *A_Restrict r = t->prbtree_right;
				PRB_SET_PARENT_COLOR(r, p, PRB_BLACK_COLOR); /* change parent & recolor node: red -> black */
				p->prbtree_left = r;
				goto _replace;
			}
		}
		else if (t->prbtree_right) {
			/* t is black:
			    -----                      -----
			    |1,B|          rotate      |1,B|
			 ---------------     ->     ---------------
			 |0,B| |2,B*   |            |0,B|     |3,B|
			 ----- |    3,R|            ----- ---------
			       ---------                  |2,B*|
		                                      ------        */
			struct _prbtree_node *A_Restrict r = t->prbtree_right;
			PRB_SET_PARENT_COLOR(r, e, PRB_BLACK_COLOR); /* change parent & recolor node: red -> black */
			e->prbtree_right = r;
			goto _replace;
		}
	}
	else {
		t = e->prbtree_left;
		if (t) {
			/* t is red:
			 ---------
			 |    3,B|
			 |2,R*   |
			 --------- */
			e->prbtree_left = (struct _prbtree_node*)0;
			goto _replace;
		}
		t = e;
	}
	{
		/* remove t from parent */
		struct _prbtree_node *A_Restrict p = PRB_GET_PARENT(t);
		if (!p) {
			tree->root = (struct _prbtree_node*)0;
			return;
		}
		if (PRB_BLACK_COLOR == PRB_GET_COLOR(t))
			__prbtree_remove(tree, p, t); /* t - leaf black node */
		p->u.leaves[t != p->prbtree_left] = (struct _prbtree_node*)0;
	}
	if (t != e) {
_replace:
		_prbtree_replace(tree, e, t); /* replace e with t */
	}
}
