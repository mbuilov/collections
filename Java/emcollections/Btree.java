/**********************************************************************************
* Embedded binary tree
* Copyright (C) 2012-2017 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

package emcollections;

import java.util.Collection;

/* Embedded binary tree:
  one object may encapsulate multiple tree nodes - to reference it from multiple trees */
public class Btree {

	/* parameterized tree node comparator
	  - compare node key with given one */
	public static interface BtreeComparator<E,K> {
		/* returns (node - key):
		  node.a != key.a ? node.a - key.a :
		  node.b != key.b ? node.b - key.b :
		                    node.c - key.c */
		public int compare(E node, K key);
	}

	/* extract key from node */
	public static interface BtreeKeyExtractorInt<E>      {public int  key(E node);}
	public static interface BtreeKeyExtractorLong<E>     {public long key(E node);}
	public static interface BtreeKeyExtractorObject<E,K> {public K    key(E node);}

	/* parameterized tree node processor,
	  - data needed for processing is passed as parameter */
	public static interface BtreeCallback<E,D> {
		public boolean process(E node, D data);
	}

	/* accessor to node 'left' and 'right' fields - references to other nodes in the tree */
	public static interface BtreeNodeReadAccessor<E> {

		public E left(E current);            /* returns null for the leftmost node */
		public E right(E current);           /* returns null for the rightmost node */
		public E _first(E tree);             /* get leftmost node of the tree */
		public E _last(E tree);              /* get rightmost node of the tree */
		public E treeFirst(E tree/*null?*/); /* get leftmost node of the tree */
		public E treeLast(E tree/*null?*/);  /* get rightmost node of the tree */

		/* count nodes in the tree */
		/* NOTE: returns Integer.MAX_VALUE if nodes count >= Integer.MAX_VALUE */
		public int treeSize(E tree/*null?*/);

		/* recursively determine tree height */
		public int treeHeight(E tree/*null?*/);

		/* check if node is in the tree */
		public boolean treeContains(E tree/*null?*/, E n);

		/* search nodes in the tree */
		public boolean treeContainsAll(E tree/*null?*/, Collection<?> c);

		/* fill nodes array preserving ordering */
		/* NOTE: cannot fill more than Integer.MAX_VALUE elements */
		public <T> T[] treeToArray(E tree/*null?*/, T[] a);

		/* fill nodes array preserving ordering */
		/* NOTE: cannot fill more than Integer.MAX_VALUE elements */
		public Object[] treeToArray(E tree/*null?*/);

		/* ===== recursive tree walking/searching ====== */

		/* walk the tree recursively,
		  walking stops if callback returns true for processed node
		  - that node is returned as result of walking */
		public <D> E treeWalkRecursive(E tree/*null?*/, D data, BtreeCallback<? super E,D> cb);

		/* walk the tree recursively forward (from first to last) or backward (from last to first) */
		public <D> E treeWalkRecursiveForward(E tree/*null?*/, D data, BtreeCallback<? super E,D> cb);
		public <D> E treeWalkRecursiveBackward(E tree/*null?*/, D data, BtreeCallback<? super E,D> cb);

		/* search node with given key in unordered tree */
		public E treeSearchRecursiveInt(E tree/*null?*/, int key, BtreeKeyExtractorInt<? super E> ext);
		public E treeSearchRecursiveLong(E tree/*null?*/, long key, BtreeKeyExtractorLong<? super E> ext);
		public <K> E treeSearchRecursiveObject(E tree/*null?*/, K key, BtreeKeyExtractorObject<? super E,K> ext);

		/* ===== fast search in ordered tree ====== */

		/* search node with given key in the ordered tree */
		public <K> E treeSearch(E tree/*null?*/, K key, BtreeComparator<? super E,K> cmp);
		public E treeSearchInt(E tree/*null?*/, int key, BtreeKeyExtractorInt<? super E> ext);
		public E treeSearchLong(E tree/*null?*/, long key, BtreeKeyExtractorLong<? super E> ext);

		/* search node with given key in the tree ordered by non-unique key idenitity hashes */
		public <K> E treeSearchObject(E tree/*null?*/, K key, BtreeKeyExtractorObject<? super E,K> ext);

		/* ===== walk over bounded same keys sub-tree of ordered tree with non-unique keys ====== */

