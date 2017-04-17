/**********************************************************************************
* Red-black binary tree of nodes with parent pointers
* Copyright (C) 2012-2017 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

package com.itr.collections;

import com.itr.collections.Btree.BtreeCallback;
import com.itr.collections.Btree.BtreeComparator;
import com.itr.collections.Btree.BtreeKeyExtractorInt;
import com.itr.collections.Btree.BtreeKeyExtractorLong;
import com.itr.collections.Btree.BtreeKeyExtractorObject;
import com.itr.collections.Ptree.BtreeRootAccessor;
import com.itr.collections.Ptree.PtreeModifier;
import com.itr.collections.Ptree.PtreeNodeAccessor;
import com.itr.collections.Ptree.PtreeNodeAccessorImpl;
import java.util.Collection;
import java.util.ListIterator;

/* red-black binary tree with parent pointers, implementation details see in prbtree.c */
public class Prbtree {

	public static interface PrbtreeNodeAccessor<E> extends PtreeNodeAccessor<E> {
		public boolean red(E current);
		public void setRed(E current, boolean red);
	}

	public abstract static class PrbtreeNodeAccessorImpl<E>
		extends PtreeNodeAccessorImpl<E>
		implements PrbtreeNodeAccessor<E>
	{
		//need to implement:
		//public E left(E current);
		//public E right(E current);
		//public E parent(E current);
		//public boolean red(E current);
		//public void setLeft(E current, E n);
		//public void setRight(E current, E n);
		//public void setParent(E current, E n);
		//public void setRed(E current, boolean red);

		private <R> void __prbtree_replace_at_parent(BtreeRootAccessor<R,E> acc, R root, E p/*null?*/, E o, E e) {
			if (null == p)
				acc.setRoot(root, e);
			else if (o == left(p))
				setLeft(p, e);
			else
				setRight(p, e);
		}

		private <R> void __prbtree_insert(final BtreeRootAccessor<R,E> acc, final R root, E p, E e) {
			while (red(p)) {
				E g = parent(p);
				E t = left(g);
				for (;;) {
					if (p != t) {
						if (t != null && red(t))
							break;
						if (left(p) == e) {
							t = right(e);
							if (t != null)
								setParent(t, p);
							setLeft(p, t);
							setParent(p, e);
							setRight(e, p);
							p = e;
						}
						else {
							setRed(e, true);
							setParent(e, p);
						}
						t = left(p);
						setRight(g, t);
						setLeft(p, g);
					}
					else {
						t = right(g);
						if (t != null && red(t))
							break;
						if (left(p) != e) {
							t = left(e);
							if (t != null)
								setParent(t, p);
							setRight(p, t);
							setParent(p, e);
							setLeft(e, p);
							p = e;
						}
						else {
							setRed(e, true);
							setParent(e, p);
						}
						t = right(p);
						setLeft(g, t);
						setRight(p, g);
					}
					if (t != null)
						setParent(t, g);
					t = parent(g);
					__prbtree_replace_at_parent(acc, root, t, g, p);
					setParent(p, t);
					setRed(p, false);
					setParent(g, p);
					setRed(g, true);
					return;
				}
				setRed(t, false);
				setRed(p, false);
				t = parent(g);
				if (t == null)
					break;
				setParent(e, p);
				setRed(e, true);
				p = t;
				e = g;
			}
			setParent(e, p);
			setRed(e, true);
		}

		/* insert new node into the tree below the right- or left-leaf parent */
		/* root - root node container;
		  p - parent node, if null - the root;
		  r - true if parent key < e key, (p.right must be null)
		  r - false if parent key >= e key, (p.left must be null) */
		@Override public <R> void insertAtParent(BtreeRootAccessor<R,E> acc, R root, E p/*null?*/, E e, boolean r) {
			assert left(e) == null && right(e) == null && parent(e) == null && !red(e) : "inserting uncleared node!";
			if (null == p)
				acc.setRoot(root, e); /* black node */
			else {
				if (r) {
					assert right(p) == null : "not a right-leaf parent!";
					setRight(p, e);
				}
				else {
					assert left(p) == null : "not a left-leaf parent!";
					setLeft(p, e);
				}
				__prbtree_insert(acc, root, p, e);
			}
		}

		/* replace old node with a new one in the tree */
		@Override public <R> void replace(BtreeRootAccessor<R,E> acc, R root, E old, E e) {
			E n = parent(old);
			setParent(e, n);
			__prbtree_replace_at_parent(acc, root, n, old, e);
			n = left(old);
			setLeft(e, n);
			if (null != n)
				setParent(n, e);
			n = right(old);
			setRight(e, n);
			if (null != n)
				setParent(n, e);
			setRed(e, red(old));
		}

		/* remove node from the tree */
		@Override public <R> void remove(BtreeRootAccessor<R,E> acc, R root, E e) {
			E t = right(e);
			for (;;) {
				if (t != null) {
					E n = left(t);
					if (n != null) {
						do {
							t = n;
							n = left(t);
						} while (n != null);
						n = right(t);
						if (n != null) {
							E p = parent(t);
							setLeft(p, n);
							setRed(n, false);
							setParent(n, p);
							break;
						}
					}
					else {
						n = right(t);
						if (n != null) {
							setRight(e, n);
							setRed(n, false);
							setParent(n, e);
							break;
						}
					}
				}
				else {
					t = left(e);
					if (t != null) {
						setLeft(e, null);
						break;
					}
					t = e;
				}
				{
					E p = parent(t);
					if (p == null) {
						acc.setRoot(root, null);
						return;
					}
					if (!red(t))
						__prbtree_remove(acc, root, p, t);
					if (t != left(p))
						setRight(p, null);
					else
						setLeft(p, null);
				}
				if (t == e)
					return;
				break;
			}
			replace(acc, root, e, t);
		}

		private <R> void __prbtree_remove(final BtreeRootAccessor<R,E> acc, final R root, E p, E e) {
			for (;;) {
				E a;
				E t = left(p);
				if (!red(p)) {
					if (e == t) {
						t = right(p);
						a = left(t);
						if (!red(t)) {
							E b = right(t);
							if (a != null && red(a)) {
								if (b != null && red(b)) {
									setRed(b, false);
									setRed(a, false);
									setRed(p, true);
								}
								else {
									setParent(t, a);
									b = right(a);
									setLeft(t, b);
									if (b != null)
										setParent(b, t);
									setRight(a, t);
									setRed(a, false);
									t = a;
								}
							}
							else if (b != null && red(b)) {
								setParent(b, t);
								setRed(b, false);
								a = t;
							}
							else {
								setRed(t, true);
								e = p;
								p = parent(p);
								if (p == null)
									return;
								continue;
							}
						}
						else {
							E c = left(a);
							if (c != null && red(c)) {
								E d = right(a);
								if (d != null && red(d)) {
									setRed(a, true);
									setRed(d, false);
									setRed(c, false);
								}
								else {
									setLeft(t, c);
									d = right(c);
									if (d != null)
										setParent(d, a);
									setLeft(a, d);
									setParent(a, c);
									setRed(a, true);
									setParent(c, t);
									setRed(c, false);
									setRight(c, a);
								}
								a = c;
							}
							setRed(t, false);
							setRed(p, true);
						}
						{
							E n = left(a);
							if (n != null)
								setParent(n, p);
							setRight(p, n);
						}
						setLeft(a, p);
					}
					else {
						a = right(t);
						if (!red(t)) {
							E b = left(t);
							if (a != null && red(a)) {
								if (b != null && red(b)) {
									setRed(b, false);
									setRed(a, false);
									setRed(p, true);
								}
								else {
									setParent(t, a);
									b = left(a);
									setRight(t, b);
									if (b != null)
										setParent(b, t);
									setLeft(a, t);
									setRed(a, false);
									t = a;
								}
							}
							else if (b != null && red(b)) {
								setParent(b, t);
								setRed(b, false);
								a = t;
							}
							else {
								setRed(t, true);
								e = p;
								p = parent(p);
								if (p == null)
									return;
								continue;
							}
						}
						else {
							E c = right(a);
							if (c != null && red(c)) {
								E d = left(a);
								if (d != null && red(d)) {
									setRed(a, true);
									setRed(d, false);
									setRed(c, false);
								}
								else {
									setRight(t, c);
									d = left(c);
									if (d != null)
										setParent(d, a);
									setRight(a, d);
									setParent(a, c);
									setRed(a, true);
									setParent(c, t);
									setRed(c, false);
									setLeft(c, a);
								}
								a = c;
							}
							setRed(t, false);
							setRed(p, true);
						}
						{
							E n = right(a);
							if (n != null)
								setParent(n, p);
							setLeft(p, n);
						}
						setRight(a, p);
					}
					{
						E g = parent(p);
						setParent(t, g);
						__prbtree_replace_at_parent(acc, root, g, p, t);
					}
				}
				else {
					E g = parent(p);
					if (e == t) {
						t = right(p);
						a = left(t);
						{
							E b = right(t);
							if (a != null && red(a)) {
								if (b != null && red(b)) {
									setParent(t, g);
									setRed(t, true);
									setRed(b, false);
									setRed(a, false);
								}
								else {
									setParent(t, a);
									setRed(t, true);
									b = right(a);
									setLeft(t, b);
									if (b != null)
										setParent(b, t);
									setRight(a, t);
									setParent(a, g);
									setRed(a, false);
									t = a;
								}
							}
							else if (b != null && red(b)) {
								setParent(t, g);
								a = t;
							}
							else {
								setRed(t, true);
								setRed(p, false);
								return;
							}
						}
						{
							E n = left(a);
							if (n != null)
								setParent(n, p);
							setRight(p, n);
						}
						setLeft(a, p);
					}
					else {
						a = right(t);
						{
							E b = left(t);
							if (a != null && red(a)) {
								if (b != null && red(b)) {
									setParent(t, g);
									setRed(t, true);
									setRed(b, false);
									setRed(a, false);
								}
								else {
									setParent(t, a);
									setRed(t, true);
									b = left(a);
									setRight(t, b);
									if (b != null)
										setParent(b, t);
									setLeft(a, t);
									setParent(a, g);
									setRed(a, false);
									t = a;
								}
							}
							else if (b != null && red(b)) {
								setParent(t, g);
								a = t;
							}
							else {
								setRed(t, true);
								setRed(p, false);
								return;
							}
						}
						{
							E n = right(a);
							if (n != null)
								setParent(n, p);
							setLeft(p, n);
						}
						setRight(a, p);
					}
					{
						E n = left(g);
						if (p == n)
							setLeft(g, t);
						else
							setRight(g, t);
					}
				}
				setParent(p, a);
				return;
			}
		}
	}

