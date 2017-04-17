/**********************************************************************************
* Simple binary tree of nodes with parent pointers
* Copyright (C) 2012-2017 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

package com.itr.collections;

import com.itr.collections.Btree.BtreeCallback;
import com.itr.collections.Btree.BtreeComparator;
import com.itr.collections.Btree.BtreeKeyExtractorInt;
import com.itr.collections.Btree.BtreeKeyExtractorLong;
import com.itr.collections.Btree.BtreeKeyExtractorObject;
import com.itr.collections.Btree.BtreeNodeAccessor;
import com.itr.collections.Btree.BtreeNodeReadAccessor;
import com.itr.collections.Btree.BtreeNodeReadAccessorImpl;
import com.itr.collections.Btree.BtreeParent;
import java.util.Collection;
import java.util.ListIterator;
import java.util.NoSuchElementException;

/* binary tree with parent pointers */
public class Ptree {

	/* accessor to tree node 'left', 'right' and 'parent' fields */
	public static interface PtreeNodeReadAccessor<E> extends BtreeNodeReadAccessor<E> {

		public E parent(E current); /* returns null for the root node */
		public E next(E current);   /* get next node, returns null for the rightmost node */
		public E prev(E current);   /* get previous node, returns null for the leftmost node */

		/* read-only iterator and collection */
		public ListIterator<E> treeReadIterator(E tree/*null?*/);
		public Collection<E> treeReadCollection(E tree/*null?*/);
	}

	/* accessor to 'root' field of object containing the tree */
	public static interface BtreeRootReadAccessor<R,E> {
		public E root(R root);                /* returns null if tree is empty */
		public boolean isEmpty(R root);       /* check root for null */
	}

	/* accessor to 'root' field of object containing the tree */
	public static interface BtreeRootAccessor<R,E> extends BtreeRootReadAccessor<R,E> {
		public void setRoot(R root, E tree);
		public void clear(R root);            /* sets root to null */
	}

	public static interface PtreeNodeAccessor<E> extends PtreeNodeReadAccessor<E>, BtreeNodeAccessor<E> {

		public void setParent(E current, E n);

		/* insert new node into the tree below the right- or left-leaf parent */
		/* root - root node container;
		  p - parent node, if null - the root;
		  r - true if parent key < e key, (p.right must be null)
		  r - false if parent key >= e key, (p.left must be null) */
		public <R> void insertAtParent(BtreeRootAccessor<R,E> acc, R root, E p/*null?*/, E e, boolean r);

		/* insert new entry into ordered tree, key - the key of e,
		  if !allow_dup - may return existing node with given key,
		  else returns null - new node is inserted */
		public <R,K> E insert(BtreeRootAccessor<R,E> acc, R root, E e, K key, BtreeComparator<? super E,K> cmp, boolean allow_dup);
		public <R> E insertInt(BtreeRootAccessor<R,E> acc, R root, E e, int key, BtreeKeyExtractorInt<? super E> ext, boolean allow_dup);
		public <R> E insertLong(BtreeRootAccessor<R,E> acc, R root, E e, long key, BtreeKeyExtractorLong<? super E> ext, boolean allow_dup);

		/* insert new entry into key-identity-hash ordered tree */
		/* unique - true if inserting node with unique key,
		   else if !allow_dup - may return existing node with given key */
		public <R,K> E insertObject(BtreeRootAccessor<R,E> acc, R root, E e, K key,
			BtreeKeyExtractorObject<? super E,K> ext, boolean unique, boolean allow_dup);

		/* insert node with unique key */
		public <R> E insertInt(BtreeRootAccessor<R,E> acc, R root, E e, BtreeKeyExtractorInt<? super E> ext);
		public <R> E insertLong(BtreeRootAccessor<R,E> acc, R root, E e, BtreeKeyExtractorLong<? super E> ext);
		public <R,K> E insertObject(BtreeRootAccessor<R,E> acc, R root, E e, BtreeKeyExtractorObject<? super E,K> ext);

		/* replace old node with a new one in the tree */
		public <R> void replace(BtreeRootAccessor<R,E> acc, R root, E old, E e);

		/* remove node from the tree */
		public <R> void remove(BtreeRootAccessor<R,E> acc, R root, E e);

