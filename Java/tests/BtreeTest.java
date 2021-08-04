/**********************************************************************************
* Embedded binary tree
* Copyright (C) 2012-2021 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

import emcollections.Btree.*;
import java.util.Collection;
import java.util.Random;

public class BtreeTest {

	public abstract static class TestNodeN<N extends TestNodeN<N>> implements TreeTest.TreeNode {
		public N left;
		public N right;
		@Override public TreeTest.TreeNode left() {return left;}
		@Override public TreeTest.TreeNode right() {return right;}
	}

	public static class TestNode extends TestNodeN<TestNode> {
		public int key;
		@Override public int key() {return key;}
		public TestNode(int k) {
			key = k;
		}
	}

	public static class OTestNode extends TestNodeN<OTestNode> {
		public TestNode obj;
		@Override public int key() {return obj.key;}
		public OTestNode(TestNode x) {
			obj = x;
		}
	}

	public static class BtreeNodeReadAccessorN<N extends TestNodeN<N>> extends BtreeNodeReadAccessorImpl<N> {
		@Override public N left(N c) {return c.left;}
		@Override public N right(N c) {return c.right;}
	};

	public static final BtreeNodeReadAccessor<TestNode> accessor = new BtreeNodeReadAccessorN<TestNode>();
	public static final BtreeNodeReadAccessor<OTestNode> oaccessor = new BtreeNodeReadAccessorN<OTestNode>();

	/*      8
	   4         12
	 2   6    10    14
	1 3 5 7  9 11 13 15  */

	static TestNode build_tree() {
		TestNode tree = new TestNode(8);
		tree.left = new TestNode(4);
		tree.right = new TestNode(12);
		tree.left.left = new TestNode(2);
		tree.left.right = new TestNode(6);
		tree.left.left.left = new TestNode(1);
		tree.left.left.right = new TestNode(3);
		tree.left.right.left = new TestNode(5);
		tree.left.right.right = new TestNode(7);
		tree.right.left = new TestNode(10);
		tree.right.right = new TestNode(14);
		tree.right.left.left = new TestNode(9);
		tree.right.left.right = new TestNode(11);
		tree.right.right.left = new TestNode(13);
		tree.right.right.right = new TestNode(15);
		return tree;
	}

	public static TestNode test1() {
		final TestNode tree = build_tree();
		{
			TestNode first = accessor.treeFirst(tree);
			if (null == first || 1 != first.key)
				TreeTest.err(tree, "bad first: 1 != " + (null == first ? "null" : ("(key=" + first.key + ")")));
		}
		{
			TestNode last = accessor.treeLast(tree);
			if (null == last || 15 != last.key)
				TreeTest.err(tree, "bad last: 15 != " + (null == last ? "null" : ("(key=" + last.key + ")")));
		}
		{
			TestNode first = accessor.treeFirst(tree.right);
			if (null == first || 9 != first.key)
				TreeTest.err(tree, "bad sub first: 9 != " + (null == first ? "null" : ("(key=" + first.key + ")")));
		}
		{
			TestNode last = accessor.treeLast(tree.left);
			if (null == last || 7 != last.key)
				TreeTest.err(tree, "bad sub last: 7 != " + (null == last ? "null" : ("(key=" + last.key + ")")));
		}
		{
			int sz = accessor.treeSize(tree);
			if (15 != sz)
				TreeTest.err(tree, "bad sz = " + sz + " != 15");
		}
		{
			int ht = accessor.treeHeight(tree);
			if (4 != ht)
				TreeTest.err(tree, "bad ht = " + ht + " != 4");
		}
		{
			int sub_sz = accessor.treeSize(tree.left);
			if (7 != sub_sz)
				TreeTest.err(tree, "bad sub sz = " + sub_sz + " != 7");
		}
		{
			int sub_ht = accessor.treeHeight(tree.right);
			if (3 != sub_ht)
				TreeTest.err(tree, "bad sub_ht = " + sub_ht + " != 3");
		}
		{
			if (!accessor.treeContains(tree, tree))
				TreeTest.err(tree, "bad contains(" + tree.key + ")");
		}
		{
			if (!accessor.treeContains(tree, tree.left.right))
				TreeTest.err(tree, "bad contains(" + tree.left.right.key + ")");
		}
		{
			Collection<TestNode> c = java.util.Arrays.asList(new TestNode[] {
				tree.left,
				tree.right,
				tree.left.left.right
			});
			if (!accessor.treeContainsAll(tree, c))
				TreeTest.err(tree, "bad containsAll()");
		}
		{
			TestNode[] arr = accessor.treeToArray(tree, new TestNode[0]);
			if (arr.length != 15)
				TreeTest.err(tree, "bad toArray<T>() length = " + arr.length);
			for (int q = 0; q < arr.length; q++) {
				if (arr[q].key != q + 1)
					TreeTest.err(tree, "bad toArray<T>() contents: arr[" + q + "] != " + (q + 1));
			}
			if (!accessor.treeContainsAll(tree, java.util.Arrays.asList(arr)))
				TreeTest.err(tree, "bad toArray<T>() returned");
		}
		{
			Object[] arr = accessor.treeToArray(tree);
			if (arr.length != 15)
				TreeTest.err(tree, "bad toArray<T>() length = " + arr.length);
			for (int q = 0; q < arr.length; q++) {
				if (((TestNode)arr[q]).key != q + 1)
					TreeTest.err(tree, "bad toArray<T>() contents: arr[" + q + "] != " + (q + 1));
			}
			if (!accessor.treeContainsAll(tree, java.util.Arrays.asList(arr)))
				TreeTest.err(tree, "bad toArray<T>() returned");
		}
		return tree;
	}

	public static TestNode walk_test1(final TestNode tree) {
		{
			TestNode[] arr = new TestNode[accessor.treeSize(tree)];
			class WalkerData {
				private final TestNode[] arr;
				private int filled = 0;
				WalkerData(TestNode[] _arr) {
					arr = _arr;
				}
			}
			{
				TestNode _n = accessor.treeWalkRecursiveForward(tree, new WalkerData(arr), new BtreeCallback<TestNode,WalkerData>() {
					@Override public boolean process(TestNode n, WalkerData d) {
						d.arr[d.filled++] = n;
						return false;
					}
				});
				if (_n != null)
					TreeTest.err(tree, "bad treeWalkRecursiveForward(): not expecting returned node: " + _n.key);
			}
			for (int i = 0; i < arr.length; i++) {
				if (arr[i].key != i + 1)
					TreeTest.err(tree, "bad walkRecursiveForward(" + arr[i].key + ") != " + (i + 1));
			}
		}
		{
			TestNode[] arr = new TestNode[accessor.treeSize(tree)];
			class WalkerData {
				private final TestNode[] arr;
				private int filled = 0;
				WalkerData(TestNode[] _arr) {
					arr = _arr;
				}
			}
			{
				TestNode _n = accessor.treeWalkRecursiveBackward(tree, new WalkerData(arr), new BtreeCallback<TestNode,WalkerData>() {
					@Override public boolean process(TestNode n, WalkerData d) {
						d.arr[d.filled++] = n;
						return false;
					}
				});
				if (_n != null)
					TreeTest.err(tree, "bad treeWalkRecursiveBackward(): not expecting returned node: " + _n.key);
			}
			for (int i = 0; i < arr.length; i++) {
				if (arr[i].key != arr.length - i)
					TreeTest.err(tree, "bad walkRecursiveBackward(" + arr[i].key + ") != " + (arr.length - i));
			}
			{
				TestNode[] arr2 = new TestNode[accessor.treeSize(tree)];
				{
					TestNode _n = accessor.treeWalkRecursive(tree, new WalkerData(arr2), new BtreeCallback<TestNode,WalkerData>() {
						@Override public boolean process(TestNode n, WalkerData d) {
							d.arr[d.filled++] = n;
							return false;
						}
					});
					if (_n != null)
						TreeTest.err(tree, "bad treeWalkRecursive(): not expecting returned node: " + _n.key);
				}
				if (!java.util.Arrays.asList(arr).containsAll(java.util.Arrays.asList(arr2)))
					TreeTest.err(tree, "bad walkRecursive()");
			}
		}
		return tree;
	}

	public static TestNode search_test1(final TestNode tree) {
		{
			TestNode n = accessor.treeSearch(tree, Integer.valueOf(13), new BtreeComparator<TestNode,Integer>() {
				@Override public int compare(TestNode node, Integer key) {
					return node.key - key;
				}
			});
			if (null == n || n.key != 13)
				TreeTest.err(tree, "bad search(13) returned: " + (null == n ? "null" : n.key));
		}
		{
			TestNode n = accessor.treeSearchInt(tree, 10, new BtreeKeyExtractorInt<TestNode>() {
				@Override public int key(TestNode node) {
					return node.key;
				}
			});
			if (null == n || n.key != 10)
				TreeTest.err(tree, "bad searchInt(10) returned: " + (null == n ? "null" : n.key));
		}
		{
			TestNode n = accessor.treeSearchRecursiveInt(tree, 3, new BtreeKeyExtractorInt<TestNode>() {
				@Override public int key(TestNode node) {
					return node.key;
				}
			});
			if (null == n || n.key != 3)
				TreeTest.err(tree, "bad searchRecursiveInt(3) returned: " + (null == n ? "null" : n.key));
		}
		{
			TestNode n = accessor.treeSearchRecursiveObject(tree, tree.left.right.left, new BtreeKeyExtractorObject<TestNode,TestNode>() {
				@Override public TestNode key(TestNode node) {
					return node.left;
				}
			});
			if (null == n || n.key != tree.left.right.key)
				TreeTest.err(tree, "bad searchRecursiveObject(" + tree.left.right.key + ") returned: " + (null == n ? "null" : n.key));
		}
		{
			TestNode n = accessor.treeSearchRecursiveObject(tree, tree.right.right.right, new BtreeKeyExtractorObject<TestNode,TestNode>() {
				@Override public TestNode key(TestNode node) {
					return node.right;
				}
			});
			if (n != tree.right.right)
				TreeTest.err(tree, "bad searchRecursiveObject(" + tree.right.right.key + ") returned: " + (null == n ? "null" : n.key));
		}
		return tree;
	}

	public static TestNode parent_test1(final TestNode tree) {
		{
			BtreeParent<TestNode> parent = new BtreeParent<TestNode>(tree);
			int c = accessor.searchParentInt(parent, tree.left.left.key, new BtreeKeyExtractorInt<TestNode>() {
				@Override public int key(TestNode node) {
					return node.key;
				}
			}, false);
			if (c != 0 || parent.p != tree.left.left)
				TreeTest.err(tree, "bad searchParentInt(" + tree.left.left.key + ", false) returned: " + c + ", " +
					(null == parent.p ? "null" : parent.p.key));
		}
		{
			BtreeParent<TestNode> parent = new BtreeParent<TestNode>(tree);
			int c = accessor.searchParentInt(parent, tree.left.left.key, new BtreeKeyExtractorInt<TestNode>() {
				@Override public int key(TestNode node) {
					return node.key;
				}
			}, true);
			if (c == 0 || (parent.p != tree.left.left.left && parent.p != tree.left.left.right))
				TreeTest.err(tree, "bad searchParentInt(" + tree.left.left.key + ", true) returned: " + c + ", " +
					(null == parent.p ? "null" : parent.p.key));
		}
		{
			BtreeParent<TestNode> parent = new BtreeParent<TestNode>(tree);
			int c = accessor.searchParentInt(parent, 22, new BtreeKeyExtractorInt<TestNode>() {
				@Override public int key(TestNode node) {
					return node.key;
				}
			}, false);
			if (c >= 0)
				TreeTest.err(tree, "bad searchParentInt(22) returned: " + c);
		}
		{
			BtreeParent<TestNode> parent = new BtreeParent<TestNode>(tree);
			int c = accessor.searchParentInt(parent, 0, new BtreeKeyExtractorInt<TestNode>() {
				@Override public int key(TestNode node) {
					return node.key;
				}
			}, false);
			if (c <= 0)
				TreeTest.err(tree, "bad searchParentInt(0) returned: " + c);
		}
		return tree;
	}

	public static void sub_test1(final TestNode tree) {
		class TestRangeKey {
			private final int range_start;
			private final int range_end;
			TestRangeKey(int _range_start, int _range_end) {
				range_start = _range_start;
				range_end = _range_end;
			}
		}
		TestRangeKey range_key = new TestRangeKey(2,5);
		BtreeComparator<TestNode,TestRangeKey> range_cmp = new BtreeComparator<TestNode,TestRangeKey>() {
			@Override public int compare(TestNode node, TestRangeKey key) {
				if (node.key < key.range_start)
					return -1;
				if (node.key > key.range_end)
					return 1;
				return 0;
			}
		};
		TestNode n = accessor.treeSearch(tree, range_key, range_cmp);
		if (null == n || n.key != 4)
			TreeTest.err(tree, "bad search(2,5) returned: " + (null == n ? "null" : n.key));
		{
			class WalkerRangeData {
				private final int[] arr;
				private int filled = 0;
				WalkerRangeData(int c) {
					arr = new int[c];
				}
			}
			BtreeCallback<TestNode,WalkerRangeData> range_cb = new BtreeCallback<TestNode,WalkerRangeData>() {
				@Override public boolean process(TestNode node, WalkerRangeData data) {
					data.arr[data.filled++] = node.key;
					return false;
				}
			};
			{
				WalkerRangeData range_data = new WalkerRangeData(4);
				TestNode n2 = accessor.walkSubRecursive(n, range_key, range_data, range_cmp, range_cb);
				if (null != n2)
					TreeTest.err(tree, "bad walkSubRecursive(2,5) unexpected return: " + n2.key);
				if (range_data.arr.length != range_data.filled)
					TreeTest.err(tree, "bad walkSubRecursive(2,5) filled: " + range_data.filled + " != " + range_data.arr.length);
				for (int i = 2; i <= 5; i++) {
					int t = 0;
					for (; t != range_data.arr.length; t++) {
						if (range_data.arr[t] == i)
							break;
					}
					if (t == range_data.arr.length)
						TreeTest.err(tree, "bad walkSubRecursive(2,5): couldn't find " + i);
				}
			}
			{
				WalkerRangeData range_data = new WalkerRangeData(4);
				TestNode n2 = accessor.walkSubRecursiveForward(n, range_key, range_data, range_cmp, range_cb);
				if (null != n2)
					TreeTest.err(tree, "bad walkSubRecursiveForward(2,5) unexpected return: " + n2.key);
				if (range_data.arr.length != range_data.filled)
					TreeTest.err(tree, "bad walkSubRecursiveForward(2,5) filled: " + range_data.filled + " != " +
						range_data.arr.length);
				for (int i = 2; i <= 5; i++) {
					if (i != range_data.arr[i-2])
						TreeTest.err(tree, "bad walkSubRecursiveForward(2,5): bad value at [" + (i-2) + "]=" +
							range_data.arr[i-2] + " != " + i);
				}
			}
			{
				WalkerRangeData range_data = new WalkerRangeData(4);
				TestNode n2 = accessor.walkSubRecursiveBackward(n, range_key, range_data, range_cmp, range_cb);
				if (null != n2)
					TreeTest.err(tree, "bad walkSubRecursiveBackward(2,5) unexpected return: " + n2.key);
				if (range_data.arr.length != range_data.filled)
					TreeTest.err(tree, "bad walkSubRecursiveBackward(2,5) filled: " + range_data.filled + " != " +
						range_data.arr.length);
				for (int i = 2; i <= 5; i++) {
					if (5 - (i-2) != range_data.arr[i-2])
						TreeTest.err(tree, "bad walkSubRecursiveBackward(2,5): bad value at [" + (i-2) + "]=" +
							range_data.arr[i-2] + " != " + (5 - (i-2)));
				}
			}
		}
	}

	public static TestNode test2(Random rnd, int mx) {
		final BtreeKeyExtractorInt<TestNode> ext = new BtreeKeyExtractorInt<TestNode>() {
			@Override public int key(TestNode n) {
				return n.key;
			}
		};
		TestNode tree = null;
		int uk = 0;
		final BtreeParent<TestNode> parent = new BtreeParent<TestNode>(null);
		for (int i = mx; i > 0; i--) {
			TestNode n = new TestNode(null == rnd ? uk++ : (i == 1 ? 31 : (rnd.nextInt() & 127)));
			if (null == tree)
				tree = n;
			else {
				parent.p = tree;
				{
					int c = accessor.searchParentInt(parent, n.key, ext, /*leaf:*/rnd != null);
					if (c == 0)
						TreeTest.err(tree, "bad searchParentInt(" + n.key + ") returned: " + c);
					if (c < 0)
						parent.p.right = n;
					else
						parent.p.left = n;
				}
				{
					class WalkerData {
						private final TestNode tree;
						private TestNode prev = null;
						WalkerData(TestNode _tree) {
							tree = _tree;
						}
					}
					WalkerData wd = new WalkerData(tree);
					{
						TestNode _n = accessor.treeWalkRecursiveForward(tree, wd,
							new BtreeCallback<TestNode,WalkerData>() {
								@Override public boolean process(TestNode n, WalkerData d) {
									if (d.prev != null && d.prev.key > n.key)
										TreeTest.err(d.tree, "unsorted tree: " + d.prev.key + " > " + n.key);
									d.prev = n;
									return false;
								}
							}
						);
						if (_n != null)
							TreeTest.err(tree, "bad treeWalkRecursiveForward(): not expecting returned node: " + _n.key);
					}
					{
						TestNode last = accessor.treeLast(tree);
						if (wd.prev != last)
							TreeTest.err(tree, "bad treeWalkRecursiveForward(): last checked node " + wd.prev.key + " != " + last.key);
					}
				}
			}
			if (0 == (i % 1000))
				System.out.println(i);
		}
		{
			int arr[] = new int[] {0,0};
			{
				TestNode _n = accessor.treeWalkRecursive(tree, arr, new BtreeCallback<TestNode,int[]>() {
					@Override public boolean process(TestNode n, int[] data) {
						if (31 == n.key)
							data[0]++;
						data[1]++;
						return false;
					}
				});
				if (_n != null)
					TreeTest.err(tree, "bad treeWalkRecursive(): not expecting returned node: " + _n.key);
			}
			if (0 == arr[0])
				TreeTest.err(tree, "node with key == 31 was not found!");
			if (arr[1] != mx)
				TreeTest.err(tree, "bad treeWalkRecursive(): only " + arr[1] + " nodes checked, total: " + mx);
			{
				TestNode x = accessor.treeSearchInt(tree, 31, ext);
				if (null == x || 31 != x.key)
					TreeTest.err(tree, "bad searchInt(31) returned: " + (null == x ? "null" : x.key));
				{
					int arr2[] = new int[] {0};
					TestNode _n = accessor.walkSubRecursiveInt(x, 31, arr2, ext,
						new BtreeCallback<TestNode,int[]>() {
							@Override public boolean process(TestNode n, int[] data) {
								data[0]++;
								return false;
							}
						}
					);
					if (_n != null)
						TreeTest.err(tree, "bad walkSubRecursiveInt(31): " + _n.key);
					if (arr[0] != arr2[0])
						TreeTest.err(tree, "bad walkSubRecursiveInt(31): " + arr2[0] + " but expecting " + arr[0]);
				}
			}
			System.out.println("checked sub-recursive: " + arr[0] + " of total " + arr[1]);
		}
		return tree;
	}

	public static OTestNode test3(Random rnd, TestNode tree, int mx) {
		final BtreeKeyExtractorInt<TestNode> ext = new BtreeKeyExtractorInt<TestNode>() {
			@Override public int key(TestNode n) {
				return n.key;
			}
		};
		final BtreeKeyExtractorObject<OTestNode,TestNode> oext = new BtreeKeyExtractorObject<OTestNode,TestNode>() {
			@Override public TestNode key(OTestNode n) {
				return n.obj;
			}
		};
		OTestNode otree = null;
		int uk = 0;
		final BtreeParent<OTestNode> parent = new BtreeParent<OTestNode>(null);
		for (int i = mx; i > 0; i--) {
			TestNode x;
			do {
				int key = (null == rnd) ? uk++ : (rnd.nextInt() & 127);
				x = accessor.treeSearchInt(tree, key, ext);
				if (null == rnd && 0 == (uk % 1000))
					System.out.println(uk);
			} while (null == x);
			{
				OTestNode n = new OTestNode(x);
				if (null == otree)
					otree = n;
				else {
					parent.p = otree;
					{
						int c = oaccessor.searchParentObject(parent, n.obj, oext,
							/*unique:*/rnd == null, /*leaf:*/rnd != null);
						if (c == 0)
							TreeTest.err(tree, "bad searchParentObject(" + n.key() + ") returned: " + c);
						if (c < 0)
							parent.p.right = n;
						else
							parent.p.left = n;
					}
				}
			}
		}
		{
			final TestNode _tree = tree;
			/* otree - ordered tree of nodes with non-unique object keys,
			  but there are no two different object keys with the same integer value */
			final BtreeCallback<OTestNode,OTestNode> cb1 = new BtreeCallback<OTestNode,OTestNode>() {
				@Override public boolean process(OTestNode node, OTestNode data) {
					if (node.key() == data.key() && node.obj != data.obj)
						TreeTest.err(_tree, "bad searchInt(" + node.key() + "): found two different nodes with the same key");
					return false;
				}
			};
			OTestNode _n = oaccessor.treeWalkRecursiveForward(otree, /*data:*/otree, new BtreeCallback<OTestNode,OTestNode>() {
				@Override public boolean process(OTestNode node, OTestNode data/*otree*/) {
					{
						OTestNode _n2 = oaccessor.treeWalkRecursive(data/*otree*/, node, cb1);
						if (_n2 != null)
							TreeTest.err(_tree, "bad treeWalkRecursive(): not expecting returned node: " + _n2.obj.key);
					}
					{
						OTestNode n = oaccessor.treeSearchObject(data/*otree*/, node.obj, oext);
						if (null == n || n.obj != node.obj)
							TreeTest.err(data/*otree*/, "bad searchObject(" + node.key() + ") returned: " + (null == n ? "null" : n.key()));
						n = oaccessor.walkSubRecursiveObject(n, node.obj, /*data:*/node, oext,
							new BtreeCallback<OTestNode,OTestNode>() {
								@Override public boolean process(OTestNode node, OTestNode data) {
									return node == data;
								}
							}
						);
						if (null == n || n != node)
							TreeTest.err(data/*otree*/, "bad walkSubRecursiveObject(" + node.key() + ") returned: " +
								(null == n ? "null" : n.key()));
					}
					return false;
				}
			});
			if (_n != null)
				TreeTest.err(tree, "bad treeWalkRecursiveForward(): not expecting returned node: " + _n.obj.key);
		}
		return otree;
	}

	public static void main(String args[]) {
		boolean print = false;
		int mx = 7*500;
		System.out.println("test1");
		sub_test1(parent_test1(search_test1(walk_test1(test1()))));
		System.out.println("test2 --unique_keys");
		{
			TestNode tree = test2(null, mx);
			if (print)
				TreeTest.print_tree(tree);
			System.out.println("test3 --unique_keys");
			{
				OTestNode otree = test3(null, tree, mx);
				if (print)
					TreeTest.print_tree(otree);
			}
		}
		{
			Random rnd = new Random();
			rnd.setSeed(/*seed:*/0);
			System.out.println("test2 --non-unique keys");
			{
				TestNode tree = test2(rnd, mx);
				if (print)
					TreeTest.print_tree(tree);
				System.out.println("test3 --non-unique keys");
				{
					OTestNode otree = test3(rnd, tree, 18);
					if (print)
						TreeTest.print_tree(otree);
				}
			}
		}
		System.out.println("all tests ok.");
	}
}
