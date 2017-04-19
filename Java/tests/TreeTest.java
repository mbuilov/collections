public class TreeTest {

	public static interface TreeNode {
		public TreeNode left();
		public TreeNode right();
		public int key();
	}

	public static int tree_height(TreeNode tree) {
		if (tree != null) {
			int left_height = tree_height(tree.left());
			int right_height = tree_height(tree.right());
			return 1 + (left_height > right_height ? left_height : right_height);
		}
		return 0;
	}

	public static final int ORDER = 3;

	public static String print_offs(int offs) {
		String s = "                                                                                                                                                                                                                                                                                                                              ";
		if (s.length() > offs)
			s = s.substring(0, offs);
		return s;
	}

	public static String print_tree_line(TreeNode tree, int level, int n, int offs, int height, String pos) {
		if (tree != null) {
			if (level == n) {
				int gap = (offs - 1)*ORDER - pos.length();
				if (gap < 0)
					gap = 0;
				pos += print_offs(gap) + tree.key();
			}
			else {
				pos = print_tree_line(tree.left(), level + 1, n, offs - (1 << (height - 1)), height - 1, pos);
				pos = print_tree_line(tree.right(), level + 1, n, offs + (1 << (height - 1)), height - 1, pos);
			}
		}
		return pos;
	}

	public static int print_tree(TreeNode tree) {
		if (tree != null) {
			int height = tree_height(tree);
			for (int k = 0; k < height; k++) {
				String s = print_tree_line(tree, 0, k, (1 << (height - 1)), height - 1, "");
				System.out.println(s);
			}
			return height;
		}
		System.out.println("null tree!");
		return 0;
	}

	public static void err(TreeNode tree, String s) {
		print_tree(tree);
		System.out.println(s);
		System.exit(-1);
	}
}
