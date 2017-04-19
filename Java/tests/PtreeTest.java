import emcollections.Ptree.*;

public class PtreeTest {

	public abstract static class PtreeTestNodeAccessorBase<
		I extends PtreeTests.TestN<I>,
		T extends PtreeTests.TestNImpl<? super T,T>
	>
		extends PtreeNodeAccessorImpl<I>
		implements PtreeTests.TestNAccessor<I>
	{
		@SuppressWarnings("unchecked") @Override public I left(I current) {return (I)((T)current).left;}
		@SuppressWarnings("unchecked") @Override public I right(I current) {return (I)((T)current).right;}
		@SuppressWarnings("unchecked") @Override public I parent(I current) {return (I)((T)current).parent;}
		@SuppressWarnings("unchecked") @Override public void setLeft(I current, I n) {((T)current).left = (T)n;}
		@SuppressWarnings("unchecked") @Override public void setRight(I current, I n) {((T)current).right = (T)n;}
		@SuppressWarnings("unchecked") @Override public void setParent(I current, I n) {((T)current).parent = (T)n;}
	}

	public static class PtreeTestNodeAccessor
		extends PtreeTestNodeAccessorBase<PtreeTests.TestNode, PtreeTests.TestNodeImpl>
		implements PtreeTests.TestNodeAccessor
	{
		@Override public PtreeTests.TestNode newNode(Integer key) {
			return new PtreeTests.TestNodeImpl(key);
		}
		@Override public int key(PtreeTests.TestNode node) {
			return node.key();
		}
	}

	public static class PtreeTestNodeAccessorObj
		extends PtreeTestNodeAccessorBase<PtreeTests.TestNodeObj, PtreeTests.TestNodeObjImpl>
		implements PtreeTests.TestNodeAccessorObj
	{
		@Override public PtreeTests.TestNodeObj newNode(Integer key) {
			return new PtreeTests.TestNodeObjImpl(key);
		}
		@Override public Integer key(PtreeTests.TestNodeObj node) {
			return node.okey();
		}
	}

	public static void main(String args[]) {
		PtreeTests.test(new PtreeTestNodeAccessor());
		PtreeTests.otest(new PtreeTestNodeAccessorObj());
		System.out.println("all tests ok.");
	}
}