		/* read-write iterator and collection */
		public <R> ListIterator<E> iterator(BtreeRootAccessor<R,E> acc, R root);
		public <R> Collection<E> collection(BtreeRootAccessor<R,E> acc, R root);
	}

	public static interface PtreeModifier<R,E> extends PtreeNodeAccessor<E>, BtreeRootAccessor<R,E> {
		public E first(R root);
		public E last(R root);
		public int size(R root);
		public boolean contains(R root, E n);
		public boolean containsAll(R root, Collection<?> c);
		public <T> T[] toArray(R root, T[] a);
		public Object[] toArray(R root);
		public <D> E walkRecursive(R root, D data, BtreeCallback<? super E,D> cb);
		public <D> E walkRecursiveForward(R root, D data, BtreeCallback<? super E,D> cb);
		public <D> E walkRecursiveBackward(R root, D data, BtreeCallback<? super E,D> cb);
		public E searchRecursiveInt(R root, int key, BtreeKeyExtractorInt<? super E> ext);
		public E searchRecursiveLong(R root, long key, BtreeKeyExtractorLong<? super E> ext);
		public <K> E searchRecursiveObject(R root, K key, BtreeKeyExtractorObject<? super E,K> ext);
		public <K> E search(R root, K key, BtreeComparator<? super E,K> cmp);
		public E searchInt(R root, int key, BtreeKeyExtractorInt<? super E> ext);
		public E searchLong(R root, long key, BtreeKeyExtractorLong<? super E> ext);
		public <K> E searchObject(R root, K key, BtreeKeyExtractorObject<? super E,K> ext);
		public ListIterator<E> readIterator(R root);
		public Collection<E> readCollection(R root);
		public void insertAtParent(R root, E p/*null?*/, E e, boolean r);
		public <K> E insert(R root, E e, K key, BtreeComparator<? super E,K> cmp, boolean allow_dup);
		public E insertInt(R root, E e, int key, BtreeKeyExtractorInt<? super E> ext, boolean allow_dup);
		public E insertLong(R root, E e, long key, BtreeKeyExtractorLong<? super E> ext, boolean allow_dup);
		public <K> E insertObject(R root, E e, K key, BtreeKeyExtractorObject<? super E,K> ext, boolean unique, boolean allow_dup);
		public E insertInt(R root, E e, BtreeKeyExtractorInt<? super E> ext);
		public E insertLong(R root, E e, BtreeKeyExtractorLong<? super E> ext);
		public <K> E insertObject(R root, E e, BtreeKeyExtractorObject<? super E,K> ext);
		public void replace(R root, E old, E e);
		public void remove(R root, E e);
		public ListIterator<E> iterator(R root);
		public Collection<E> collection(R root);
	}

	/* *************** implementation ************** */

	/* accessor to 'root' field of object containing the tree */
	public abstract static class BtreeRootReadAccessorImpl<R,E> implements BtreeRootReadAccessor<R,E> {

		//need to implement:
		//public E root(R root);

		/* check root for null */
		@Override public boolean isEmpty(R root) {
			return null == root(root);
		}
	}

	/* accessor to 'root' field of object containing the tree */
	public abstract static class BtreeRootAccessorImpl<R,E>
		extends BtreeRootReadAccessorImpl<R,E>
		implements BtreeRootAccessor<R,E>
	{
		//need to implement:
		//public E root(R root);
		//public void setRoot(R root, E tree);

		/* sets root to null */
		@Override public void clear(R root) {
			setRoot(root, null);
		}
	}