		/* walk over same key sub-tree,
		  walking stops if processor callback returns true for processed node
		  - that node is returned as result of walking */
		/* tree - result of previous treeSearch() */
		public <K,D> E walkSubRecursive(E tree, K key, D data, BtreeComparator<? super E,K> cmp, BtreeCallback<? super E,D> cb);
		public <K,D> E walkSubRecursiveForward(E tree, K key, D data, BtreeComparator<? super E,K> cmp, BtreeCallback<? super E,D> cb);
		public <K,D> E walkSubRecursiveBackward(E tree, K key, D data, BtreeComparator<? super E,K> cmp, BtreeCallback<? super E,D> cb);

		public <D> E walkSubRecursiveInt(E tree,
			int key, D data, BtreeKeyExtractorInt<? super E> ext, BtreeCallback<? super E,D> cb);
		public <D> E walkSubRecursiveLong(E tree,
			long key, D data, BtreeKeyExtractorLong<? super E> ext, BtreeCallback<? super E,D> cb);
		public <K,D> E walkSubRecursiveObject(E tree,
			K key, D data, BtreeKeyExtractorObject<? super E,K> ext, BtreeCallback<? super E,D> cb);

		/* ===== search parent node (for insert) ====== */

		/* search leaf parent of to be inserted node in the tree ordered by abstract keys,
		  initially parent references the root of the tree,
		  returns:
		 < 0 - if parent at left,
		 > 0 - if parent at right,
		   0 - if parent references node with the same key and leaf is false,
		  NOTE: if tree allows nodes with non-unique keys, leaf must be true */
		public <K> int searchParent(BtreeParent<E> parent/*in,out*/, K key, BtreeComparator<? super E,K> cmp, boolean leaf);
		public int searchParentInt(BtreeParent<E> parent/*in,out*/, int key, BtreeKeyExtractorInt<? super E> ext, boolean leaf);
		public long searchParentLong(BtreeParent<E> parent/*in,out*/, long key, BtreeKeyExtractorLong<? super E> ext, boolean leaf);

		/* search node with given key in the tree ordered by non-unique key idenitity hashes */
		/* unique - true if inserting node with unique key (so don't try to search another node with the same key) */
		public <K> int searchParentObject(BtreeParent<E> parent/*in,out*/,
			K key, BtreeKeyExtractorObject<? super E,K> ext, boolean unique, boolean leaf);
	}

	public static interface BtreeNodeAccessor<E> extends BtreeNodeReadAccessor<E> {
		public void setLeft(E current, E n);
		public void setRight(E current, E n);
	}

	/* *************** implementation ************** */

	public static class BtreeParent<E> {
		public E p;
		public BtreeParent(E _p) {
			p = _p;
		}
	}

	/* accessor to node 'left' and 'right' fields - references to other nodes in the tree */
	public abstract static class BtreeNodeReadAccessorImpl<E> implements BtreeNodeReadAccessor<E> {

		//need to implement:
		//public E left(E current);
		//public E right(E current);

		/* get leftmost node of the tree */
		@Override public E _first(E tree) {
			for (;;) {
				E _left = left(tree);
				if (null == _left)
					return tree;
				tree = _left;
			}
		}

		/* get rightmost node of the tree */
		@Override public E _last(E tree) {
			for (;;) {
				E _right = right(tree);
				if (null == _right)
					return tree;
				tree = _right;
			}
		}

		/* get leftmost node of the tree */
		@Override public E treeFirst(E tree/*null?*/) {
			return tree == null ? null : _first(tree);
		}

		/* get rightmost node of the tree */
		@Override public E treeLast(E tree/*null?*/) {
			return tree == null ? null : _last(tree);
		}

		private int _size(E tree/*null?*/, int s) {
			while (null != tree) {
				if (s == Integer.MAX_VALUE)
					return Integer.MAX_VALUE;
				s = _size(left(tree), s + 1);
				tree = right(tree);
			}
			return s;
		}

		/* recursively count nodes in the tree */
		/* NOTE: returns Integer.MAX_VALUE if nodes count >= Integer.MAX_VALUE */
		@Override public int treeSize(E tree/*null?*/) {
			return _size(tree, 0);
		}

		/* recursively determine tree height */
		@Override public int treeHeight(E tree/*null?*/) {
			if (null == tree)
				return 0;
			{
				int left_height = treeHeight(left(tree));
				int right_height = treeHeight(right(tree));
				return 1 + (left_height > right_height ? left_height : right_height);
			}
		}

		/* recursively check if node is in the tree */
		@Override public boolean treeContains(E tree/*null?*/, final E n) {
			for (;;) {
				if (tree == n)
					return true;
				if (null == tree)
					return false;
				if (treeContains(left(tree), n))
					return true;
				tree = right(tree);
			}
		}

