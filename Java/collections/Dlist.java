/**********************************************************************************
* Doubly-linked list
* Copyright (C) 2012-2017 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

package collections;

import java.util.Collection;
import java.util.Iterator;
import java.util.ListIterator;
import java.util.NoSuchElementException;

/* doubly-linked list:

    ------------------------------------------------------------------------
   /     entryN             dlist             entry0            entryN-1    \
   \     ------            -------            ------             ------     /
    ---> |next| --> <null> |first| ---------> |next| --> ... --> |next| >---
    ---< |prev| <--------- |last | <null> <-- |prev| <-- ... <-- |prev| <---
   /     |data|            -------            |data|             |data|     \
   \     ------                               ------             ------     /
    ------------------------------------------------------------------------
*/

/* Note: one object may encapsulate multiple list entries - to place one object into many lists,
  for example an object of class 'Apple' may be placed to 'fruits' and 'food' lists simultaneously:

  class Apple {
    Apple fruit_next;
    Apple fruit_prev;
    Apple food_next;
    Apple food_prev;
  }

  DlistEntryAccessor<Apple> fruit_entry_accessor = new DlistEntryAccessorImpl<Apple>() {
    @Override public Apple next(Apple c)            {return c.fruit_next;}
    @Override public Apple prev(Apple c)            {return c.fruit_prev;}
    @Override public void setNext(Apple c, Apple n) {c.fruit_next = n;}
    @Override public void setPrev(Apple c, Apple n) {c.fruit_prev = n;}
  };

  DlistEntryAccessor<Apple> food_entry_accessor = new DlistEntryAccessorImpl<Apple>() {
    @Override public Apple next(Apple c)            {return c.food_next;}
    @Override public Apple prev(Apple c)            {return c.food_prev;}
    @Override public void setNext(Apple c, Apple n) {c.food_next = n;}
    @Override public void setPrev(Apple c, Apple n) {c.food_prev = n;}
  };

  class Fruits {
    Apple fruit_first;
    Apple fruit_last;
  }

  DlistListAccessor<Fruits, Apple> fruits_list_accessor = new DlistListAccessorImpl<Fruits, Apple>() {
    @Override public Apple first(Fruits list)            {return list.fruit_first;}
    @Override public Apple last(Fruits list)             {return list.fruit_last;}
    @Override public void setFirst(Fruits list, Apple n) {list.fruit_first = n;}
    @Override public void setLast(Fruits list, Apple n)  {list.fruit_last = n;}
  };

  class Food {
    Apple food_first;
    Apple food_last;
  }

  DlistListAccessor<Food, Apple> food_list_accessor = new DlistListAccessorImpl<Food, Apple>() {
    @Override public Apple first(Food list)            {return list.food_first;}
    @Override public Apple last(Food list)             {return list.food_last;}
    @Override public void setFirst(Food list, Apple n) {list.food_first = n;}
    @Override public void setLast(Food list, Apple n)  {list.food_last = n;}
  };

  Apple my_apple = new Apple();
  Fruits fruits = new Fruits();
  Food food = new Food();

  fruit_entry_accessor.addBack(fruits_list_accessor, fruits, my_apple);
  food_entry_accessor.addBack(food_list_accessor, food, my_apple);
*/

public class Dlist {

	/* accessor to 'first' and 'last' fields of object - a list of entries */
	public static interface DlistListReadAccessor<L,E> {
		public E first(L list);                 /* returns null if list is empty */
		public E last(L list);                  /* returns null if list is empty */
		public boolean isEmpty(L list);
	}

	/* accessor to 'next' and 'prev' fields of object - an entry in list */
	public static interface DlistEntryReadAccessor<E> {

		public E next(E current);               /* returns null for the last entry */
		public E prev(E current);               /* returns null for the first entry */

		/* count entries in the list */
		/* NOTE: returns Integer.MAX_VALUE if entries count >= Integer.MAX_VALUE */
		public int listSize(E first/*null?*/);

        /* check if entry is in the list */
		public boolean listContains(E first/*null?*/, E n);

		/* search entries in the list */
		public boolean listContainsAll(E first/*null?*/, Collection<?> c);

		/* fill entries array preserving ordering */
		/* NOTE: cannot fill more than Integer.MAX_VALUE elements */
		public <T> T[] listToArray(E first/*null?*/, T[] a);

		/* fill entries array preserving ordering */
		/* NOTE: cannot fill more than Integer.MAX_VALUE elements */
		public Object[] listToArray(E first/*null?*/);

		public ListIterator<E> listReadIterator(E first/*null?*/);
		public Collection<E> listReadCollection(E first/*null?*/);

