/**********************************************************************************
* Embedded red-back binary tree of nodes with parent pointers
* Copyright (C) 2012-2021 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

import emcollections.Btree.*;
import emcollections.Ptree.*;
import emcollections.Prbtree.*;
import java.util.ListIterator;
import java.util.Random;

public class PrbtreeTest {

	public static final int rounds = 100;
	public static final int ins = 20;

	public static class PNode implements TreeTest.TreeNode {
		public PNode a_left;
		public PNode a_right;
		public PNode a_parent;
		public boolean a_red;
		public int a_key;
		public PNode(int _key) {
			a_key = _key;
		}
		@Override public TreeTest.TreeNode left() {return a_left;}
		@Override public TreeTest.TreeNode right() {return a_right;}
		@Override public int key() {return a_key;}
	}

	public static class PTree {
		public PNode a_root = null;
	}

	public static final PtreeModifier<PTree,PNode> a_tree_modifier = new PrbtreeModifierImpl<PTree,PNode>() {
		@Override public PNode root(PTree root)                  {return root.a_root;}
		@Override public PNode left(PNode current)               {return current.a_left;}
		@Override public PNode right(PNode current)              {return current.a_right;}
		@Override public PNode parent(PNode current)             {return current.a_parent;}
		@Override public boolean red(PNode current)              {return current.a_red;}
		@Override public void setRoot(PTree root, PNode n)       {root.a_root = n;}
		@Override public void setLeft(PNode current, PNode n)    {current.a_left = n;}
		@Override public void setRight(PNode current, PNode n)   {current.a_right = n;}
		@Override public void setParent(PNode current, PNode n)  {current.a_parent = n;}
		@Override public void setRed(PNode current, boolean red) {current.a_red = red;}
	};

	static void test2() {
		PTree tree = new PTree();
		{
			PNode n1 = new PNode(1);
			PNode n2 = new PNode(2);
			PNode n3 = new PNode(3);
			PNode n4 = new PNode(4);
			PNode n5 = new PNode(5);
			PNode n14 = new PNode(14);
			PNode n15 = new PNode(15);
			PNode n16 = new PNode(16);
			PNode n17 = new PNode(17);
			PNode n18 = new PNode(18);
			PNode n19 = new PNode(19);
			PNode n20 = new PNode(20);
			PNode n21 = new PNode(21);
			PNode n22 = new PNode(22);
			PNode n23 = new PNode(23);
			PNode n24 = new PNode(24);
			PNode n25 = new PNode(25);
			n1.a_red = false;
			n1.a_parent = n2;
			n3.a_red = false;
			n3.a_parent = n2;
			n2.a_red = true;
			n2.a_parent = n4;
			n2.a_left = n1;
			n2.a_right = n3;
			n5.a_red = false;
			n5.a_parent = n4;
			n4.a_red = false;
			n4.a_parent = n14;
			n4.a_left = n2;
			n4.a_right = n5;
			n14.a_red = false;
			n14.a_left = n4;
			n14.a_right = n20;
			n20.a_red = true;
			n20.a_parent = n14;
			n20.a_left = n18;
			n20.a_right = n22;
			n18.a_red = false;
			n18.a_parent = n20;
			n18.a_left = n16;
			n18.a_right = n19;
			n16.a_red = true;
			n16.a_parent = n18;
			n16.a_left = n15;
			n16.a_right = n17;
			n15.a_red = false;
			n15.a_parent = n16;
			n17.a_red = false;
			n17.a_parent = n16;
			n19.a_red = false;
			n19.a_parent = n18;
			n22.a_red = false;
			n22.a_parent = n20;
			n22.a_left = n21;
			n22.a_right = n24;
			n21.a_red = false;
			n21.a_parent = n22;
			n24.a_red = false;
			n24.a_parent = n22;
			n24.a_left = n23;
			n24.a_right = n25;
			n23.a_red = true;
			n23.a_parent = n24;
			n25.a_red = true;
			n25.a_parent = n24;
			tree.a_root = n14;
		}
		{
			int count = a_tree_modifier.readCollection(tree).size();
			if (count != 17)
				TreeTest.err(tree.a_root, "wrong elements count in collection before remove: " + count + ", expecting 17");
			while (count > 0) {
				count--;
				a_tree_modifier.remove(tree, tree.a_root);
				{
					int count2 = a_tree_modifier.readCollection(tree).size();
					if (count2 != count)
						TreeTest.err(tree.a_root, "wrong elements count in collection after remove: " + count2 + ", expecting " + count);
				}
			}
		}
		{
			Random rg = new Random();
			rg.setSeed(0);
			final BtreeKeyExtractorInt<PNode> ext = new BtreeKeyExtractorInt<PNode>() {
				@Override public int key(PNode n) {return n.a_key;}
			};
			for (int round = rounds; round != 0; round--) {
				int cnt = 0;
				for (int i = ins; i != 0; i--) {
					PNode n = new PNode(rg.nextInt(100));
					PNode b = a_tree_modifier.insertInt(tree, n, n.a_key, ext, /*allow_dup:*/(round % 3) == 0);
					if (b == null)
						cnt++;
				}
				for (; cnt != 0; cnt--) {
					int at = rg.nextInt(100) % cnt;
					PNode d = null;
					{
						ListIterator<PNode> it = a_tree_modifier.readIterator(tree);
						for (int pos = 0; it.hasNext(); pos++) {
							d = it.next();
							if (pos == at)
								break;
						}
					}
					{
						int count = a_tree_modifier.readCollection(tree).size();
						if (count != cnt)
							TreeTest.err(tree.a_root, "wrong elements count in collection before remove: " + count + ", expecting " + cnt);
					}
					a_tree_modifier.remove(tree, d);
					{
						int count = a_tree_modifier.size(tree);
						if (count != cnt - 1)
							TreeTest.err(tree.a_root, "wrong elements count in collection after remove: " + count + ", expecting " + (cnt - 1));
					}
				}
			}
		}
	}

	public static interface TestRbN {
		public boolean get_red();
		public void set_red(boolean red);
	}

	public static class TestRbNodeImpl extends PtreeTests.TestNodeImpl implements TestRbN {
		public boolean red;
		@Override public boolean get_red() {return red;}
		@Override public void set_red(boolean red) {this.red = red;}
		@Override public void clear() {
			super.clear();
			red = false;
		}
		public TestRbNodeImpl(int k) {
			super(k);
		}
	}

	public static class TestRbNodeObjImpl extends PtreeTests.TestNodeObjImpl implements TestRbN {
		public boolean red;
		@Override public boolean get_red() {return red;}
		@Override public void set_red(boolean red) {this.red = red;}
		@Override public void clear() {
			super.clear();
			red = false;
		}
		public TestRbNodeObjImpl(Integer k) {
			super(k);
		}
	}

	public abstract static class PrbtreeTestNodeAccessorBase<
		I extends PtreeTests.TestN<I>,
		Q extends I,
		T extends PtreeTests.TestNImpl<I,Q> & TestRbN
	>
		extends PrbtreeNodeAccessorImpl<I>
		implements PtreeTests.TestNAccessor<I>
	{
		@SuppressWarnings("unchecked") @Override public I left(I current) {return ((T)current).left;}
		@SuppressWarnings("unchecked") @Override public I right(I current) {return ((T)current).right;}
		@SuppressWarnings("unchecked") @Override public I parent(I current) {return ((T)current).parent;}
		@SuppressWarnings("unchecked") @Override public boolean red(I current) {return ((T)current).get_red();}
		@SuppressWarnings("unchecked") @Override public void setLeft(I current, I n) {((T)current).left = (Q)n;}
		@SuppressWarnings("unchecked") @Override public void setRight(I current, I n) {((T)current).right = (Q)n;}
		@SuppressWarnings("unchecked") @Override public void setParent(I current, I n) {((T)current).parent = (Q)n;}
		@SuppressWarnings("unchecked") @Override public void setRed(I current, boolean red) {((T)current).set_red(red);}
	}

	public static class PrbtreeTestNodeAccessor
		extends PrbtreeTestNodeAccessorBase<
			PtreeTests.TestNode,
			PtreeTests.TestNodeImpl,
			TestRbNodeImpl
		>
		implements PtreeTests.TestNodeAccessor
	{
		@Override public PtreeTests.TestNode newNode(Integer key) {
			return new TestRbNodeImpl(key);
		}
		@Override public int key(PtreeTests.TestNode node) {
			return node.key();
		}
	}

	public static class PrbtreeTestNodeAccessorObj
		extends PrbtreeTestNodeAccessorBase<
			PtreeTests.TestNodeObj,
			PtreeTests.TestNodeObjImpl,
			TestRbNodeObjImpl
		>
		implements PtreeTests.TestNodeAccessorObj
	{
		@Override public PtreeTests.TestNodeObj newNode(Integer key) {
			return new TestRbNodeObjImpl(key);
		}
		@Override public Integer key(PtreeTests.TestNodeObj node) {
			return node.okey();
		}
	}

	public static void main(String args[]) {
		PtreeTests.test(new PrbtreeTestNodeAccessor());
		PtreeTests.otest(new PrbtreeTestNodeAccessorObj());
		test2();
		System.out.println("all tests ok.");
	}
}