		/* recursively search nodes in the tree */
		@SuppressWarnings("unchecked")
		@Override public boolean treeContainsAll(E tree/*null?*/, Collection<?> c) {
			for (Object e : c) {
				if (!treeContains(tree, (E)e))
					return false;
			}
			return true;
		}

		private int _fill_array_recursive(E tree/*null?*/, Object[] arr, int at) {
			while (null != tree) {
				at = _fill_array_recursive(left(tree), arr, at);
				arr[at++] = tree;
				tree = right(tree);
			}
			return at;
		}

		/* recursively fill nodes array preserving ordering */
		/* NOTE: cannot fill more than Integer.MAX_VALUE elements */
		@SuppressWarnings("unchecked")
		@Override public <T> T[] treeToArray(E tree/*null?*/, T[] a) {
			T[] arr = (T[])java.lang.reflect.Array.newInstance(a.getClass().getComponentType(), treeSize(tree));
			_fill_array_recursive(tree, arr, 0);
			return arr;
		}

		/* recursively fill nodes array preserving ordering */
		/* NOTE: cannot fill more than Integer.MAX_VALUE elements */
		@Override public Object[] treeToArray(E tree/*null?*/) {
			Object[] arr = new Object[treeSize(tree)];
			_fill_array_recursive(tree, arr, 0);
			return arr;
		}

		/* ===== recursive tree walking/searching ====== */

		/* walk the tree recursively,
		  walking stops if callback returns true for processed node
		  - that node is returned as result of walking */
		@Override public <D> E treeWalkRecursive(E tree/*null?*/, final D data, final BtreeCallback<? super E,D> cb) {
			for (; null != tree && !cb.process(tree, data); tree = right(tree)) {
				E n = treeWalkRecursive(left(tree), data, cb);
				if (null != n)
					return n;
			}
			return tree;
		}

		/* walk the tree recursively forward - from first to last */
		@Override public <D> E treeWalkRecursiveForward(E tree/*null?*/, final D data, final BtreeCallback<? super E,D> cb) {
			for (; null != tree; tree = right(tree)) {
				E n = treeWalkRecursiveForward(left(tree), data, cb);
				if (null != n)
					return n;
				if (cb.process(tree, data))
					break;
			}
			return tree;
		}

		/* walk the tree recursively backward - from last to first */
		@Override public <D> E treeWalkRecursiveBackward(E tree/*null?*/, final D data, final BtreeCallback<? super E,D> cb) {
			for (; null != tree; tree = left(tree)) {
				E n = treeWalkRecursiveBackward(right(tree), data, cb);
				if (null != n)
					return n;
				if (cb.process(tree, data))
					break;
			}
			return tree;
		}

		/* search node with given key in unordered tree */
		@Override public E treeSearchRecursiveInt(E tree/*null?*/, final int key, final BtreeKeyExtractorInt<? super E> ext) {
			for (; null != tree && ext.key(tree) != key; tree = right(tree)) {
				E _left = treeSearchRecursiveInt(left(tree), key, ext);
				if (null != _left)
					return _left;
			}
			return tree;
		}

		/* search node with given key in unordered tree */
		@Override public E treeSearchRecursiveLong(E tree/*null?*/, final long key, final BtreeKeyExtractorLong<? super E> ext) {
			for (; null != tree && ext.key(tree) != key; tree = right(tree)) {
				E _left = treeSearchRecursiveLong(left(tree), key, ext);
				if (null != _left)
					return _left;
			}
			return tree;
		}

		/* search node with given key in unordered tree */
		@Override public <K> E treeSearchRecursiveObject(E tree/*null?*/, final K key, final BtreeKeyExtractorObject<? super E,K> ext) {
			for (; null != tree && ext.key(tree) != key; tree = right(tree)) {
				E _left = treeSearchRecursiveObject(left(tree), key, ext);
				if (null != _left)
					return _left;
			}
			return tree;
		}

		/* ===== fast search in ordered tree ====== */

		/* search node with given key in the tree ordered by abstract keys */
		@Override public <K> E treeSearch(E tree/*null?*/, final K key, final BtreeComparator<? super E,K> cmp) {
			while (null != tree) {
				int c = cmp.compare(tree, key); /* c = tree - key */
				if (c < 0)
					tree = right(tree);
				else if (c > 0)
					tree = left(tree);
				else
					break;
			}
			return tree;
		}