		public <L> int size(DlistListReadAccessor<L,E> acc, L list);
		public <L> boolean contains(DlistListReadAccessor<L,E> acc, L list, E n);
		public <L> boolean containsAll(DlistListReadAccessor<L,E> acc, L list, Collection<?> c);
		public <L,T> T[] toArray(DlistListReadAccessor<L,E> acc, L list, T[] a);
		public <L> Object[] toArray(DlistListReadAccessor<L,E> acc, L list);
		public <L> ListIterator<E> readIterator(DlistListReadAccessor<L,E> acc, L list);
		public <L> Collection<E> readCollection(DlistListReadAccessor<L,E> acc, L list);
	}

	public static interface DlistListAccessor<L,E> extends DlistListReadAccessor<L,E> {
		public void setFirst(L list, E n);
		public void setLast(L list, E n);
		public void clear(L list);
		public void move(L dst, L src); /* clear dst, then move elements src -> dst, then clear src */
	}

	public static interface DlistEntryAccessor<E> extends DlistEntryReadAccessor<E> {
		public void setNext(E current, E n);
		public void setPrev(E current, E n);
		/* link two nodes: p.next = n; n.prev = p; */
		public void link(E p, E n);
		/* insert double-linked chain: s <-> x1 <-> ... <-> xN <-> e after/before current entry */
		public <L> void insertAfter(DlistListAccessor<L,E> acc, L list, E current/*null to add front*/, E s, E e);
		public <L> void insertBefore(DlistListAccessor<L,E> acc, L list, E current/*null to add back*/, E s, E e);
		/* insert an entry after/before current entry */
		public <L> void insertAfter(DlistListAccessor<L,E> acc, L list, E current/*null to add front*/, E e);
		public <L> void insertBefore(DlistListAccessor<L,E> acc, L list, E current/*null to add back*/, E e);
		/* insert a list after/before current entry, then clear source list */
		public <L> void insertListAfter(DlistListAccessor<L,E> acc, L dst, E current/*null to add front*/, L src); /* clears src */
		public <L> void insertListBefore(DlistListAccessor<L,E> acc, L dst, E current/*null to add back*/, L src); /* clears src */
		/* same as insertAfter()/insertBefore() with current == null */
		public <L> void addFront(DlistListAccessor<L,E> acc, L list, E e);
		public <L> void addBack(DlistListAccessor<L,E> acc, L list, E e);
		public <L> void addListFront(DlistListAccessor<L,E> acc, L dst, L src); /* clears src */
		public <L> void addListBack(DlistListAccessor<L,E> acc, L dst, L src); /* clears src */
		public <L> void remove(DlistListAccessor<L,E> acc, L list, E e); /* NOTE: entry must be in list */
		public <L> void restore(DlistListAccessor<L,E> acc, L list, E e); /* restore just removed entry */
		public <L> void replace(DlistListAccessor<L,E> acc, L list, E o, E n); /* NOTE: o must be in list, n - must not be in any list */
		public <L> ListIterator<E> iterator(DlistListAccessor<L,E> acc, L list);
		public <L> Collection<E> collection(DlistListAccessor<L,E> acc, L list);
	}

	public static interface DlistModifier<L,E> extends DlistEntryAccessor<E>, DlistListAccessor<L,E> {
		public int size(L list);
		public boolean contains(L list, E n);
		public boolean containsAll(L list, Collection<?> c);
		public <T> T[] toArray(L list, T[] a);
		public Object[] toArray(L list);
		public ListIterator<E> readIterator(L list);
		public Collection<E> readCollection(L list);
		public void insertAfter(L list, E current/*null to add front*/, E s, E e);
		public void insertBefore(L list, E current/*null to add back*/, E s, E e);
		public void insertAfter(L list, E current/*null to add front*/, E e);
		public void insertBefore(L list, E current/*null to add back*/, E e);
		public void insertListAfter(L list, E current/*null to add front*/, L src); /* clears src */
		public void insertListBefore(L list, E current/*null to add back*/, L src); /* clears src */
		public void addFront(L list, E e);
		public void addBack(L list, E e);
		public void addListFront(L dst, L src); /* clears src */
		public void addListBack(L dst, L src); /* clears src */
		public void remove(L list, E e); /* NOTE: entry must be in list */
		public void restore(L list, E e); /* restore just removed entry */
		public void replace(L list, E o, E n); /* NOTE: o must be in list, n - must not be in any list */
		public ListIterator<E> iterator(L list);
		public Collection<E> collection(L list);
	}

	/* *************** implementation ************** */

