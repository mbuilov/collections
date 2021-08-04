/**********************************************************************************
* Embedded binary tree of nodes with parent pointers
* Copyright (C) 2012-2021 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

import emcollections.Btree.*;
import emcollections.Ptree.*;
import java.util.Collection;
import java.util.Iterator;
import java.util.ListIterator;

public class PtreeTests {

	public static interface TestN<T extends TestN<T>> extends TreeTest.TreeNode {
		public T parent();
		@Override public T left();
		@Override public T right();
		public void clear();
	}

	public static interface TestNode extends TestN<TestNode> {
		public void setKey(int k);
	}

	public static interface TestNodeObj extends TestN<TestNodeObj> {
		public Integer okey();
	}

	public abstract static class TestNImpl<I extends TestN<I>, T extends I> implements TestN<I> {
		public T left;
		public T right;
		public T parent;
		@Override public I left() {return left;}
		@Override public I right() {return right;}
		@Override public I parent() {return parent;}
		@Override public void clear() {
			left = null;
			right = null;
			parent = null;
		}
	}

	public static class TestNodeImpl
		extends TestNImpl<TestNode, TestNodeImpl>
		implements TestNode
	{
		public int key;
		@Override public int key() {return key;}
		@Override public void setKey(int k) {key = k;}
		public TestNodeImpl(int k) {
			key = k;
		}
	}

	public static class TestNodeObjImpl
		extends TestNImpl<TestNodeObj, TestNodeObjImpl>
		implements TestNodeObj
	{
		public Integer x;
		@Override public int key() {return x;}
		@Override public Integer okey() {return x;}
		public TestNodeObjImpl(Integer k) {
			x = k;
		}
	}

	public static interface TestNAccessor<T> extends PtreeNodeAccessor<T> {
		public T newNode(Integer key);
	}

	public static interface TestNodeAccessor extends TestNAccessor<TestNode>, BtreeKeyExtractorInt<TestNode> {}
	public static interface TestNodeAccessorObj extends TestNAccessor<TestNodeObj>, BtreeKeyExtractorObject<TestNodeObj,Integer> {}

	public static class TestRoot<T> extends BtreeRootAccessorImpl<TestRoot<T>,T> {
		public T root;
		public T root() {return root;}
		public void setRoot(T tree) {root = tree;}
		@Override public T root(TestRoot<T> root) {return root.root();}
		@Override public void setRoot(TestRoot<T> root, T tree) {root.setRoot(tree);}
		public TestRoot() {
			root = null;
		}
		public TestRoot(T tree) {
			root = tree;
		}
	}

	/*      8
	   4         12
	 2   6    10    14
	1 3 5 7  9 11 13 15  */

	static TestNode build_tree(TestNodeAccessor a) {
		TestRoot<TestNode> r = new TestRoot<TestNode>(a.newNode(8));
		a.insertInt(r, r, a.newNode(4), a);
		a.insertInt(r, r, a.newNode(12), a);
		a.insertInt(r, r, a.newNode(2), a);
		a.insertInt(r, r, a.newNode(6), a);
		a.insertInt(r, r, a.newNode(10), a);
		a.insertInt(r, r, a.newNode(14), a);
		a.insertInt(r, r, a.newNode(1), a);
		a.insertInt(r, r, a.newNode(3), a);
		a.insertInt(r, r, a.newNode(5), a);
		a.insertInt(r, r, a.newNode(7), a);
		a.insertInt(r, r, a.newNode(9), a);
		a.insertInt(r, r, a.newNode(11), a);
		a.insertInt(r, r, a.newNode(13), a);
		a.insertInt(r, r, a.newNode(15), a);
		return r.root();
	}

	static <T extends TestN<T>> boolean check_tree(TestN<T> tree) {
		while (tree != null) {
			{
				TestN<T> l = tree.left();
				if (null != l && (l.parent() != tree || !check_tree(l)))
					return false;
			}
			{
				TestN<T> r = tree.right();
				if (null != r && r.parent() != tree)
					return false;
				tree = r;
			}
		}
		return true;
	}

	static void test_read_iterator(TestNode tree, ListIterator<TestNode> it) {
		if (!it.hasNext())
			TreeTest.err(tree, "bad iterator: first it.hasNext() is false");
		if (it.hasPrevious())
			TreeTest.err(tree, "bad iterator: first it.hasPrevious() is true");
		{
			int i = it.nextIndex();
			if (i != 0)
				TreeTest.err(tree, "bad iterator: first it.nextIndex(): " + i + " != 0");
		}
		{
			int i = it.previousIndex();
			if (i != -1)
				TreeTest.err(tree, "bad iterator: first it.previousIndex(): " + i + " != -1");
		}
		{
			int i = 0;
			while (it.hasNext()) {
				{
					int k = it.nextIndex();
					if (i != k)
						TreeTest.err(tree, "bad iterator: it.nextIndex(): " + i + " != " + k);
				}
				{
					TestNode n = it.next();
					if (null == n || ++i != n.key())
						TreeTest.err(tree, "bad iterator: it.next(): " + i + " != " + (null == n ? "null" : n.key()));
				}
			}
			{
				int k = it.nextIndex();
				if (i != k)
					TreeTest.err(tree, "bad iterator: it.nextIndex() at end: " + i + " != " + k);
			}
		}
		{
			int i = it.nextIndex();
			while (it.hasPrevious()) {
				{
					int k = it.previousIndex();
					if (--i != k)
						TreeTest.err(tree, "bad iterator: it.previousIndex(): " + i + " != " + k);
				}
				{
					TestNode n = it.previous();
					if (null == n || i + 1 != n.key())
						TreeTest.err(tree, "bad iterator: it.previous(): " + (i + 1) + " != " + (null == n ? "null" : n.key()));
				}
			}
			{
				int k = it.previousIndex();
				if (--i != k)
					TreeTest.err(tree, "bad iterator: it.previousIndex() at end: " + i + " != " + k);
			}
		}
	}

	static void test_read_collection(TestNode tree, Collection<TestNode> c) {
		int i = 0;
		for (TestNode n : c) {
			if (++i != n.key())
				TreeTest.err(tree, "bad collection: n.key: " + n.key() + " != " + i);
		}
		{
			int s = c.size();
			if (i != s)
				TreeTest.err(tree, "bad collection: size(): " + s + " != " + i);
		}
	}

	static TestNode test1(final TestNodeAccessor accessor) {
		TestNode tree = build_tree(accessor);
		{
			int i = 1;
			TestNode n = accessor.treeFirst(tree);
			if (null == n || i != n.key())
				TreeTest.err(tree, "bad first: " + i + " != " + (null == n ? "null" : ("(key=" + n.key() + ")")));
			do {
				n = accessor.next(n);
				if (null == n || ++i != n.key())
					TreeTest.err(tree, "bad next: " + i + " != " + (null == n ? "null" : ("(key=" + n.key() + ")")));
			} while (i < 15);
			n = accessor.next(n);
			if (null != n)
				TreeTest.err(tree, "bad next after last: null != " + ("(key=" + n.key() + ")"));
		}
		{
			int i = 15;
			TestNode n = accessor.treeLast(tree);
			if (null == n || i != n.key())
				TreeTest.err(tree, "bad last: " + i + " != " + (null == n ? "null" : ("(key=" + n.key() + ")")));
			do {
				n = accessor.prev(n);
				if (null == n || --i != n.key())
					TreeTest.err(tree, "bad prev: " + i + " != " + (null == n ? "null" : ("(key=" + n.key() + ")")));
			} while (i > 1);
			n = accessor.prev(n);
			if (null != n)
				TreeTest.err(tree, "bad prev after first: null != " + ("(key=" + n.key() + ")"));
		}
		test_read_iterator(tree, accessor.treeReadIterator(tree));
		test_read_collection(tree, accessor.treeReadCollection(tree));
		return tree;
	}

	public static void test(final TestNodeAccessor accessor) {
		final TestRoot<TestNode> root = new TestRoot<TestNode>(test1(accessor));
		{
			TestNode x = accessor.newNode(33);
			{
				TestNode p = accessor.treeSearchInt(root.root(), 5, accessor);
				if (null == p || p.key() != 5)
					TreeTest.err(root.root(), "searchInt(5) failed: found: " + (null == p ? "null" : p.key()));
				accessor.insertAtParent(root, root, p, x, /*r:*/true);
				{
					int i = 0;
					int[] test_arr = new int[] {1,2,3,4,5,33,6,7,8,9,10,11,12,13,14,15};
					for (TestNode n : accessor.treeReadCollection(root.root())) {
						if (i >= test_arr.length || test_arr[i] != n.key()) {
							TreeTest.err(root.root(),
								"insertAtParent() failed: expecting " + test_arr[i] + " at " + i + " instead of " + n.key());
						}
						i++;
					}
					if (i != test_arr.length)
						TreeTest.err(root.root(), "insertAtParent() failed: elements count " + i + " != " + test_arr.length);
				}
			}
			accessor.remove(root, root, x);
			if (!check_tree(root.root()))
				TreeTest.err(root.root(), "bad tree");
			{
				int i = 0;
				int[] test_arr = new int[] {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
				for (TestNode n : accessor.treeReadCollection(root.root())) {
					if (i >= test_arr.length || test_arr[i] != n.key())
						TreeTest.err(root.root(), "remove() failed: expecting " + test_arr[i] + " at " + i + " instead of " + n.key());
					i++;
				}
				if (i != test_arr.length)
					TreeTest.err(root.root(), "remove() failed: elements count " + i + " != " + test_arr.length);
			}
			{
				TestNode a = accessor.treeSearchInt(root.root(), 6, accessor);
				if (null == a || a.key() != 6)
					TreeTest.err(root.root(), "searchInt(6) failed: found: " + (null == a ? "null" : a.key()));
				x.setKey(53);
				accessor.replace(root, root, a, x);
			}
			{
				int i = 0;
				int[] test_arr = new int[] {1,2,3,4,5,53,7,8,9,10,11,12,13,14,15};
				for (TestNode n : accessor.treeReadCollection(root.root())) {
					if (i >= test_arr.length || test_arr[i] != n.key())
						TreeTest.err(root.root(), "replace() failed: expecting " + test_arr[i] + " instead of " + n.key() + " at " + i);
					i++;
				}
				if (i != test_arr.length)
					TreeTest.err(root.root(), "replace() failed: elements count " + i + " != " + test_arr.length);
			}
			x.setKey(6);
		}
		{
			TestNode x = accessor.newNode(2);
			{
				TestNode e = accessor.insertInt(root, root, x, x.key(), accessor, /*allow_dup:*/false);
				if (null == e || e.key() != x.key()) {
					TreeTest.err(root.root(),
						"insertInt(false) failed: found " + (null == e ? "null" : e.key()) + " instead of " + x.key());
				}
			}
			{
				TestNode e = accessor.insertInt(root, root, x, x.key(), accessor, /*allow_dup:*/true);
				if (null != e)
					TreeTest.err(root.root(), "insertInt(" + x.key() + ", true) failed: found " + e.key() + " instead of null");
			}
		}
		{
			TestNode e = accessor.insertInt(root, root, accessor.newNode(2), 2, accessor, /*allow_dup:*/true);
			if (null != e)
				TreeTest.err(root.root(), "insertInt(2, true) failed: found " + e.key() + " instead of null");
		}
		{
			TestNode e = accessor.insertInt(root, root, accessor.newNode(3), 3, accessor, /*allow_dup:*/true);
			if (null != e)
				TreeTest.err(root.root(), "insertInt(3, true) failed: found " + e.key() + " instead of null");
		}
		{
			int i = 0;
			int[] test_arr = new int[] {1,2,2,2,3,3,4,5,6,7,8,9,10,11,12,13,14,15};
			for (TestNode n : accessor.treeReadCollection(root.root())) {
				if (i >= test_arr.length || test_arr[i] != n.key())
					TreeTest.err(root.root(), "insertInt(true) failed: expecting " + test_arr[i] + " at " + i + " instead of " + n.key());
				i++;
			}
			if (i != test_arr.length)
				TreeTest.err(root.root(), "insertInt(true) failed: elements count " + i + " != " + test_arr.length);
		}
		{
			TestNode a = accessor.treeWalkRecursive(root.root(), Integer.valueOf(2), new BtreeCallback<TestNode,Integer>() {
				@Override public boolean process(TestNode node, Integer key) {
					return node.key() == key;
				}
			});
			if (null == a || a.key() != 2)
				TreeTest.err(root.root(), "walkRecursive(2) failed: found: " + (null == a ? "null" : a.key()));
			{
				TestNode b = accessor.walkSubRecursiveInt(a, 2, a, accessor, new BtreeCallback<TestNode,TestNode>() {
					@Override public boolean process(TestNode node, TestNode data) {
						return node != data;
					}
				});
				if (null == b || b.key() != 2 || b == a)
					TreeTest.err(root.root(),
						"walkSubRecursiveInt(2) failed: found: " + (null == b ? "null" : b == a ? "b == a" : b.key()));
				{
					TestNode c = accessor.treeSearchInt(root.root(), 3, accessor);
					if (null == c || c.key() != 3)
						TreeTest.err(root.root(), "searchInt(3) failed: found: " + (null == c ? "null" : c.key()));
					{
						TestNode d = accessor.treeSearchInt(root.root(), 12, accessor);
						if (null == d || d.key() != 12)
							TreeTest.err(root.root(), "searchInt(12) failed: found: " + (null == d ? "null" : d.key()));
						accessor.remove(root, root, a);
						if (!check_tree(root.root()))
							TreeTest.err(root.root(), "bad tree");
						accessor.remove(root, root, d);
						if (!check_tree(root.root()))
							TreeTest.err(root.root(), "bad tree");
						accessor.remove(root, root, b);
						if (!check_tree(root.root()))
							TreeTest.err(root.root(), "bad tree");
						accessor.remove(root, root, c);
						if (!check_tree(root.root()))
							TreeTest.err(root.root(), "bad tree");
					}
				}
			}
		}
		{
			int i = 0;
			int[] test_arr = new int[] {1,2,3,4,5,6,7,8,9,10,11,13,14,15};
			for (TestNode n : accessor.treeReadCollection(root.root())) {
				if (i >= test_arr.length || test_arr[i] != n.key())
					TreeTest.err(root.root(), "remove() failed: expecting " + test_arr[i] + " at " + i + " instead of " + n.key());
				i++;
			}
			if (i != test_arr.length)
				TreeTest.err(root.root(), "remove() failed: elements count " + i + " != " + test_arr.length);
		}
		{
			TestNode n = accessor.insertInt(root, root, accessor.newNode(12), accessor);
			if (null != n)
				TreeTest.err(root.root(), "insertInt(12, false) failed: found " + n.key() + " instead of null");
		}
		{
			final TestRoot<TestNode> dst_root = new TestRoot<TestNode>();
			ListIterator<TestNode> dst = accessor.iterator(dst_root, dst_root);
			ListIterator<TestNode> src = accessor.iterator(root, root);
			test_read_iterator(root.root(), src);
			while (src.hasNext()) {
				TestNode n = src.next();
				src.remove();
				if (!check_tree(root.root()))
					TreeTest.err(root.root(), "bad tree");
				n.clear();
				dst.add(n);
				if (!check_tree(dst_root.root()))
					TreeTest.err(dst_root.root(), "bad tree");
			}
			while (dst.hasPrevious())
				dst.previous();
			test_read_iterator(dst_root.root(), dst);
			if (root.root() != null)
				TreeTest.err(root.root(), "bad all remove, root != null");
			root.setRoot(dst_root.root());
		}
		{
			final TestRoot<TestNode> dst_root = new TestRoot<TestNode>();
			ListIterator<TestNode> dst = accessor.iterator(dst_root, dst_root);
			ListIterator<TestNode> src = accessor.iterator(root, root);
			while (src.hasNext())
				src.next();
			while (src.hasPrevious()) {
				TestNode n = src.previous();
				src.remove();
				if (!check_tree(root.root()))
					TreeTest.err(root.root(), "bad tree");
				n.clear();
				dst.add(n);
				if (!check_tree(dst_root.root()))
					TreeTest.err(dst_root.root(), "bad tree");
			}
			if (root.root() != null)
				TreeTest.err(root.root(), "bad all previous remove, root != null");
			{
				int i = 0;
				int[] test_arr = new int[] {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
				for (TestNode n : accessor.treeReadCollection(dst_root.root())) {
					if (i >= test_arr.length || test_arr[i] != n.key()) {
						TreeTest.err(root.root(),
							"previuos remove() failed: expecting " + test_arr[i] + " at " + i + " instead of " + n.key());
					}
					i++;
				}
				if (i != test_arr.length)
					TreeTest.err(root.root(), "previous remove() failed: elements count " + i + " != " + test_arr.length);
			}
			while (dst.hasPrevious()) {
				TestNode n = dst.previous();
				dst.remove();
				if (!check_tree(dst_root.root()))
					TreeTest.err(dst_root.root(), "bad tree");
				n.clear();
				src.add(n);
				if (!check_tree(root.root()))
					TreeTest.err(root.root(), "bad tree");
			}
			if (dst_root.root() != null)
				TreeTest.err(dst_root.root(), "bad all previous remove, dst_root != null");
			while (src.hasPrevious())
				src.previous();
			test_read_iterator(root.root(), src);
		}
		{
			ListIterator<TestNode> it = accessor.iterator(root, root);
			while (it.hasNext()) {
				it.next();
				it.set(accessor.newNode(-1));
				it.set(accessor.newNode(-2));
			}
			{
				TestNode[] arr = accessor.treeToArray(root.root(), new TestNode[0]);
				if (arr.length != 15)
					TreeTest.err(root.root(), "toArray() failed: " + arr.length + " != 15");
				for (TestNode n : arr) {
					if (n.key() != -2)
						TreeTest.err(root.root(), "iterator.set() failed: found " + n.key() + " instead of -2");
				}
			}
			{
				int i = 15;
				while (it.hasPrevious()) {
					it.previous();
					it.set(accessor.newNode(i--));
				}
			}
			test_read_iterator(root.root(), it);
		}
		{
			TestNode[] arr = new TestNode[4];
			arr[0] = accessor.treeSearchInt(root.root(), 2, accessor);
			arr[1] = accessor.treeSearchInt(root.root(), 11, accessor);
			arr[2] = accessor.treeSearchInt(root.root(), 5, accessor);
			arr[3] = accessor.treeSearchInt(root.root(), 6, accessor);
			if (null == arr[0] || arr[0].key() != 2)
				TreeTest.err(root.root(), "searchInt(2) failed: found: " + (null == arr[0] ? "null" : arr[0].key()));
			if (null == arr[1] || arr[1].key() != 11)
				TreeTest.err(root.root(), "searchInt(11) failed: found: " + (null == arr[0] ? "null" : arr[0].key()));
			if (null == arr[2] || arr[2].key() != 5)
				TreeTest.err(root.root(), "searchInt(5) failed: found: " + (null == arr[0] ? "null" : arr[0].key()));
			if (null == arr[3] || arr[3].key() != 6)
				TreeTest.err(root.root(), "searchInt(6) failed: found: " + (null == arr[0] ? "null" : arr[0].key()));
			{
				Collection<TestNode> c = accessor.collection(root, root);
				c.removeAll(java.util.Arrays.asList(arr));
			}
		}
		{
			int[] check_arr = new int[] {1,3,4,7,8,9,10,12,13,14,15};
			TestNode[] arr = accessor.treeToArray(root.root(), new TestNode[0]);
			if (check_arr.length != arr.length)
				TreeTest.err(root.root(), "removeAll(2,11,5,6) failed: expected size " + check_arr.length + " != " + arr.length);
			for (int i = 0; i < check_arr.length; i++) {
				if (check_arr[i] != arr[i].key()) {
					TreeTest.err(root.root(),
						"removeAll(2,11,5,6) failed: found: " + arr[i].key() + " instead of " + check_arr[i] + " at " + i);
				}
			}
		}
		{
			TestNode[] arr = new TestNode[] {
				accessor.treeSearchInt(root.root(), 1, accessor),
				accessor.treeSearchInt(root.root(), 3, accessor),
				accessor.treeSearchInt(root.root(), 12, accessor)
			};
			if (null == arr[0] || arr[0].key() != 1)
				TreeTest.err(root.root(), "searchInt(1) failed: found: " + (null == arr[0] ? "null" : arr[0].key()));
			if (null == arr[1] || arr[1].key() != 3)
				TreeTest.err(root.root(), "searchInt(3) failed: found: " + (null == arr[0] ? "null" : arr[0].key()));
			if (null == arr[2] || arr[2].key() != 12)
				TreeTest.err(root.root(), "searchInt(12) failed: found: " + (null == arr[0] ? "null" : arr[0].key()));
			{
				Collection<TestNode> c = accessor.collection(root, root);
				c.retainAll(java.util.Arrays.asList(arr));
				if (c.isEmpty())
					TreeTest.err(root.root(), "collection is empty after retainAll()");
				if (c.size() != arr.length)
					TreeTest.err(root.root(), "bad size() = " + c.size() + " after retainAll() != " + arr.length);
				if (!c.containsAll(java.util.Arrays.asList(arr)))
					TreeTest.err(root.root(), "bad contansAll() after retainAll()");
				{
					TestNode[] arr2 = c.toArray(new TestNode[0]);
					if (arr2.length != arr.length)
						TreeTest.err(root.root(), "bad toArray() after retainAll(): length " + arr2.length + " != " + arr.length);
					for (int t = 0; t < arr2.length; t++) {
						if (arr2[t] != arr[t]) {
							TreeTest.err(root.root(),
								"bad toArray() after retainAll(): " + arr2[t].key() + " != " + arr[t].key() + " at " + t);
						}
					}
				}
				{
					Object[] arr2 = c.toArray();
					if (arr2.length != arr.length)
						TreeTest.err(root.root(), "bad toArray() after retainAll(): length " + arr2.length + " != " + arr.length);
					for (int t = 0; t < arr2.length; t++) {
						if (arr2[t] != arr[t]) {
							TreeTest.err(root.root(),
								"bad toArray() after retainAll(): " + ((TestNode)arr2[t]).key() + " != " + arr[t].key() + " at " + t);
						}
					}
				}
				{
					Iterator<TestNode> it = c.iterator();
					if (!it.hasNext())
						TreeTest.err(root.root(), "bad iterator.hasNext(0) == false after retainAll()");
					{
						int k = it.next().key();
						if (k != 1)
							TreeTest.err(root.root(), "bad iterator.next(0) != 1 after retainAll()");
					}
					if (!it.hasNext())
						TreeTest.err(root.root(), "bad iterator.hasNext(1) == false after retainAll()");
					{
						int k = it.next().key();
						if (k != 3)
							TreeTest.err(root.root(), "bad iterator.next(1) != 3 after retainAll()");
					}
					if (!it.hasNext())
						TreeTest.err(root.root(), "bad iterator.hasNext(2) == false after retainAll()");
					{
						int k = it.next().key();
						if (k != 12)
							TreeTest.err(root.root(), "bad iterator.next(2) != 12 after retainAll()");
					}
					if (it.hasNext())
						TreeTest.err(root.root(), "bad iterator.hasNext(3) == true after retainAll()");
					if (((ListIterator<TestNode>)it).nextIndex() != 3)
						TreeTest.err(root.root(), "bad iterator.nextIndex(3) != 3 == true after retainAll()");
				}
			}
		}
		{
			int[] check_arr = new int[] {1,3,12};
			TestNode[] arr = accessor.treeToArray(root.root(), new TestNode[0]);
			if (check_arr.length != arr.length)
				TreeTest.err(root.root(), "retainAll(1,3,12) failed: expected size " + check_arr.length + " != " + arr.length);
			for (int i = 0; i < check_arr.length; i++) {
				if (check_arr[i] != arr[i].key()) {
					TreeTest.err(root.root(),
						"retainAll(1,3,12) failed: found: " + arr[i].key() + " instead of " + check_arr[i] + " at " + i);
				}
			}
		}
		{
			TestNode[] arr = new TestNode[] {accessor.newNode(14), accessor.newNode(16), accessor.newNode(20)};
			Collection<TestNode> c = accessor.collection(root, root);
			c.addAll(java.util.Arrays.asList(arr));
		}
		{
			int[] check_arr = new int[] {1,3,12,14,16,20};
			TestNode[] arr = accessor.treeToArray(root.root(), new TestNode[0]);
			if (check_arr.length != arr.length)
				TreeTest.err(root.root(), "addAll() failed: expected size " + check_arr.length + " != " + arr.length);
			for (int i = 0; i < check_arr.length; i++) {
				if (check_arr[i] != arr[i].key())
					TreeTest.err(root.root(), "addAll() failed: found: " + arr[i].key() + " instead of " + check_arr[i] + " at " + i);
			}
		}
		{
			Collection<TestNode> c = accessor.collection(root, root);
			c.clear();
			{
				TestNode[] arr = accessor.treeToArray(root.root(), new TestNode[0]);
				if (0 != arr.length)
					TreeTest.err(root.root(), "clear() failed: length: " + arr.length + " != 0");
			}
		}
		if (null != root.root())
			TreeTest.err(root.root(), "root() failed: != null");
	}

	public static void otest(final TestNodeAccessorObj accessorObj) {
		final TestRoot<TestNodeObj> oroot = new TestRoot<TestNodeObj>();
		Integer o1 = 1;
		Integer o2 = 2;
		Integer o3 = 3;
		Integer o4 = 4;
		Integer o5 = 5;
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot, accessorObj.newNode(o1), accessorObj);
			if (null != o)
				TreeTest.err(oroot.root(), "insertObject(1, true, false) failed: found exising node: " + o.key());
		}
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot, accessorObj.newNode(o2), accessorObj);
			if (null != o)
				TreeTest.err(oroot.root(), "insertObject(2, true, false) failed: found exising node: " + o.key());
		}
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot, accessorObj.newNode(o3), accessorObj);
			if (null != o)
				TreeTest.err(oroot.root(), "insertObject(3, true, false) failed: found exising node: " + o.key());
		}
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot, accessorObj.newNode(o4), accessorObj);
			if (null != o)
				TreeTest.err(oroot.root(), "insertObject(4, true, false) failed: found exising node: " + o.key());
		}
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot, accessorObj.newNode(o5), accessorObj);
			if (null != o)
				TreeTest.err(oroot.root(), "insertObject(5, true, false) failed: found exising node: " + o.key());
		}
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot, accessorObj.newNode(o3), accessorObj);
			if (null != o)
				TreeTest.err(oroot.root(), "insertObject(3, false, true) failed: found exising node: " + o.key());
		}
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot, accessorObj.newNode(o4), accessorObj);
			if (null != o)
				TreeTest.err(oroot.root(), "insertObject(4, false, true) failed: found exising node: " + o.key());
		}
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot, accessorObj.newNode(o5), accessorObj);
			if (null != o)
				TreeTest.err(oroot.root(), "insertObject(5, false, true) failed: found exising node: " + o.key());
		}
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot,
				accessorObj.newNode(o3), o3, accessorObj, /*unique:*/false, /*allow_dup:*/false);
			Object _x = (null == o) ? null : o.okey();
			if (_x != o3) {
				TreeTest.err(oroot.root(),
					"insertObject(3, false, false) failed: found " + (null == o ? "null" : o.key()) + " instead of " + o3);
			}
		}
		{
			TestNodeObj o = accessorObj.insertObject(oroot, oroot,
				accessorObj.newNode(o1), o1, accessorObj, /*unique:*/false, /*allow_dup:*/false);
			Object _x = (null == o) ? null : o.okey();
			if (_x != o1)
				TreeTest.err(oroot.root(), "insertObject(1) failed: found " + (null == o ? "null" : o.key()) + " instead of " + o1);
		}
		{
			Collection<TestNodeObj> c1 = accessorObj.collection(oroot, oroot);
			int sz = c1.size();
			ListIterator<TestNodeObj> it = (ListIterator<TestNodeObj>)c1.iterator();
			for (;;) {
				{
					Object o = null;
					while (it.hasNext())
						o = it.next();
					if (null == o)
						break;
				}
				it.remove(); // remove last node
				if (!check_tree(oroot.root()))
					TreeTest.err(oroot.root(), "bad tree after removing last node");
				{
					int sz2 = c1.size();
					if (sz2 != --sz)
						TreeTest.err(oroot.root(), "bad collection size after removing last node: " + sz2 + " != " + sz);
				}
				if (it.nextIndex() != sz)
					TreeTest.err(oroot.root(), "bad it.nextIndex() after removing last node: " + it.nextIndex() + " != " + sz);
				{
					Object o = null;
					while (it.hasPrevious())
						o = it.previous();
					if (null == o)
						break;
				}
				it.remove(); // remove first node
				if (!check_tree(oroot.root()))
					TreeTest.err(oroot.root(), "bad tree after removing first node");
				{
					int sz2 = c1.size();
					if (sz2 != --sz)
						TreeTest.err(oroot.root(), "bad collection size after removing first node: " + sz2 + " != " + sz);
				}
				if (it.previousIndex() != -1)
					TreeTest.err(oroot.root(), "bad it.previousIndex() after removing first node: " + it.previousIndex() + " != -1");
			}
		}
	}
}