		/* search node with given key in the tree ordered by int keys */
		@Override public E treeSearchInt(E tree/*null?*/, final int key, final BtreeKeyExtractorInt<? super E> ext) {
			while (null != tree) {
				int c = ext.key(tree) - key;
				if (c < 0)
					tree = right(tree);
				else if (c > 0)
					tree = left(tree);
				else
					break;
			}
			return tree;
		}

		/* search node with given key in the tree ordered by long keys */
		@Override public E treeSearchLong(E tree/*null?*/, final long key, final BtreeKeyExtractorLong<? super E> ext) {
			while (null != tree) {
				long c = ext.key(tree) - key;
				if (c < 0)
					tree = right(tree);
				else if (c > 0)
					tree = left(tree);
				else
					break;
			}
			return tree;
		}

		private <K> E searchSubRecursiveObjectLeft(E tree,
			final K key, final int hash, final BtreeKeyExtractorObject<? super E,K> ext)
		{
			for (;;) {
				tree = left(tree);
				for (;;) {
					if (null == tree)
						return null;
					{
						K k = ext.key(tree);
						if (System.identityHashCode(k) != hash)
							tree = right(tree);
						else if (k == key)
							return tree;
						else {
							E n = treeSearchRecursiveObject(right(tree), key, ext);
							if (null != n)
								return n;
							break;
						}
					}
				}
			}
		}

		private <K> E searchSubRecursiveObjectRight(E tree,
			final K key, final int hash, final BtreeKeyExtractorObject<? super E,K> ext)
		{
			for (;;) {
				tree = right(tree);
				for (;;) {
					if (null == tree)
						return null;
					{
						K k = ext.key(tree);
						if (System.identityHashCode(k) != hash)
							tree = left(tree);
						else if (k == key)
							return tree;
						else {
							E n = treeSearchRecursiveObject(left(tree), key, ext);
							if (null != n)
								return n;
							break;
						}
					}
				}
			}
		}

		/* search node with given key in the tree ordered by non-unique key idenitity hashes */
		@Override public <K> E treeSearchObject(E tree/*null?*/, final K key, final BtreeKeyExtractorObject<? super E,K> ext) {
			final int hash = System.identityHashCode(key);
			while (null != tree) {
				K k = ext.key(tree);
				int c = System.identityHashCode(k) - hash;
				if (c < 0)
					tree = right(tree);
				else if (c > 0)
					tree = left(tree);
				else if (k == key)
					return tree;
				else {
					E n = searchSubRecursiveObjectLeft(tree, key, hash, ext);
					if (null != n)
						return n;
					return searchSubRecursiveObjectRight(tree, key, hash, ext);
				}
			}
			return null;
		}

		/* ===== walk over bounded same keys sub-tree of ordered tree with non-unique keys ====== */