	/* accessor to tree node 'left', 'right' and 'parent' fields */
	public abstract static class PtreeNodeReadAccessorImpl<E>
		extends BtreeNodeReadAccessorImpl<E>
		implements PtreeNodeReadAccessor<E>
	{
		//need to implement:
		//public E left(E current);
		//public E right(E current);
		//public E parent(E current);

		protected E _right_parent(E current) {
			for (;;) {
				E t = parent(current);
				if (null == t)
					return null;
				if (left(t) == current)
					return t;
				current = t;
			}
		}

		protected E _left_parent(E current) {
			for (;;) {
				E t = parent(current);
				if (null == t)
					return null;
				if (right(t) == current)
					return t;
				current = t;
			}
		}

		/* get next node, returns null for the rightmost node */
		@Override public E next(E current) {
			E t = right(current);
			if (null != t)
				return _first(t);
			return _right_parent(current);
		}

		/* get previous node, returns null for the leftmost node */
		@Override public E prev(E current) {
			E t = left(current);
			if (null != t)
				return _last(t);
			return _left_parent(current);
		}

		public abstract class PtreeIteratorBase implements ListIterator<E> {
			protected long idx = 0;
			protected E next;
			protected E prev = null;
			protected PtreeIteratorBase(E first/*null?*/)    {next = first;}
			@Override public int nextIndex()       {return idx < Integer.MAX_VALUE ? (int)idx : Integer.MAX_VALUE;}
			@Override public int previousIndex()   {return nextIndex() - 1;}
			@Override public boolean hasNext()     {return next != null;}
			@Override public boolean hasPrevious() {return prev != null;}
			@Override public E next() {
				if (null != next) {
					idx++;
					prev = next;
					next = PtreeNodeReadAccessorImpl.this.next(next);
					return prev;
				}
				throw new NoSuchElementException();
			}
			@Override public E previous() {
				if (null != prev) {
					idx--;
					next = prev;
					prev = PtreeNodeReadAccessorImpl.this.prev(prev);
					return next;
				}
				throw new NoSuchElementException();
			}
		}

		public class PtreeReadIterator extends PtreeIteratorBase {
			protected PtreeReadIterator(E first) {super(first);}
			@Override public void remove() {throw new UnsupportedOperationException();}
			@Override public void set(E e) {throw new UnsupportedOperationException();}
			@Override public void add(E e) {throw new UnsupportedOperationException();}
		}

		@Override public ListIterator<E> treeReadIterator(E tree/*null?*/) {
			return new PtreeReadIterator(treeFirst(tree));
		}

		public class PtreeReadCollection implements Collection<E> {
			protected final E tree;
			protected PtreeReadCollection(E _tree) {tree = _tree;}
			@Override public ListIterator<E> iterator() {
				return PtreeNodeReadAccessorImpl.this.treeReadIterator(tree);
			}
			@Override public boolean isEmpty() {
				return null == tree;
			}
			@Override public int size() {
				return PtreeNodeReadAccessorImpl.this.treeSize(tree);
			}
			@SuppressWarnings("unchecked")
			@Override public boolean contains(Object o) {
				return PtreeNodeReadAccessorImpl.this.treeContains(tree, (E)o);
			}
			@Override public boolean containsAll(Collection<?> c) {
				return PtreeNodeReadAccessorImpl.this.treeContainsAll(tree, c);
			}
			@Override public Object[] toArray() {
				return PtreeNodeReadAccessorImpl.this.treeToArray(tree);
			}
			@Override public <T> T[] toArray(T[] a) {
				return PtreeNodeReadAccessorImpl.this.treeToArray(tree, a);
			}
			@Override public boolean add(E e)                          {throw new UnsupportedOperationException();}
			@Override public boolean remove(Object o)                  {throw new UnsupportedOperationException();}
			@Override public boolean addAll(Collection<? extends E> c) {throw new UnsupportedOperationException();}
			@Override public boolean removeAll(Collection<?> c)        {throw new UnsupportedOperationException();}
			@Override public boolean retainAll(Collection<?> c)        {throw new UnsupportedOperationException();}
			@Override public void clear()                              {throw new UnsupportedOperationException();}
		}

		@Override public Collection<E> treeReadCollection(E tree/*null?*/) {
			return new PtreeReadCollection(tree);
		}
	}