	public abstract static class DlistListReadAccessorImpl<L,E> implements DlistListReadAccessor<L,E> {

		//need to implement:
		//public E first(L list);
		//public E last(L list);

		/* if returns true then both first and last are nulls */
		@Override public boolean isEmpty(L list) {
			return null == first(list);
		}
	}

	public abstract static class DlistEntryReadAccessorImpl<E> implements DlistEntryReadAccessor<E> {

		//need to implement:
		//public E next(E current);
		//public E prev(E current);

		/* count entries in the list */
		/* NOTE: returns Integer.MAX_VALUE if entries count >= Integer.MAX_VALUE */
		@Override public int listSize(E first/*null?*/) {
			int s = 0;
			while (first != null) {
				if (Integer.MAX_VALUE == ++s)
					break;
				first = next(first);
			}
			return s;
		}

        /* check if entry is in the list */
		@Override public boolean listContains(E first/*null?*/, final E n) {
			for (;;) {
				if (first == n)
					return true;
				if (null == first)
					return false;
				first = next(first);
			}
		}

		/* search entries in the list */
		@SuppressWarnings("unchecked")
		@Override public boolean listContainsAll(E first/*null?*/, Collection<?> c) {
			for (Object e : c)
				if (!listContains(first, (E)e))
					return false;
			return true;
		}

		private void _fill_array(E first/*null?*/, Object[] arr) {
			for (int i = 0; null != first; first = next(first), i++)
				arr[i] = first;
		}

		/* fill entries array preserving ordering */
		/* NOTE: cannot fill more than Integer.MAX_VALUE elements */
		@SuppressWarnings("unchecked")
		@Override public <T> T[] listToArray(E first/*null?*/, T[] a) {
			T[] arr = (T[])java.lang.reflect.Array.newInstance(a.getClass().getComponentType(), listSize(first));
			_fill_array(first, arr);
			return arr;
		}

		/* fill entries array preserving ordering */
		/* NOTE: cannot fill more than Integer.MAX_VALUE elements */
		@Override public Object[] listToArray(E first/*null?*/) {
			Object[] arr = new Object[listSize(first)];
			_fill_array(first, arr);
			return arr;
		}

		public abstract class DlistIteratorBase implements ListIterator<E> {
			protected long idx = 0;
			protected E next;
			protected E prev = null;
			protected DlistIteratorBase(E first/*null?*/) {next = first;}
			@Override public int nextIndex()       {return idx < Integer.MAX_VALUE ? (int)idx : Integer.MAX_VALUE;}
			@Override public int previousIndex()   {return nextIndex() - 1;}
			@Override public boolean hasNext()     {return null != next;}
			@Override public boolean hasPrevious() {return null != prev;}
			@Override public E next() {
				if (null != next) {
					prev = next;
					next = DlistEntryReadAccessorImpl.this.next(next);
					idx++;
					return prev;
				}
				throw new NoSuchElementException();
			}
			@Override public E previous() {
				if (null != prev) {
					next = prev;
					prev = DlistEntryReadAccessorImpl.this.prev(prev);
					idx--;
					return next;
				}
				throw new NoSuchElementException();
			}
		}

		public class DlistReadIterator extends DlistIteratorBase {
			protected DlistReadIterator(E first/*null?*/) {super(first);}
			@Override public void remove() {throw new UnsupportedOperationException();}
			@Override public void set(E e) {throw new UnsupportedOperationException();}
			@Override public void add(E e) {throw new UnsupportedOperationException();}
		}

		@Override public ListIterator<E> listReadIterator(E first/*null?*/) {
			return new DlistReadIterator(first);
		}

		public class DlistReadCollection implements Collection<E> {
			protected final E first;
			protected DlistReadCollection(E _first) {first = _first;}
			@Override public ListIterator<E> iterator() {
				return DlistEntryReadAccessorImpl.this.listReadIterator(first);
			}
			@Override public boolean isEmpty() {
				return null == first;
			}
			@Override public int size() {
				return DlistEntryReadAccessorImpl.this.listSize(first);
			}
			@SuppressWarnings("unchecked")
			@Override public boolean contains(Object o) {
				return DlistEntryReadAccessorImpl.this.listContains(first, (E)o);
			}
			@Override public boolean containsAll(Collection<?> c) {
				return DlistEntryReadAccessorImpl.this.listContainsAll(first, c);
			}
			@Override public <T> T[] toArray(T[] a) {
				return DlistEntryReadAccessorImpl.this.listToArray(first, a);
			}
			@Override public Object[] toArray() {
				return DlistEntryReadAccessorImpl.this.listToArray(first);
			}
			@Override public boolean add(E e)                          {throw new UnsupportedOperationException();}
			@Override public boolean remove(Object o)                  {throw new UnsupportedOperationException();}
			@Override public boolean addAll(Collection<? extends E> c) {throw new UnsupportedOperationException();}
			@Override public boolean removeAll(Collection<?> c)        {throw new UnsupportedOperationException();}
			@Override public boolean retainAll(Collection<?> c)        {throw new UnsupportedOperationException();}
			@Override public void clear()                              {throw new UnsupportedOperationException();}
		}