	public abstract static class PrbtreeModifierImpl<R,E>
		extends PrbtreeNodeAccessorImpl<E>
		implements PtreeModifier<R,E>
	{
		//need to implement:
		//public E left(E current);
		//public E right(E current);
		//public E parent(E current);
		//public E root(R root);
		//public boolean red(E current);
		//public void setLeft(E current, E n);
		//public void setRight(E current, E n);
		//public void setParent(E current, E n);
		//public void setRoot(R root, E tree);
		//public void setRed(E current, boolean red);
		@Override public E first(R root) {
			return treeFirst(root(root));
		}
		@Override public E last(R root) {
			return treeLast(root(root));
		}
		@Override public int size(R root) {
			return treeSize(root(root));
		}
		@Override public boolean contains(R root, E n) {
			return treeContains(root(root), n);
		}
		@Override public boolean containsAll(R root, Collection<?> c) {
			return treeContainsAll(root(root), c);
		}
		@Override public <T> T[] toArray(R root, T[] a) {
			return treeToArray(root(root), a);
		}
		@Override public Object[] toArray(R root) {
			return treeToArray(root(root));
		}
		@Override public <D> E walkRecursive(R root, D data, BtreeCallback<? super E,D> cb) {
			return treeWalkRecursive(root(root), data, cb);
		}
		@Override public <D> E walkRecursiveForward(R root, D data, BtreeCallback<? super E,D> cb) {
			return treeWalkRecursiveForward(root(root), data, cb);
		}
		@Override public <D> E walkRecursiveBackward(R root, D data, BtreeCallback<? super E,D> cb) {
			return treeWalkRecursiveBackward(root(root), data, cb);
		}
		@Override public E searchRecursiveInt(R root, int key, BtreeKeyExtractorInt<? super E> ext) {
			return treeSearchRecursiveInt(root(root), key, ext);
		}
		@Override public E searchRecursiveLong(R root, long key, BtreeKeyExtractorLong<? super E> ext) {
			return treeSearchRecursiveLong(root(root), key, ext);
		}
		@Override public <K> E searchRecursiveObject(R root, K key, BtreeKeyExtractorObject<? super E,K> ext) {
			return treeSearchRecursiveObject(root(root), key, ext);
		}
		@Override public <K> E search(R root, K key, BtreeComparator<? super E,K> cmp) {
			return treeSearch(root(root), key, cmp);
		}
		@Override public E searchInt(R root, int key, BtreeKeyExtractorInt<? super E> ext) {
			return treeSearchInt(root(root), key, ext);
		}
		@Override public E searchLong(R root, long key, BtreeKeyExtractorLong<? super E> ext) {
			return treeSearchLong(root(root), key, ext);
		}
		@Override public <K> E searchObject(R root, K key, BtreeKeyExtractorObject<? super E,K> ext) {
			return treeSearchObject(root(root), key, ext);
		}
		@Override public ListIterator<E> readIterator(R root) {
			return treeReadIterator(root(root));
		}
		@Override public Collection<E> readCollection(R root) {
			return treeReadCollection(root(root));
		}
		@Override public void insertAtParent(R root, E p/*null?*/, E e, boolean r) {
			insertAtParent(this, root, p, e, r);
		}
		@Override public <K> E insert(R root, E e, K key, BtreeComparator<? super E,K> cmp, boolean allow_dup) {
			return insert(this, root, e, key, cmp, allow_dup);
		}
		@Override public E insertInt(R root, E e, int key, BtreeKeyExtractorInt<? super E> ext, boolean allow_dup) {
			return insertInt(this, root, e, key, ext, allow_dup);
		}
		@Override public E insertLong(R root, E e, long key, BtreeKeyExtractorLong<? super E> ext, boolean allow_dup) {
			return insertLong(this, root, e, key, ext, allow_dup);
		}
		@Override public <K> E insertObject(R root, E e, K key, BtreeKeyExtractorObject<? super E,K> ext, boolean unique, boolean allow_dup) {
			return insertObject(this, root, e, key, ext, unique, allow_dup);
		}
		@Override public E insertInt(R root, E e, BtreeKeyExtractorInt<? super E> ext) {
			return insertInt(this, root, e, ext);
		}
		@Override public E insertLong(R root, E e, BtreeKeyExtractorLong<? super E> ext) {
			return insertLong(this, root, e, ext);
		}
		@Override public <K> E insertObject(R root, E e, BtreeKeyExtractorObject<? super E,K> ext) {
			return insertObject(this, root, e, ext);
		}
		@Override public void replace(R root, E old, E e) {
			replace(this, root, old, e);
		}
		@Override public void remove(R root, E e) {
			remove(this, root, e);
		}
		@Override public ListIterator<E> iterator(R root) {
			return iterator(this, root);
		}
		@Override public Collection<E> collection(R root) {
			return collection(this, root);
		}
		@Override public boolean isEmpty(R root) {
			return null == root(root);
		}
		@Override public void clear(R root) {
			setRoot(root, null);
		}
	}

// examples:
/*
	public static interface IPNode {
		public int getData();
	}

	public static class PNode implements IPNode {
		protected int data;

		public int a_key;
		public PNode a_left;
		public PNode a_right;
		public PNode a_parent;
		public boolean a_red;

		public long b_key;
		public PNode b_left;
		public PNode b_right;
		public PNode b_parent;

		@Override public int getData() {return data;}

		public static final PrbtreeNodeAccessor<PNode> a_accessor = new PrbtreeNodeAccessorImpl<PNode>() {
			@Override public PNode left(PNode c) {return c.a_left;}
			@Override public PNode right(PNode c) {return c.a_right;}
			@Override public PNode parent(PNode c) {return c.a_parent;}
			@Override public boolean red(PNode c) {return c.a_red;}
			@Override public void setLeft(PNode c, PNode n) {c.a_left = n;}
			@Override public void setRight(PNode c, PNode n) {c.a_right = n;}
			@Override public void setParent(PNode c, PNode n) {c.a_parent = n;}
			@Override public void setRed(PNode c, boolean red) {c.a_red = red;}
		};
		public static final PtreeNodeAccessor<PNode> b_accessor = new PtreeNodeAccessorImpl<PNode>() {
			@Override public PNode left(PNode c) {return c.b_left;}
			@Override public PNode right(PNode c) {return c.b_right;}
			@Override public PNode parent(PNode c) {return c.b_parent;}
			@Override public void setLeft(PNode c, PNode n) {c.b_left = n;}
			@Override public void setRight(PNode c, PNode n) {c.b_right = n;}
			@Override public void setParent(PNode c, PNode n) {c.b_parent = n;}
		};
		public static final com.itr.collections.Btree.BtreeSearcherInt<PNode> a_searcher =
			new com.itr.collections.Btree.BtreeSearcherInt<PNode>()
		{
			@Override public PNode left(PNode c) {return c.a_left;}
			@Override public PNode right(PNode c) {return c.a_right;}
			@Override public int key(PNode node) {return node.a_key;}
		};
		public static final com.itr.collections.Btree.BtreeSearcherLong<PNode> b_searcher =
			new com.itr.collections.Btree.BtreeSearcherLong<PNode>()
		{
			@Override public PNode left(PNode c) {return c.b_left;}
			@Override public PNode right(PNode c) {return c.b_right;}
			@Override public long key(PNode node) {return node.b_key;}
		};
	}

	public static class PContainer1 {
		public int data;
		public PNode a_root;
		public PNode b_root;

		public static final BtreeRootAccessor<PContainer1, PNode> a_tree_accessor =
			new com.itr.collections.Ptree.BtreeRootAccessorImpl<PContainer1, PNode>()
		{
			@Override public PNode root(PContainer1 root) {return root.a_root;}
			@Override public void setRoot(PContainer1 root, PNode n) {root.a_root = n;}
		};
		public static final PtreeModifier<PContainer1, PNode> a_tree_modifier =
			new PrbtreeModifierImpl<PContainer1, PNode>()
		{
			@Override public PNode root(PContainer1 root)            {return root.a_root;}
			@Override public PNode left(PNode current)               {return current.a_left;}
			@Override public PNode right(PNode current)              {return current.a_right;}
			@Override public PNode parent(PNode current)             {return current.a_parent;}
			@Override public boolean red(PNode current)              {return current.a_red;}
			@Override public void setRoot(PContainer1 root, PNode n) {root.a_root = n;}
			@Override public void setLeft(PNode current, PNode n)    {current.a_left = n;}
			@Override public void setRight(PNode current, PNode n)   {current.a_right = n;}
			@Override public void setParent(PNode current, PNode n)  {current.a_parent = n;}
			@Override public void setRed(PNode current, boolean red) {current.a_red = red;}
		};
		public static final PtreeModifier<PContainer1, PNode> b_tree_modifier =
			new com.itr.collections.Ptree.PtreeModifierImpl<PContainer1, PNode>()
		{
			@Override public PNode root(PContainer1 root)            {return root.b_root;}
			@Override public PNode left(PNode current)               {return current.b_left;}
			@Override public PNode right(PNode current)              {return current.b_right;}
			@Override public PNode parent(PNode current)             {return current.b_parent;}
			@Override public void setRoot(PContainer1 root, PNode n) {root.b_root = n;}
			@Override public void setLeft(PNode current, PNode n)    {current.b_left = n;}
			@Override public void setRight(PNode current, PNode n)   {current.b_right = n;}
			@Override public void setParent(PNode current, PNode n)  {current.b_parent = n;}
		};

		public ListIterator<PNode> a_iterator() {return PNode.a_accessor.iterator(a_tree_accessor, this);}
		public ListIterator<PNode> b_iterator() {return b_tree_modifier.iterator(this);}
		public Collection<PNode> a_tree() {return PNode.a_accessor.collection(a_tree_accessor, this);}
		public Collection<PNode> b_tree() {return b_tree_modifier.collection(this);}

		public ListIterator<? extends IPNode> a_read_iterator() {return PNode.a_accessor.treeReadIterator(a_tree_accessor.root(this));}
		public ListIterator<? extends PNode> b_read_iterator() {return b_tree_modifier.readIterator(this);}
		public Iterable<? extends IPNode> a_read_tree() {return PNode.a_accessor.treeReadCollection(a_tree_accessor.root(this));}
		public Iterable<? extends PNode> b_read_tree() {return b_tree_modifier.readCollection(this);}

		public PNode a_search(int key) {return PNode.a_searcher.search(a_root, key);}
		public PNode b_search(long key) {return PNode.b_searcher.search(b_root, key);}
	}

	public static void iterate_on_a(PContainer1 c) {
		// iterate without creating any objects
		{
			PNode a = PNode.a_accessor.treeFirst(c.a_root);
			while (a != null) {
				PNode.a_accessor.remove(PContainer1.a_tree_accessor, c, a);
				a = PNode.a_accessor.next(a);
			}
		}
		// create iterator
		{
			ListIterator<PNode> i = c.a_iterator();
			while (i.hasNext()) {
				PNode a = i.next();
			}
		}
		// create object which creates iterators
		{
			for (PNode a : c.a_tree()) {
			}
		}
		// create read iterator
		{
			ListIterator<? extends IPNode> i = c.a_read_iterator();
			while (i.hasNext()) {
				IPNode ai = i.next();
			}
		}
		// create object which creates read iterators
		{
			for (IPNode ai : c.a_read_tree()) {
			}
		}
		// create read iterator
		{
			ListIterator<? extends PNode> i = c.b_read_iterator();
			while (i.hasNext()) {
				PNode bi = i.next();
			}
		}
		// create object which creates read iterators
		{
			for (PNode bi : c.b_read_tree()) {
			}
		}
		// search node in b-tree
		{
			PNode n = c.b_search(1);
			if (null != n)
				PContainer1.b_tree_modifier.remove(c, n);
		}
	}
*/
}