	public abstract static class PtreeNodeAccessorImpl<E>
		extends PtreeNodeReadAccessorImpl<E>
		implements PtreeNodeAccessor<E>
	{
		//need to implement:
		//public E left(E current);
		//public E right(E current);
		//public E parent(E current);
		//public void setLeft(E current, E n);
		//public void setRight(E current, E n);
		//public void setParent(E current, E n);

		/* insert new node into the tree below the right- or left-leaf parent */
		/* root - root node container;
		  p - parent node, if null - the root;
		  r - true if parent key < e key, (p.right must be null)
		  r - false if parent key >= e key, (p.left must be null) */
		@Override public <R> void insertAtParent(BtreeRootAccessor<R,E> acc, R root, E p/*null?*/, E e, boolean r) {
			assert left(e) == null && right(e) == null && parent(e) == null : "inserting uncleared node!";
			if (null == p)
				acc.setRoot(root, e);
			else {
				if (r) {
					assert right(p) == null : "not a right-leaf parent!";
					setRight(p, e);
				}
				else {
					assert left(p) == null : "not a left-leaf parent!";
					setLeft(p, e);
				}
				setParent(e, p);
			}
		}

		/* insert new entry into ordered tree, key - the key of e,
		  if !allow_dup - may return existing node with given key,
		  else returns null - new node is inserted */
		@Override public <R,K> E insert(BtreeRootAccessor<R,E> acc, R root, E e,
			K key, BtreeComparator<? super E,K> cmp, boolean allow_dup)
		{
			assert left(e) == null && right(e) == null && parent(e) == null : "inserting uncleared node!";
			final BtreeParent<E> parent = new BtreeParent<E>(acc.root(root));
			int c = searchParent(parent, key, cmp, /*leaf:*/allow_dup);
			if (c == 0)
				return parent.p; /* exsisting node with the same key found */
			insertAtParent(acc, root, parent.p, e, c < 0);
			return null; /* ok, node inserted */
		}

		/* insert new entry into int-key ordered tree */
		@Override public <R> E insertInt(BtreeRootAccessor<R,E> acc, R root, E e,
			int key, BtreeKeyExtractorInt<? super E> ext, boolean allow_dup)
		{
			assert left(e) == null && right(e) == null && parent(e) == null : "inserting uncleared node!";
			final BtreeParent<E> parent = new BtreeParent<E>(acc.root(root));
			int c = searchParentInt(parent, key, ext, /*leaf:*/allow_dup);
			if (c == 0)
				return parent.p; /* exsisting node with the same key found */
			insertAtParent(acc, root, parent.p, e, c < 0);
			return null; /* ok, node inserted */
		}

		/* insert new entry into long-key ordered tree */
		@Override public <R> E insertLong(BtreeRootAccessor<R,E> acc, R root, E e,
			long key, BtreeKeyExtractorLong<? super E> ext, boolean allow_dup)
		{
			assert left(e) == null && right(e) == null && parent(e) == null : "inserting uncleared node!";
			final BtreeParent<E> parent = new BtreeParent<E>(acc.root(root));
			long c = searchParentLong(parent, key, ext, /*leaf:*/allow_dup);
			if (c == 0)
				return parent.p; /* exsisting node with the same key found */
			insertAtParent(acc, root, parent.p, e, c < 0);
			return null; /* ok, node inserted */
		}

		/* insert new entry into key-identity-hash ordered tree */
		/* unique - true if inserting node with unique key,
		   else if !allow_dup - may return existing node with given key */
		@Override public <R,K> E insertObject(BtreeRootAccessor<R,E> acc, R root, E e,
			K key, BtreeKeyExtractorObject<? super E,K> ext, boolean unique, boolean allow_dup)
		{
			assert left(e) == null && right(e) == null && parent(e) == null : "inserting uncleared node!";
			final BtreeParent<E> parent = new BtreeParent<E>(acc.root(root));
			int c = searchParentObject(parent, key, ext, unique, /*leaf:*/allow_dup);
			if (c == 0)
				return parent.p; /* exsisting node with the same key found */
			insertAtParent(acc, root, parent.p, e, c < 0);
			return null; /* ok, node inserted */
		}

		@Override public <R> E insertInt(BtreeRootAccessor<R,E> acc, R root, E e, BtreeKeyExtractorInt<? super E> ext) {
			return insertInt(acc, root, e, ext.key(e), ext, /*allow_dup:*/false);
		}
		@Override public <R> E insertLong(BtreeRootAccessor<R,E> acc, R root, E e, BtreeKeyExtractorLong<? super E> ext) {
			return insertLong(acc, root, e, ext.key(e), ext, /*allow_dup:*/false);
		}
		@Override public <R,K> E insertObject(BtreeRootAccessor<R,E> acc, R root, E e, BtreeKeyExtractorObject<? super E,K> ext) {
			return insertObject(acc, root, e, ext.key(e), ext, /*unique:*/true, /*allow_dup:*/false);
		}

		/* replace old node with a new one in the tree */
		@Override public <R> void replace(BtreeRootAccessor<R,E> acc, R root, E old, E e) {
			E n = parent(old);
			setParent(e, n);
			if (null == n)
				acc.setRoot(root, e);
			else {
				if (left(n) == old)
					setLeft(n, e);
				else
					setRight(n, e);
			}
			n = left(old);
			setLeft(e, n);
			if (null != n)
				setParent(n, e);
			n = right(old);
			setRight(e, n);
			if (null != n)
				setParent(n, e);
		}

		/* remove node from the tree */
		@Override public <R> void remove(BtreeRootAccessor<R,E> acc, R root, E e) {
			E p = parent(e);
			E l = left(e);
			E r = right(e);
			if (null == l)
				l = r;
			else if (r != null) {
				E n = right(l);
				if (null != n) {
					E q = l;
					for (;;) {
						E t = right(n);
						if (null == t)
							break;
						q = n;
						n = t;
					}
					{
						E t = left(n);
						setRight(q, t);
						if (null != t)
							setParent(t, q);
					}
					setParent(l, n);
					setLeft(n, l);
					l = n;
				}
				setParent(r, l);
				setRight(l, r);
			}
			if (l != null)
				setParent(l, p);
			if (null == p)
				acc.setRoot(root, l);
			else if (left(p) == e)
				setLeft(p, l);
			else
				setRight(p, l);
		}

		public class PtreeIterator<R> extends PtreeIteratorBase {
			protected int dir = 0;
			protected final BtreeRootAccessor<R,E> acc;
			protected final R root;
			protected PtreeIterator(BtreeRootAccessor<R,E> _acc, R _root) {
				super(treeFirst(_acc.root(_root)));
				acc = _acc;
				root = _root;
			}
			@Override public E next() {
				if (null != next) {
					idx++;
					prev = next;
					next = right(next);
					if (null != next) {
						next = _first(next);
						dir = 1; /* next is lower than prev: 0001 */
					}
					else {
						next = _right_parent(prev);
						dir = 3; /* next is higher than prev: 0011 */
					}
					return prev;
				}
				throw new NoSuchElementException();
			}
			@Override public E previous() {
				if (null != prev) {
					idx--;
					next = prev;
					prev = left(prev);
					if (null != prev) {
						prev = _last(prev);
						dir = -1; /* prev is lower than next: 1111 */
					}
					else {
						prev = _left_parent(next);
						dir = -3; /* prev is higher than next: 1101 */
					}
					return next;
				}
				throw new NoSuchElementException();
			}
			@Override public void set(E e) {
				if (0 == (dir & 1))
					throw new IllegalStateException(); /* may call set() only after next() or previous() */
				if (dir > 0) {
					replace(acc, root, prev, e); /* replace node returned by last next() */
					prev = e;
				}
				else {
					replace(acc, root, next, e); /* replace node returned by last previous() */
					next = e;
				}
			}
			/* insert new node between 'prev' and 'next', then advance */
			@Override public void add(E e) {
				assert left(e) == null && right(e) == null && parent(e) == null : "adding uncleared node!";
				if (0 == (dir & 2))
					/* next is lower than prev, next.left == null */
					insertAtParent(acc, root, next, e, /*r:*/false);
				else
					/* prev is lower than next, prev.right == null */
					insertAtParent(acc, root, prev, e, /*r:*/true);
				next = e;
				next();
				dir &= ~1; /* don't allow remove() after add() */
			}
			@Override public void remove() {
				if (0 == (dir & 1))
					throw new IllegalStateException(); /* may call remove() only once after next() or previous() */
				E p = null;
				if (dir > 0) {
					if (next == null) {
						/* removing last node */
						p = left(prev);
						p = (null == p) ? parent(prev) : _last(p);
					}
					PtreeNodeAccessorImpl.this.remove(acc, root, prev);
					if (null != next) {
						prev = next;
						previous();
						dir &= ~1; /* don't allow another remove() */
					}
					else {
						prev = p;
						dir = 2; /* for add(): prev is lower than next: 0010 */
						idx--;
					}
				}
				else {
					if (prev == null) {
						/* removing first node */
						p = right(next);
						p = (null == p) ? parent(next) : _first(p);
					}
					PtreeNodeAccessorImpl.this.remove(acc, root, next);
					if (null != prev) {
						next = prev;
						next();
						idx--;
						dir &= ~1; /* don't allow another remove() */
					}
					else {
						next = p;
						dir = 0; /* for add(): next is lower than prev: 0000 */
					}
				}
			}
		}

		@Override public <R> ListIterator<E> iterator(BtreeRootAccessor<R,E> acc, R root) {
			return new PtreeIterator<R>(acc, root);
		}

		public class PtreeCollection<R> implements Collection<E> {
			protected final BtreeRootAccessor<R,E> acc;
			protected final R root;
			protected PtreeCollection(BtreeRootAccessor<R,E> _acc, R _root) {
				acc = _acc;
				root = _root;
			}
			@Override public boolean isEmpty() {
				return null == acc.root(root);
			}
			@Override public int size() {
				return PtreeNodeAccessorImpl.this.treeSize(acc.root(root));
			}
			@SuppressWarnings("unchecked")
			@Override public boolean contains(Object o) {
				return PtreeNodeAccessorImpl.this.treeContains(acc.root(root), (E)o);
			}
			@Override public boolean containsAll(Collection<?> c) {
				return PtreeNodeAccessorImpl.this.treeContainsAll(acc.root(root), c);
			}
			@Override public Object[] toArray() {
				return PtreeNodeAccessorImpl.this.treeToArray(acc.root(root));
			}
			@Override public <T> T[] toArray(T[] a) {
				return PtreeNodeAccessorImpl.this.treeToArray(acc.root(root), a);
			}
			@Override public ListIterator<E> iterator() {
				return PtreeNodeAccessorImpl.this.iterator(acc, root);
			}
			/* append new entry at tail of collection */
			/* NOTE: assume e is not in collection yet */
			/* NOTE: assume collection c is sorted from lower to higher and
			  first element in collection c have maximal key among tree nodes */
			@Override public boolean add(E e) {
				assert left(e) == null && right(e) == null && parent(e) == null : "adding uncleared node!";
				insertAtParent(acc, root, treeLast(acc.root(root)), e, /*r:*/true);
				return true; /* NOTE: added node must have maximal key among tree nodes */
			}
			/* append new entries at tail of collection */
			/* NOTE: assume new entries are not in collection yet */
			/* NOTE: assume collection c is sorted from lower to higher and
			  first element in collection c have maximal key among tree nodes */
			@Override public boolean addAll(Collection<? extends E> c) {
				if (!c.isEmpty()) {
					E l = treeLast(acc.root(root));
					for (E e : c) {
						insertAtParent(acc, root, l, e, /*r:*/true);
						l = e;
					}
					return true;
				}
				return false; /* collection not changed - nothing to add */
			}
			@SuppressWarnings("unchecked")
			@Override public boolean remove(Object o) {
				E e = (E)o;
				if (!contains(e))
					return false;
				PtreeNodeAccessorImpl.this.remove(acc, root, e);
				return true;
			}
			@SuppressWarnings("unchecked")
			@Override public boolean removeAll(Collection<?> c) {
				if (this.isEmpty())
					return false; /* collection not changed - nothing to remove */
				{
					boolean changed = false;
					for (Object o : c) {
						E e = (E)o;
						if (this.remove(e))
							changed = true;
					}
					return changed;
				}
			}
			@Override public boolean retainAll(Collection<?> c) {
				if (c.isEmpty()) {
					if (this.isEmpty())
						return false; /* collection was not changed - it's already empty */
					acc.clear(root);
					return true;
				}
				{
					boolean changed = false;
					ListIterator<E> i = PtreeNodeAccessorImpl.this.iterator(acc, root);
					while (i.hasNext()) {
						E e = i.next();
						if (!c.contains(e)) {
							i.remove();
							changed = true;
						}
					}
					return changed;
				}
			}
			@Override public void clear() {
				acc.clear(root);
			}
		}

		@Override public <R> Collection<E> collection(BtreeRootAccessor<R,E> acc, R root) {
			return new PtreeCollection<R>(acc, root);
		}
	}

	public abstract static class PtreeModifierImpl<R,E>
		extends PtreeNodeAccessorImpl<E>
		implements PtreeModifier<R,E>
	{
		//need to implement:
		//public E root(R root);
		//public E left(E current);
		//public E right(E current);
		//public E parent(E current);
		//public void setRoot(R root, E tree);
		//public void setLeft(E current, E n);
		//public void setRight(E current, E n);
		//public void setParent(E current, E n);
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
		@Override public <K> E insertObject(R root, E e, K key,
			BtreeKeyExtractorObject<? super E,K> ext, boolean unique, boolean allow_dup)
		{
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
}