		@Override public Collection<E> listReadCollection(E first/*null?*/) {
			return new DlistReadCollection(first);
		}

		@Override public <L> int size(DlistListReadAccessor<L,E> acc, L list) {
			return listSize(acc.first(list));
		}
		@Override public <L> boolean contains(DlistListReadAccessor<L,E> acc, L list, E n) {
			return listContains(acc.first(list), n);
		}
		@Override public <L> boolean containsAll(DlistListReadAccessor<L,E> acc, L list, Collection<?> c) {
			return listContainsAll(acc.first(list), c);
		}
		@Override public <L,T> T[] toArray(DlistListReadAccessor<L,E> acc, L list, T[] a) {
			return listToArray(acc.first(list), a);
		}
		@Override public <L> Object[] toArray(DlistListReadAccessor<L,E> acc, L list) {
			return listToArray(acc.first(list));
		}
		@Override public <L> ListIterator<E> readIterator(DlistListReadAccessor<L,E> acc, L list) {
			return listReadIterator(acc.first(list));
		}
		@Override public <L> Collection<E> readCollection(DlistListReadAccessor<L,E> acc, L list) {
			return listReadCollection(acc.first(list));
		}
	}

	public abstract static class DlistListAccessorImpl<L,E>
		extends DlistListReadAccessorImpl<L,E>
		implements DlistListAccessor<L,E>
	{
		//need to implement:
		//public E first(L list);
		//public E last(L list);
		//public void setFirst(L list, E n);
		//public void setLast(L list, E n);

		/* set first and last to null */
		@Override public void clear(L list) {
			setFirst(list, null);
			setLast(list, null);
		}

		/* move items from src list to dst, then clear src list */
		@Override public void move(L dst, L src) {
			setFirst(dst, first(src));
			setLast(dst, last(src));
			clear(src);
		}
	}