		private <K,D> E walkSubRecursiveLeft(E tree, final K key, final D data,
			final BtreeComparator<? super E,K> cmp, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = left(tree);; tree = right(tree)) {
					if (null == tree)
						return null;
					if (cmp.compare(tree, key) == 0)
						break;
				}
				if (cb.process(tree, data))
					return tree;
				{
					E n = treeWalkRecursive(right(tree), data, cb);
					if (null != n)
						return n;
				}
			}
		}

		private <K,D> E walkSubRecursiveRight(E tree, final K key, final D data,
			final BtreeComparator<? super E,K> cmp, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = right(tree);; tree = left(tree)) {
					if (null == tree)
						return null;
					if (cmp.compare(tree, key) == 0)
						break;
				}
				if (cb.process(tree, data))
					return tree;
				{
					E n = treeWalkRecursive(left(tree), data, cb);
					if (null != n)
						return n;
				}
			}
		}

		/* walk over same key sub-tree,
		  walking stops if processor callback returns true for processed node
		  - that node is returned as result of walking */
		/* tree - result of previous treeSearch() */
		@Override public <K,D> E walkSubRecursive(E tree, final K key, final D data,
			final BtreeComparator<? super E,K> cmp, final BtreeCallback<? super E,D> cb)
		{
			if (cb.process(tree, data))
				return tree;
			{
				E n = walkSubRecursiveLeft(tree, key, data, cmp, cb);
				if (null != n)
					return n;
			}
			return walkSubRecursiveRight(tree, key, data, cmp, cb);
		}

		private <K,D> E walkSubRecursiveForwardLeft(E tree, final K key, final D data,
			final BtreeComparator<? super E,K> cmp, final BtreeCallback<? super E,D> cb)
		{
			for (tree = left(tree);; tree = right(tree)) {
				if (null == tree)
					return null;
				if (cmp.compare(tree, key) == 0)
					break;
			}
			{
				E n = walkSubRecursiveForwardLeft(tree, key, data, cmp, cb);
				if (null != n)
					return n;
			}
			if (cb.process(tree, data))
				return tree;
			return treeWalkRecursiveForward(right(tree), data, cb);
		}

		private <K,D> E walkSubRecursiveForwardRight(E tree, final K key, final D data,
			final BtreeComparator<? super E,K> cmp, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = right(tree);; tree = left(tree)) {
					if (null == tree)
						return null;
					if (cmp.compare(tree, key) == 0)
						break;
				}
				{
					E n = treeWalkRecursiveForward(left(tree), data, cb);
					if (null != n)
						return n;
				}
				if (cb.process(tree, data))
					return tree;
			}
		}

		@Override public <K,D> E walkSubRecursiveForward(E tree, final K key, final D data,
			final BtreeComparator<? super E,K> cmp, final BtreeCallback<? super E,D> cb)
		{
			{
				E n = walkSubRecursiveForwardLeft(tree, key, data, cmp, cb);
				if (null != n)
					return n;
			}
			if (cb.process(tree, data))
				return tree;
			return walkSubRecursiveForwardRight(tree, key, data, cmp, cb);
		}

		private <K,D> E walkSubRecursiveBackwardRight(E tree, final K key, final D data,
			final BtreeComparator<? super E,K> cmp, final BtreeCallback<? super E,D> cb)
		{
			for (tree = right(tree);; tree = left(tree)) {
				if (null == tree)
					return null;
				if (cmp.compare(tree, key) == 0)
					break;
			}
			{
				E n = walkSubRecursiveBackwardRight(tree, key, data, cmp, cb);
				if (null != n)
					return n;
			}
			if (cb.process(tree, data))
				return tree;
			return treeWalkRecursiveBackward(left(tree), data, cb);
		}

		private <K,D> E walkSubRecursiveBackwardLeft(E tree, final K key, final D data,
			final BtreeComparator<? super E,K> cmp, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = left(tree);; tree = right(tree)) {
					if (null == tree)
						return null;
					if (cmp.compare(tree, key) == 0)
						break;
				}
				{
					E n = treeWalkRecursiveBackward(right(tree), data, cb);
					if (null != n)
						return n;
				}
				if (cb.process(tree, data))
					return tree;
			}
		}

		@Override public <K,D> E walkSubRecursiveBackward(E tree, final K key, final D data,
			final BtreeComparator<? super E,K> cmp, final BtreeCallback<? super E,D> cb)
		{
			{
				E n = walkSubRecursiveBackwardRight(tree, key, data, cmp, cb);
				if (null != n)
					return n;
			}
			if (cb.process(tree, data))
				return tree;
			return walkSubRecursiveBackwardLeft(tree, key, data, cmp, cb);
		}

		private <D> E walkSubRecursiveLeftInt(E tree, final int key, final D data,
			final BtreeKeyExtractorInt<? super E> ext, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = left(tree);; tree = right(tree)) {
					if (null == tree)
						return null;
					if (key == ext.key(tree))
						break;
				}
				if (cb.process(tree, data))
					return tree;
				{
					E n = treeWalkRecursive(right(tree), data, cb);
					if (null != n)
						return n;
				}
			}
		}

		private <D> E walkSubRecursiveRightInt(E tree, final int key, final D data,
			final BtreeKeyExtractorInt<? super E> ext, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = right(tree);; tree = left(tree)) {
					if (null == tree)
						return null;
					if (key == ext.key(tree))
						break;
				}
				if (cb.process(tree, data))
					return tree;
				{
					E n = treeWalkRecursive(left(tree), data, cb);
					if (null != n)
						return n;
				}
			}
		}

		/* walk over same key sub-tree */
		@Override public <D> E walkSubRecursiveInt(E tree, final int key, final D data,
			final BtreeKeyExtractorInt<? super E> ext, final BtreeCallback<? super E,D> cb)
		{
			if (cb.process(tree, data))
				return tree;
			{
				E n = walkSubRecursiveLeftInt(tree, key, data, ext, cb);
				if (null != n)
					return n;
			}
			return walkSubRecursiveRightInt(tree, key, data, ext, cb);
		}

		private <D> E walkSubRecursiveLeftLong(E tree, final long key, final D data,
			final BtreeKeyExtractorLong<? super E> ext, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = left(tree);; tree = right(tree)) {
					if (null == tree)
						return null;
					if (key == ext.key(tree))
						break;
				}
				if (cb.process(tree, data))
					return tree;
				{
					E n = treeWalkRecursive(right(tree), data, cb);
					if (null != n)
						return n;
				}
			}
		}

		private <D> E walkSubRecursiveRightLong(E tree, final long key, final D data,
			final BtreeKeyExtractorLong<? super E> ext, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = right(tree);; tree = left(tree)) {
					if (null == tree)
						return null;
					if (key == ext.key(tree))
						break;
				}
				if (cb.process(tree, data))
					return tree;
				{
					E n = treeWalkRecursive(left(tree), data, cb);
					if (null != n)
						return n;
				}
			}
		}

		/* walk over same long keys sub-tree */
		@Override public <D> E walkSubRecursiveLong(E tree, final long key, final D data,
			final BtreeKeyExtractorLong<? super E> ext, final BtreeCallback<? super E,D> cb)
		{
			if (cb.process(tree, data))
				return tree;
			{
				E n = walkSubRecursiveLeftLong(tree, key, data, ext, cb);
				if (null != n)
					return n;
			}
			return walkSubRecursiveRightLong(tree, key, data, ext, cb);
		}

		private <K,D> E walkSubRecursiveLeftObject(E tree, final K key, final D data,
			final BtreeKeyExtractorObject<? super E,K> ext, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = left(tree);; tree = right(tree)) {
					if (null == tree)
						return null;
					if (key == ext.key(tree))
						break;
				}
				if (cb.process(tree, data))
					return tree;
				{
					E n = treeWalkRecursive(right(tree), data, cb);
					if (null != n)
						return n;
				}
			}
		}

		private <K,D> E walkSubRecursiveRightObject(E tree, final K key, final D data,
			final BtreeKeyExtractorObject<? super E,K> ext, final BtreeCallback<? super E,D> cb)
		{
			for (;;) {
				for (tree = right(tree);; tree = left(tree)) {
					if (null == tree)
						return null;
					if (key == ext.key(tree))
						break;
				}
				if (cb.process(tree, data))
					return tree;
				{
					E n = treeWalkRecursive(left(tree), data, cb);
					if (null != n)
						return n;
				}
			}
		}

		/* walk over same Object keys sub-tree */
		@Override public <K,D> E walkSubRecursiveObject(E tree, final K key, final D data,
			final BtreeKeyExtractorObject<? super E,K> ext, final BtreeCallback<? super E,D> cb)
		{
			if (cb.process(tree, data))
				return tree;
			{
				E n = walkSubRecursiveLeftObject(tree, key, data, ext, cb);
				if (null != n)
					return n;
			}
			return walkSubRecursiveRightObject(tree, key, data, ext, cb);
		}

		/* ===== search parent node (for insert) ====== */

		/* search leaf parent of to be inserted node in the tree ordered by abstract keys,
		  initially parent references the root of the tree,
		  returns:
		 < 0 - if parent at left,
		 > 0 - if parent at right,
		   0 - if parent references node with the same key and leaf is false,
		  NOTE: if tree allows nodes with non-unique keys, leaf must be true */
		@Override public <K> int searchParent(final BtreeParent<E> parent/*in,out*/,
			final K key, final BtreeComparator<? super E,K> cmp, final boolean leaf)
		{
			E next = parent.p;
			if (null == next)
				return 1; /* tree is empty, parent is null */
			for (;;) {
				E p = next;
				int c = cmp.compare(p, key); /* c = p - key */
				if (c < 0) {
					next = right(next);
					if (null != next)
						continue;
				}
				else if (c > 0) {
					next = left(next);
					if (null != next)
						continue;
				}
				else if (leaf) {
					if (null == right(p))
						c = -1; /* parent at left */
					else {
						next = left(p);
						if (null == next)
							c = 1; /* parent at right */
						else {
							p = _last(next);
							c = -1;  /* parent at left */
						}
					}
				}
				parent.p = p;
				return c; /* if 0, then parent.p - references found node, else parent.p - references leaf parent */
			}
		}

		/* search leaf parent of to be inserted node in the tree ordered by int keys */
		@Override public int searchParentInt(final BtreeParent<E> parent/*in,out*/,
			final int key, final BtreeKeyExtractorInt<? super E> ext, final boolean leaf)
		{
			E next = parent.p;
			if (null == next)
				return 1; /* tree is empty, parent is null */
			for (;;) {
				E p = next;
				int c = ext.key(p) - key;
				if (c < 0) {
					next = right(next);
					if (null != next)
						continue;
				}
				else if (c > 0) {
					next = left(next);
					if (null != next)
						continue;
				}
				else if (leaf) {
					if (null == right(p))
						c = -1; /* parent at left */
					else {
						next = left(p);
						if (null == next)
							c = 1; /* parent at right */
						else {
							p = _last(next);
							c = -1;  /* parent at left */
						}
					}
				}
				parent.p = p;
				return c; /* if 0, then parent.p - references found node, else parent.p - references leaf parent */
			}
		}

		/* search leaf parent of to be inserted node in the tree ordered by long keys */
		@Override public long searchParentLong(final BtreeParent<E> parent/*in,out*/,
			final long key, final BtreeKeyExtractorLong<? super E> ext, final boolean leaf)
		{
			E next = parent.p;
			if (null == next)
				return 1; /* tree is empty, parent is null */
			for (;;) {
				E p = next;
				long c = ext.key(p) - key;
				if (c < 0) {
					next = right(next);
					if (null != next)
						continue;
				}
				else if (c > 0) {
					next = left(next);
					if (null != next)
						continue;
				}
				else if (leaf) {
					if (null == right(p))
						c = -1; /* parent at left */
					else {
						next = left(p);
						if (null == next)
							c = 1; /* parent at right */
						else {
							p = _last(next);
							c = -1;  /* parent at left */
						}
					}
				}
				parent.p = p;
				return c; /* if 0, then parent.p - references found node, else parent.p - references leaf parent */
			}
		}

		/* note: after return parent.p - some lowest (may be non-leaf) node with key 'key' */
		private <K> int searchSubRecursiveParentObjectLeft(E p, final BtreeParent<E> parent,
			final K key, final int hash, final BtreeKeyExtractorObject<? super E,K> ext)
		{
			for (;;) {
				E next = left(p);
				if (null == next) {
					parent.p = p;
					return 1; /* parent at right */
				}
				for (;;) {
					p = next;
					{
						K k = ext.key(p);
						if (System.identityHashCode(k) != hash) {
							next = right(p);
							if (null == next) {
								parent.p = p;
								return -1; /* parent at left */
							}
						}
						else if (k == key) {
							parent.p = p;
							return 0;
						}
						else {
							next = treeSearchRecursiveObject(right(p), key, ext);
							if (null != next) {
								parent.p = next;
								return 0;
							}
							break;
						}
					}
				}
			}
		}

		/* search node with given key in the tree ordered by non-unique key idenitity hashes */
		/* unique - true if inserting node with unique key (so don't try to search another node with the same key) */
		@Override public <K> int searchParentObject(final BtreeParent<E> parent/*in,out*/,
			final K key, final BtreeKeyExtractorObject<? super E,K> ext, final boolean unique, final boolean leaf)
		{
			E next = parent.p;
			if (null == next)
				return 1; /* tree is empty, parent is null */
			{
				final int hash = System.identityHashCode(key);
				for (;;) {
					E p = next;
					K k = ext.key(p);
					int c = System.identityHashCode(k) - hash;
					if (c < 0) {
						next = right(next);
						if (null != next)
							continue;
					}
					else if (c > 0) {
						next = left(next);
						if (null != next)
							continue;
					}
					else if (!unique) {
						if (k != key) {
							next = searchSubRecursiveObjectRight(p, key, hash, ext);
							if (null != next)
								p = next;
							else {
								c = searchSubRecursiveParentObjectLeft(p, parent, key, hash, ext);
								if (c != 0 || !leaf)
									return c; /* if != 0, parent.p - references leaf parent */
								p = parent.p;
							}
						}
						/* p - references node with given key */
						if (!leaf) {
							parent.p = p;
							return 0;
						}
					}
					if (c == 0) {
						if (null == right(p))
							c = -1; /* parent at left */
						else {
							next = left(p);
							if (null == next)
								c = 1; /* parent at right */
							else {
								p = _last(next);
								c = -1;  /* parent at left */
							}
						}
					}
					parent.p = p;
					return c; /* != 0, parent.p - references leaf parent */
				}
			}
		}
	}

	public abstract static class BtreeSearcher<E,K>
		extends BtreeNodeReadAccessorImpl<E>
		implements BtreeComparator<E,K>, BtreeCallback<E,K>
	{
		//need to implement:
		//public E left(E current);
		//public E right(E current);
		//public int compare(E node, K key);
		@Override public boolean process(E node, K key) {
			return 0 == compare(node, key);
		}
		public E search(E tree/*null?*/, K key) {
			return treeSearch(tree, key, this);
		}
		public E searchRecursive(E tree/*null?*/, K key) {
			return treeWalkRecursive(tree, key, this);
		}
		public int searchParent(BtreeParent<E> parent/*in,out*/, K key, boolean leaf) {
			return searchParent(parent, key, this, leaf);
		}
		public <D> E walkSubRecursive(E tree, K key, D data, BtreeCallback<E,D> cb) {
			return walkSubRecursive(tree, key, data, this, cb);
		}
	}

	public abstract static class BtreeSearcherInt<E>
		extends BtreeNodeReadAccessorImpl<E>
		implements BtreeKeyExtractorInt<E>
	{
		//need to implement:
		//public E left(E current);
		//public E right(E current);
		//public int key(E node);
		public E search(E tree/*null?*/, int key) {
			return treeSearchInt(tree, key, this);
		}
		public E searchRecursive(E tree/*null?*/, int key) {
			return treeSearchRecursiveInt(tree, key, this);
		}
		public int searchParent(BtreeParent<E> parent/*in,out*/, int key, boolean leaf) {
			return searchParentInt(parent, key, this, leaf);
		}
		public <D> E walkSubRecursive(E tree, int key, D data, BtreeCallback<E,D> cb) {
			return walkSubRecursiveInt(tree, key, data, this, cb);
		}
	}

	public abstract static class BtreeSearcherLong<E>
		extends BtreeNodeReadAccessorImpl<E>
		implements BtreeKeyExtractorLong<E>
	{
		//need to implement:
		//public E left(E current);
		//public E right(E current);
		//public long key(E node);
		public E search(E tree/*null?*/, long key) {
			return treeSearchLong(tree, key, this);
		}
		public E searchRecursive(E tree/*null?*/, long key) {
			return treeSearchRecursiveLong(tree, key, this);
		}
		public long searchParent(BtreeParent<E> parent/*in,out*/, long key, boolean leaf) {
			return searchParentLong(parent, key, this, leaf);
		}
		public <D> E walkSubRecursive(E tree, long key, D data, BtreeCallback<E,D> cb) {
			return walkSubRecursiveLong(tree, key, data, this, cb);
		}
	}

	public abstract static class BtreeSearcherObject<E,K>
		extends BtreeNodeReadAccessorImpl<E>
		implements BtreeKeyExtractorObject<E,K>
	{
		//need to implement:
		//public E left(E current);
		//public E right(E current);
		//public Object key(E node);
		public E search(E tree/*null?*/, K key) {
			return treeSearchObject(tree, key, this);
		}
		public E searchRecursive(E tree/*null?*/, K key) {
			return treeSearchRecursiveObject(tree, key, this);
		}
		public int searchParent(BtreeParent<E> parent/*in,out*/, K key, boolean check_dup, boolean leaf) {
			return searchParentObject(parent, key, this, check_dup, leaf);
		}
		public <D> E walkSubRecursive(E tree, K key, D data, BtreeCallback<E,D> cb) {
			return walkSubRecursiveObject(tree, key, data, this, cb);
		}
	}