	public abstract static class DlistEntryAccessorImpl<E>
		extends DlistEntryReadAccessorImpl<E>
		implements DlistEntryAccessor<E>
	{
		//need to implement:
		//public E next(E current);
		//public E prev(E current);
		//public void setNext(E current, E n);
		//public void setPrev(E current, E n);

		@Override public void link(E p, E n) {
			setNext(p, n);
			setPrev(n, p);
		}

		@Override public <L> void insertAfter(DlistListAccessor<L,E> acc, L list, E current/*null to add front*/, E s, E e) {
			setPrev(s, current);
			{
				E n;
				if (null != current) {
					n = next(current);
					setNext(current, s);
				}
				else {
					n = acc.first(list);
					acc.setFirst(list, s);
				}
				if (null != n)
					setPrev(n, e);
				else
					acc.setLast(list, e);
				setNext(e, n);
			}
		}

		@Override public <L> void insertBefore(DlistListAccessor<L,E> acc, L list, E current/*null to add back*/, E s, E e) {
			setNext(e, current);
			{
				E p;
				if (null != current) {
					p = prev(current);
					setPrev(current, e);
				}
				else {
					p = acc.last(list);
					acc.setLast(list, e);
				}
				if (null != p)
					setNext(p, s);
				else
					acc.setFirst(list, s);
				setPrev(s, p);
			}
		}

		@Override public <L> void insertAfter(DlistListAccessor<L,E> acc, L list, E current/*null to add front*/, E e) {
			insertAfter(acc, list, current, e, e);
		}

		@Override public <L> void insertBefore(DlistListAccessor<L,E> acc, L list, E current/*null to add back*/, E e) {
			insertBefore(acc, list, current, e, e);
		}

		@Override public <L> void insertListAfter(DlistListAccessor<L,E> acc, L dst, E current/*null to add front*/, L src) {
			if (!acc.isEmpty(src)) {
				insertAfter(acc, dst, current, acc.first(src), acc.last(src));
				acc.clear(src);
			}
		}

		@Override public <L> void insertListBefore(DlistListAccessor<L,E> acc, L dst, E current/*null to add back*/, L src) {
			if (!acc.isEmpty(src)) {
				insertBefore(acc, dst, current, acc.first(src), acc.last(src));
				acc.clear(src);
			}
		}

		private <L> void _add_front(DlistListAccessor<L,E> acc, L list, E s, E e) {
			E n = acc.first(list);
			acc.setFirst(list, s);
			if (null != n)
				setPrev(n, e);
			else
				acc.setLast(list, e);
			setNext(e, n);
		}

		private <L> void _add_back(DlistListAccessor<L,E> acc, L list, E s, E e) {
			E p = acc.last(list);
			acc.setLast(list, e);
			if (null != p)
				setNext(p, s);
			else
				acc.setFirst(list, s);
			setPrev(s, p);
		}

		@Override public <L> void addFront(DlistListAccessor<L,E> acc, L list, E e) {
			_add_front(acc, list, e, e);
			setPrev(e, null);
		}

		@Override public <L> void addListFront(DlistListAccessor<L,E> acc, L dst, L src) {
			if (!acc.isEmpty(src)) {
				_add_front(acc, dst, acc.first(src), acc.last(src));
				acc.clear(src);
			}
		}

		@Override public <L> void addBack(DlistListAccessor<L,E> acc, L list, E e) {
			_add_back(acc, list, e, e);
			setNext(e, null);
		}

		@Override public <L> void addListBack(DlistListAccessor<L,E> acc, L dst, L src) {
			if (!acc.isEmpty(src)) {
				_add_back(acc, dst, acc.first(src), acc.last(src));
				acc.clear(src);
			}
		}

		private <L> void _remove(DlistListAccessor<L,E> acc, L list, E p, E n) {
			if (null != p)
				setNext(p, n);
			else
				acc.setFirst(list, n);
			if (null != n)
				setPrev(n, p);
			else
				acc.setLast(list, p);
		}

		@Override public <L> void remove(DlistListAccessor<L,E> acc, L list, E e) {
			_remove(acc, list, prev(e), next(e));
		}

		@Override public <L> void restore(DlistListAccessor<L,E> acc, L list, E e) {
			{
				E p = prev(e);
				if (null != p)
					setNext(p, e);
				else
					acc.setFirst(list, e);
			}
			{
				E n = next(e);
				if (null != n)
					setPrev(n, e);
				else
					acc.setLast(list, e);
			}
		}

		private <L> void _replace(DlistListAccessor<L,E> acc, L list, E p, E n, E e) {
			setPrev(e, p);
			if (null != p)
				setNext(p, e);
			else
				acc.setFirst(list, e);
			setNext(e, n);
			if (null != n)
				setPrev(n, e);
			else
				acc.setLast(list, e);
		}

		@Override public <L> void replace(DlistListAccessor<L,E> acc, L list, E o, E n) {
			_replace(acc, list, prev(o), next(o), n);
		}

		public class DlistIterator<L> extends DlistIteratorBase {
			protected int dir = 0;
			protected final DlistListAccessor<L,E> acc;
			protected final L list;
			protected DlistIterator(DlistListAccessor<L,E> _acc, L _list) {
				super(_acc.first(_list));
				acc = _acc;
				list = _list;
			}
			@Override public E next() {
				E n = super.next(); /* may throw an exception */
				dir = 1;
				return n;
			}
			@Override public E previous() {
				E n = super.previous(); /* may throw an exception */
				dir = -1;
				return n;
			}
			@Override public void remove() {
				if (dir > 0) {
					prev = DlistEntryAccessorImpl.this.prev(prev);
					idx--;
				}
				else if (dir < 0)
					next = DlistEntryAccessorImpl.this.next(next);
				else
					throw new IllegalStateException();
				_remove(acc, list, prev, next);
				dir = 0;
			}
			@Override public void set(E e) {
				if (dir > 0)
					prev = DlistEntryAccessorImpl.this.prev(prev);
				else if (dir < 0)
					next = DlistEntryAccessorImpl.this.next(next);
				else
					throw new IllegalStateException();
				_replace(acc, list, prev, next, e);
				if (dir > 0)
					prev = e;
				else
					next = e;
			}
			@Override public void add(E e) {
				setPrev(e, prev);
				setNext(e, next);
				if (null != prev)
					setNext(prev, e);
				else
					acc.setFirst(list, e);
				if (null != next)
					setPrev(next, e);
				else
					acc.setLast(list, e);
				prev = e;
				idx++;
				dir = 0;
			}
		}

		@Override public <L> ListIterator<E> iterator(DlistListAccessor<L,E> acc, L list) {
			return new DlistIterator<L>(acc, list);
		}

		public class DlistCollection<L> implements Collection<E> {
			protected final DlistListAccessor<L,E> acc;
			protected final L list;
			protected DlistCollection(DlistListAccessor<L,E> _acc, L _list) {
				acc = _acc;
				list = _list;
			}
			@Override public ListIterator<E> iterator() {
				return DlistEntryAccessorImpl.this.iterator(acc, list);
			}
			@Override public boolean isEmpty() {
				return acc.isEmpty(list);
			}
			@Override public int size() {
				return DlistEntryAccessorImpl.this.size(acc, list);
			}
			@SuppressWarnings("unchecked")
			@Override public boolean contains(Object o) {
				return DlistEntryAccessorImpl.this.contains(acc, list, (E)o);
			}
			@Override public boolean containsAll(Collection<?> c) {
				return DlistEntryAccessorImpl.this.containsAll(acc, list, c);
			}
			@Override public <T> T[] toArray(T[] a) {
				return DlistEntryAccessorImpl.this.toArray(acc, list, a);
			}
			@Override public Object[] toArray() {
				return DlistEntryAccessorImpl.this.toArray(acc, list);
			}
			/* append new entry at tail of collection */
			/* NOTE: assume e is not in collection yet */
			@Override public boolean add(E e) {
				addBack(acc, list, e);
				return true;
			}
			/* append new entries at tail of collection */
			/* NOTE: assume new entries are not in collection yet */
			@Override public boolean addAll(Collection<? extends E> c) {
				Iterator<? extends E> i = c.iterator();
				if (i.hasNext()) {
					E p = acc.last(list);
					E e = i.next();
					if (null != p)
						setNext(p, e);
					else
						acc.setFirst(list, e);
					for (;;) {
						setPrev(e, p);
						if (!i.hasNext()) {
							acc.setLast(list, e);
							setNext(e, null);
							return true;
						}
						p = e;
						e = i.next();
						setNext(p, e);
					}
				}
				return false; /* collection not changed - nothing to add */
			}
			@SuppressWarnings("unchecked")
			@Override public boolean remove(Object o) {
				E e = (E)o;
				if (!contains(e))
					return false;
				DlistEntryAccessorImpl.this.remove(acc, list, e);
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
					if (acc.isEmpty(list))
						return false; /* collection not changed - it's already empty */
					acc.clear(list);
					return true;
				}
				{
					boolean changed = false;
					E p = null;
					{
						boolean hole = false;
						for (E e = acc.first(list); null != e; e = next(e)) {
							if (!c.contains(e)) {
								changed = true; /* some element was removed */
								hole = true;
							}
							else {
								if (hole) {
									hole = false;
									if (null != p)
										setNext(p, e);
									else
										acc.setFirst(list, e);
									setPrev(e, p);
								}
								p = e;
							}
						}
					}
					if (changed) {
						acc.setLast(list, p);
						if (null == p)
							acc.setFirst(list, null);
					}
					return changed;
				}
			}
			@Override public void clear() {
				acc.clear(list);
			}
		}

		@Override public <L> Collection<E> collection(DlistListAccessor<L,E> acc, L list) {
			return new DlistCollection<L>(acc, list);
		}
	}

	public abstract static class DlistModifierImpl<L,E>
		extends DlistEntryAccessorImpl<E>
		implements DlistModifier<L,E>
	{
		//need to implement:
		//public E next(E current);
		//public E prev(E current);
		//public E first(L list);
		//public E last(L list);
		//public void setNext(E current, E n);
		//public void setPrev(E current, E n);
		//public void setFirst(L list, E n);
		//public void setLast(L list, E n);
		@Override public int size(L list) {
			return size(this, list);
		}
		@Override public boolean contains(L list, E n) {
			return contains(this, list, n);
		}
		@Override public boolean containsAll(L list, Collection<?> c) {
			return containsAll(this, list, c);
		}
		@Override public <T> T[] toArray(L list, T[] a) {
			return toArray(this, list, a);
		}
		@Override public Object[] toArray(L list) {
			return toArray(this, list);
		}
		@Override public ListIterator<E> readIterator(L list) {
			return readIterator(this, list);
		}
		@Override public Collection<E> readCollection(L list) {
			return readCollection(this, list);
		}
		@Override public void insertAfter(L list, E current/*null to add front*/, E s, E e) {
			insertAfter(this, list, current, s, e);
		}
		@Override public void insertBefore(L list, E current/*null to add back*/, E s, E e) {
			insertBefore(this, list, current, s, e);
		}
		@Override public void insertAfter(L list, E current/*null to add front*/, E e) {
			insertAfter(this, list, current, e);
		}
		@Override public void insertBefore(L list, E current/*null to add back*/, E e) {
			insertBefore(this, list, current, e);
		}
		@Override public void insertListAfter(L list, E current/*null to add front*/, L src) {
			insertListAfter(this, list, current, src);
		}
		@Override public void insertListBefore(L list, E current/*null to add back*/, L src) {
			insertListBefore(this, list, current, src);
		}
		@Override public void addFront(L list, E e) {
			addFront(this, list, e);
		}
		@Override public void addBack(L list, E e) {
			addBack(this, list, e);
		}
		@Override public void addListFront(L dst, L src) {
			addListFront(this, dst, src);
		}
		@Override public void addListBack(L dst, L src) {
			addListBack(this, dst, src);
		}
		@Override public void remove(L list, E e) {
			remove(this, list, e);
		}
		@Override public void restore(L list, E e) {
			restore(this, list, e);
		}
		@Override public void replace(L list, E o, E n) {
			replace(this, list, o, n);
		}
		@Override public ListIterator<E> iterator(L list) {
			return iterator(this, list);
		}
		@Override public Collection<E> collection(L list) {
			return collection(this, list);
		}
		@Override public boolean isEmpty(L list) {
			return null == first(list);
		}
		@Override public void clear(L list) {
			setFirst(list, null);
			setLast(list, null);
		}
		@Override public void move(L dst, L src) {
			setFirst(dst, first(src));
			setLast(dst, last(src));
			clear(src);
		}
	}