// example:
/*
	public static class Node {
		public Node a_left;
		public Node a_right;
		public Node b_left;
		public Node b_right;
		public int a_key;
		public long b_key;

		public static final BtreeNodeReadAccessor<Node> a_accessor = new BtreeNodeReadAccessorImpl<Node>() {
			@Override public Node left(Node c) {return c.a_left;}
			@Override public Node right(Node c) {return c.a_right;}
		};
		public static final BtreeNodeReadAccessor<Node> b_accessor = new BtreeNodeReadAccessorImpl<Node>() {
			@Override public Node left(Node c) {return c.b_left;}
			@Override public Node right(Node c) {return c.b_right;}
		};
		public static final BtreeSearcherInt<Node> a_searcher = new BtreeSearcherInt<Node>() {
			@Override public Node left(Node c) {return c.a_left;}
			@Override public Node right(Node c) {return c.a_right;}
			@Override public int key(Node node) {return node.a_key;}
		};
		public static final BtreeSearcherLong<Node> b_searcher = new BtreeSearcherLong<Node>() {
			@Override public Node left(Node c) {return c.b_left;}
			@Override public Node right(Node c) {return c.b_right;}
			@Override public long key(Node node) {return node.b_key;}
		};
	}

	public static Node search_a(Node n, int a_key) {
		return Node.a_accessor.treeSearchInt(n, a_key, new BtreeKeyExtractorInt<Node>() {
			@Override public int key(Node node) {return node.a_key;}
		});
	}

	public static Node search_b(Node n, long b_key) {
		return Node.b_searcher.search(n, b_key);
	}
*/
}