// example:
/*
	public static interface IEntry {
		public int getIntData();
		public String getStringData();
	}

	public static class Entry implements IEntry {
		public int int_data;
		public Entry a_next;
		public Entry a_prev;
		public String string_data;
		public Entry b_prev;
		public Entry b_next;

		@Override public int getIntData() {return int_data;}
		@Override public String getStringData() {return string_data;}

		public static final DlistEntryAccessor<Entry> a_accessor = new DlistEntryAccessorImpl<Entry>() {
			@Override public Entry next(Entry c) {return c.a_next;}
			@Override public Entry prev(Entry c) {return c.a_prev;}
			@Override public void setNext(Entry c, Entry n) {c.a_next = n;}
			@Override public void setPrev(Entry c, Entry n) {c.a_prev = n;}
		};
		public static final DlistEntryAccessor<Entry> b_accessor = new DlistEntryAccessorImpl<Entry>() {
			@Override public Entry next(Entry c) {return c.b_next;}
			@Override public Entry prev(Entry c) {return c.b_prev;}
			@Override public void setNext(Entry c, Entry n) {c.b_next = n;}
			@Override public void setPrev(Entry c, Entry n) {c.b_prev = n;}
		};
	}

	public static class Container1 {
		public int data;
		public Entry a_first;
		public Entry a_last;
		public Entry b_first;
		public Entry b_last;

		public static final DlistModifier<Container1, Entry> a_list_modifier = new DlistModifierImpl<Container1, Entry>() {
			@Override public Entry next(Entry current) {return Entry.a_accessor.next(current);}
			@Override public Entry prev(Entry current) {return Entry.a_accessor.prev(current);}
			@Override public void setNext(Entry current, Entry n) {Entry.a_accessor.setNext(current, n);}
			@Override public void setPrev(Entry current, Entry n) {Entry.a_accessor.setPrev(current, n);}
			@Override public Entry first(Container1 list) {return list.a_first;}
			@Override public Entry last(Container1 list) {return list.a_last;}
			@Override public void setFirst(Container1 list, Entry n) {list.a_first = n;}
			@Override public void setLast(Container1 list, Entry n) {list.a_last = n;}
		};
		public static final DlistListAccessor<Container1, Entry> b_list_accessor = new DlistListAccessorImpl<Container1, Entry>() {
			@Override public Entry first(Container1 list) {return list.b_first;}
			@Override public Entry last(Container1 list) {return list.b_last;}
			@Override public void setFirst(Container1 list, Entry n) {list.b_first = n;}
			@Override public void setLast(Container1 list, Entry n) {list.b_last = n;}
		};

		public ListIterator<Entry> a_iterator() {return a_list_modifier.iterator(this);}
		public ListIterator<Entry> b_iterator() {return Entry.b_accessor.iterator(b_list_accessor, this);}
		public Collection<Entry> a_list() {return a_list_modifier.collection(this);}
		public Collection<Entry> b_list() {return Entry.b_accessor.collection(b_list_accessor, this);}

		public ListIterator<? extends Entry> a_read_iterator() {return a_list_modifier.readIterator(this);}
		public ListIterator<? extends Entry> b_read_iterator() {return Entry.b_accessor.listReadIterator(b_list_accessor.first(this));}
		public Collection<? extends Entry> a_read_list() {return a_list_modifier.readCollection(this);}
		public Collection<? extends Entry> b_read_list() {return Entry.b_accessor.listReadCollection(b_list_accessor.first(this));}
	}

	public static class Container2 {
		public Entry b_first;
		public Entry b_last;
		public String data;
		public Entry a_first;
		public Entry a_last;

		public static final DlistModifier<Container2, Entry> a_list_modifier = new DlistModifierImpl<Container2, Entry>() {
			@Override public Entry next(Entry current) {return Entry.a_accessor.next(current);}
			@Override public Entry prev(Entry current) {return Entry.a_accessor.prev(current);}
			@Override public void setNext(Entry current, Entry n) {Entry.a_accessor.setNext(current, n);}
			@Override public void setPrev(Entry current, Entry n) {Entry.a_accessor.setPrev(current, n);}
			@Override public Entry first(Container2 list) {return list.a_first;}
			@Override public Entry last(Container2 list) {return list.a_last;}
			@Override public void setFirst(Container2 list, Entry n) {list.a_first = n;}
			@Override public void setLast(Container2 list, Entry n) {list.a_last = n;}
		};
		public static final DlistModifier<Container2, Entry> b_list_modifier = new DlistModifierImpl<Container2, Entry>() {
			@Override public Entry next(Entry current) {return current.b_next;}
			@Override public Entry prev(Entry current) {return current.b_prev;}
			@Override public void setNext(Entry current, Entry n) {current.b_next = n;}
			@Override public void setPrev(Entry current, Entry n) {current.b_prev = n;}
			@Override public Entry first(Container2 list) {return list.b_first;}
			@Override public Entry last(Container2 list) {return list.b_last;}
			@Override public void setFirst(Container2 list, Entry n) {list.b_first = n;}
			@Override public void setLast(Container2 list, Entry n) {list.b_last = n;}
		};

		public ListIterator<Entry> a_iterator() {return a_list_modifier.iterator(this);}
		public ListIterator<Entry> b_iterator() {return b_list_modifier.iterator(this);}
		public Iterable<Entry> a_list() {return a_list_modifier.collection(this);}
		public Iterable<Entry> b_list() {return b_list_modifier.collection(this);}

		public ListIterator<? extends IEntry> a_read_iterator() {return a_list_modifier.readIterator(this);}
		public ListIterator<? extends IEntry> b_read_iterator() {return Entry.b_accessor.listReadIterator(b_list_modifier.first(this));}
		public Iterable<? extends IEntry> a_read_list() {return a_list_modifier.readCollection(this);}
		public Iterable<? extends IEntry> b_read_list() {return Entry.b_accessor.listReadCollection(b_list_modifier.first(this));}
	}

	public static void iterate_on_a(Container1 c) {
		// iterate without creating any objects
		{
			Entry a = Container1.a_list_modifier.first(c);
			while (a != null) {
				Container1.a_list_modifier.remove(c, a);
				a = Container1.a_list_modifier.next(a);
			}
		}
		// create iterator
		{
			ListIterator<Entry> i = c.a_iterator();
			while (i.hasNext()) {
				Entry a = i.next();
			}
		}
		// create object which creates iterators
		{
			for (Entry a : c.a_list()) {
			}
		}
		// create read iterator
		{
			ListIterator<? extends Entry> i = c.a_read_iterator();
			while (i.hasNext()) {
				Entry ai = i.next();
			}
		}
		// create object which creates read iterators
		{
			for (Entry ai : c.a_read_list()) {
			}
		}
	}

	public static void iterate_on_b(Container1 c) {
		// iterate without creating any objects
		{
			Entry b = Container1.b_list_accessor.first(c);
			while (b != null) {
				Entry.b_accessor.remove(Container1.b_list_accessor, c, b);
				b = Entry.b_accessor.next(b);
			}
		}
		// create iterator
		{
			ListIterator<Entry> i = c.b_iterator();
			while (i.hasNext()) {
				Entry b = i.next();
			}
		}
		// create object which creates iterators
		{
			for (Entry b : c.b_list()) {
			}
		}
	}

	public static void iterate_on_b2(Container2 c) {
		// create read iterator
		{
			ListIterator<? extends IEntry> i = c.a_read_iterator();
			while (i.hasNext()) {
				IEntry ai = i.next();
			}
		}
		// create object which creates read iterators
		{
			for (IEntry bi : c.b_read_list()) {
			}
		}
	}
*/
}
