/**********************************************************************************
* Embedded doubly-linked list
* Copyright (C) 2012-2021 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

import emcollections.Dlist.*;
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.ListIterator;

public class DlistTest {

	public static class MyStruct {
		public MyStruct next;
		public MyStruct prev;
		public int m;
		public MyStruct(int _m) {
			m = _m;
		}
	}

	public static class MyList {
		public MyStruct first;
		public MyStruct last;
		public static final DlistModifier<MyList,MyStruct> modifier = new DlistModifierImpl<MyList,MyStruct>() {
			@Override public MyStruct first(MyList list)                {return list.first;}
			@Override public MyStruct last(MyList list)                 {return list.last;}
			@Override public void setFirst(MyList list, MyStruct n)     {list.first = n;}
			@Override public void setLast(MyList list, MyStruct n)      {list.last = n;}
			@Override public MyStruct next(MyStruct current)            {return current.next;}
			@Override public MyStruct prev(MyStruct current)            {return current.prev;}
			@Override public void setNext(MyStruct current, MyStruct n) {current.next = n;}
			@Override public void setPrev(MyStruct current, MyStruct n) {current.prev = n;}
		};
	}

	public static void err(MyList list, String s) {
		MyStruct n = MyList.modifier.first(list);
		String t = "";
		for (boolean first = true; n != null; n = MyList.modifier.next(n), first = false) {
			if (!first)
				t += ", ";
			t += n.m;
		}
		System.out.println(t);
		System.out.println(s);
		System.exit(-1);
	}

	static void test() {
		MyList list = new MyList();
		MyList list1 = new MyList();
		MyList list2 = new MyList();
		MyList list3 = new MyList();
		MyList list4 = new MyList();
		{
			int s = MyList.modifier.size(list);
			if (s != 0)
				err(list, "bad initial list size: " + s + " != 0");
		}
		{
			int i = 0;
			do {
				MyList.modifier.addBack(list, new MyStruct(i));
			} while (++i < 50);
			{
				MyStruct s = new MyStruct(i);
				MyStruct e = s;
				while (++i < 100) {
					MyStruct n = new MyStruct(i);
					MyList.modifier.link(e, n);
					e = n;
				}
				MyList.modifier.insertBefore(list, null, s, e);
			}
		}
		{
			int s = MyList.modifier.size(list);
			if (s != 100)
				err(list, "bad list size: " + s + " != 100");
		}
		{
			Object[] arr = MyList.modifier.toArray(list);
			if (arr.length != 100)
				err(list, "bad toArray(): length " + arr.length + " != 100");
		}
		{
			MyStruct[] arr = MyList.modifier.toArray(list, new MyStruct[0]);
			if (arr.length != 100)
				err(list, "bad toArray(list, arr): length " + arr.length + " != 100");
			for (int i = 0; i < arr.length; i++) {
				if (arr[i].m != i)
					err(list, "bad toArray(list, arr): [" + i + "] != " + i);
			}
			{
				MyStruct[] arr2 = new MyStruct[] {
					arr[0],
					arr[33],
					arr[55],
					arr[4],
					arr[55]
				};
				if (!MyList.modifier.containsAll(list, Arrays.asList(arr2)))
					err(list, "bad containsAll(list, arr2)");
			}
		}
		{
			int i = 100;
			MyStruct s = new MyStruct(i);
			MyStruct e = s;
			while (++i < 133) {
				MyStruct n = new MyStruct(i);
				MyList.modifier.link(e, n);
				e = n;
			}
			MyList.modifier.insertAfter(list2, null, s, e);
		}
		{
			MyStruct p = null;
			int i = 150;
			do {
				MyStruct n = new MyStruct(i);
				MyList.modifier.insertAfter(list3, p, n, n);
				p = n;
			} while (++i < 200);
		}
		{
			for (int i = 150; --i >= 110;)
				MyList.modifier.addFront(list4, new MyStruct(i));
		}
		{
			int i = 0;
			ListIterator<MyStruct> it = MyList.modifier.readIterator(list);
			if (it.previousIndex() != -1)
				err(list, "readIterator(list).previousIndex(): [" + (-1) + "] != " + (-1));
			for (; it.hasNext(); i++) {
				if (it.nextIndex() != i)
					err(list, "readIterator(list).nextIndex(): [" + (i - 1) + "] != " + i);
				if (it.previousIndex() != i - 1)
					err(list, "readIterator(list).previousIndex(): [" + (i - 1) + "] != " + (i - 1));
				{
					MyStruct n = it.next();
					if (i != n.m)
						err(list, "readIterator(list): [" + i + "] != " + n.m);
				}
			}
			if (100 != i)
				err(list, "readIterator(list): " + i + " != 100");
			if (it.nextIndex() != i)
				err(list, "readIterator(list).nextIndex(): [" + (i - 1) + "] != " + i);
		}
		{
			int i = 0;
			for (MyStruct n : MyList.modifier.readCollection(list)) {
				if (i != n.m)
					err(list, "readCollection(list): [" + i + "] != " + n.m);
				i++;
			}
			if (100 != i)
				err(list, "readCollection(list): " + i + " != 100");
		}
		{
			int i = 0;
			ListIterator<MyStruct> it = MyList.modifier.iterator(list);
			if (it.previousIndex() != i - 1)
				err(list, "iterator(list).previousIndex(): [" + (i - 1) + "] != " + (i - 1));
			for (; it.hasNext(); i++) {
				if (it.nextIndex() != i)
					err(list, "iterator(list).nextIndex(): [" + (i - 1) + "] != " + i);
				if (it.previousIndex() != i - 1)
					err(list, "iterator(list).previousIndex(): [" + (i - 1) + "] != " + (i - 1));
				{
					MyStruct n = it.next();
					if (i != n.m)
						err(list, "iterator(list): [" + i + "] != " + n.m);
				}
			}
			if (100 != i)
				err(list, "iterator(list): " + i + " != 100");
			if (it.nextIndex() != i)
				err(list, "iterator(list).nextIndex(): [" + (i - 1) + "] != " + i);
		}
		{
			int i = 0;
			for (MyStruct n : MyList.modifier.collection(list)) {
				if (i != n.m)
					err(list, "collection(list): [" + i + "] != " + n.m);
				i++;
			}
			if (100 != i)
				err(list, "collection(list): " + i + " != 100");
		}
		{
			ListIterator<MyStruct> it = MyList.modifier.readIterator(list);
			while (it.hasNext())
				it.next();
			{
				int i = 100;
				if (it.previousIndex() != i - 1)
					err(list, "readIterator(list, backward).previousIndex(): [" + (i - 1) + "] != " + (i - 1));
				while (it.hasPrevious()) {
					if (it.nextIndex() != i)
						err(list, "readIterator(list, backward).nextIndex(): [" + (i - 1) + "] != " + i);
					if (it.previousIndex() != i - 1)
						err(list, "readIterator(list, backward).previousIndex(): [" + (i - 1) + "] != " + (i - 1));
					{
						MyStruct n = it.previous();
						if (--i != n.m)
							err(list, "readIterator(list, backward): [" + i + "] != " + n.m);
					}
				}
				if (i != 0)
					err(list, "readIterator(list, backward): " + i + " != 0");
				if (it.previousIndex() != i - 1)
					err(list, "readIterator(list, backward).previousIndex(): [" + (i - 1) + "] != " + (i - 1));
			}
		}
		{
			ListIterator<MyStruct> it = MyList.modifier.iterator(list);
			while (it.hasNext())
				it.next();
			{
				int i = 100;
				if (it.previousIndex() != i - 1)
					err(list, "iterator(list, backward).previousIndex(): [" + (i - 1) + "] != " + (i - 1));
				while (it.hasPrevious()) {
					if (it.nextIndex() != i)
						err(list, "iterator(list, backward).nextIndex(): [" + (i - 1) + "] != " + i);
					if (it.previousIndex() != i - 1)
						err(list, "iterator(list, backward).previousIndex(): [" + (i - 1) + "] != " + (i - 1));
					{
						MyStruct n = it.previous();
						if (--i != n.m)
							err(list, "iterator(list, backward): [" + i + "] != " + n.m);
					}
				}
				if (i != 0)
					err(list, "iterator(list, backward): " + i + " != 0");
				if (it.previousIndex() != i - 1)
					err(list, "iterator(list, backward).previousIndex(): [" + (i - 1) + "] != " + (i - 1));
			}
		}
		MyList.modifier.move(list1, list);
		if (!MyList.modifier.isEmpty(list))
			err(list, "MyList.modifier.isEmpty(list): false");
		{
			int i = 0;
			ListIterator<MyStruct> it = MyList.modifier.readIterator(list1);
			for (; it.hasNext(); i++) {
				MyStruct n = it.next();
				if (i != n.m)
					err(list1, "readIterator(list1): [" + i + "] != " + n.m);
			}
			if (100 != i)
				err(list1, "readIterator(list1): " + i + " != 100");
		}
		MyList.modifier.addListBack(list1, list2);
		if (!MyList.modifier.isEmpty(list2))
			err(list2, "MyList.modifier.isEmpty(list2): false");
		{
			int i = 0;
			Iterator<MyStruct> it = MyList.modifier.iterator(list2);
			for (; it.hasNext(); i++)
				it.next();
			if (i != 0)
				err(list2, "MyList.modifier.iterator(list2): " + i + " != 0");
		}
		{
			int i = 0;
			Iterator<MyStruct> it = MyList.modifier.readIterator(list2);
			for (; it.hasNext(); i++)
				it.next();
			if (i != 0)
				err(list2, "MyList.modifier.readIterator(list2): " + i + " != 0");
		}
		{
			ListIterator<MyStruct> it = MyList.modifier.iterator(list2);
			if (it.hasPrevious())
				err(list2, "MyList.modifier.iterator(list2).it.hasPrevious(): true");
			if (it.hasNext())
				err(list2, "MyList.modifier.iterator(list2).it.hasNext(): true");
			{
				int i = it.previousIndex();
				if (i != -1)
					err(list2, "MyList.modifier.iterator(list2).it.previousIndex(): -1 != " + i);
			}
			{
				int i = it.nextIndex();
				if (i != 0)
					err(list2, "MyList.modifier.iterator(list2).it.nextIndex(): 0 != " + i);
			}
		}
		{
			int i = 0;
			Iterator<MyStruct> it = MyList.modifier.readIterator(list1);
			for (; it.hasNext(); i++) {
				MyStruct s = it.next();
				if (i != s.m)
					err(list1, "MyList.modifier.readIterator(list1 + list2): [" + i + "] = " + s.m);
			}
			if (133 != i)
				err(list1, "MyList.modifier.readIterator(list1 + list2)" + i + " != 133");
		}
		MyList.modifier.addListFront(list3, list4);
		if (!MyList.modifier.isEmpty(list4))
			err(list4, "MyList.modifier.isEmpty(list4): false");
		{
			int i = 110;
			Iterator<MyStruct> it = MyList.modifier.readIterator(list3);
			for (; it.hasNext(); i++) {
				MyStruct s = it.next();
				if (i != s.m)
					err(list3, "MyList.modifier.readIterator(list3 + list4): [" + i + "] = " + s.m);
			}
			if (200 != i)
				err(list3, "MyList.modifier.readIterator(list3 + list4): " + i + " != 200");
		}
		{
			MyStruct f = MyList.modifier.first(list3);
			MyList.modifier.remove(list3, f);
			{
				int i = 111;
				Iterator<MyStruct> it = MyList.modifier.readIterator(list3);
				for (; it.hasNext(); i++) {
					MyStruct s = it.next();
					if (i != s.m)
						err(list3, "MyList.modifier.readIterator(list3+list4) after remove first: [" + i + "] = " + s.m);
				}
				if (200 != i)
					err(list3, "MyList.modifier.readIterator(list3+list4) after remove first: " + i + " != 200");
			}
			MyList.modifier.restore(list3, f);
		}
		{
			int i = 110;
			Iterator<MyStruct> it = MyList.modifier.iterator(list3);
			for (; it.hasNext(); i++) {
				MyStruct s = it.next();
				if (i != s.m)
					err(list3, "MyList.modifier.iterator(list3+list4) after restore first: [" + i + "] = " + s.m);
			}
			if (200 != i)
				err(list3, "MyList.modifier.readIterator(list3+list4) after restore first: " + i + " != 200");
		}
		{
			MyStruct l = MyList.modifier.last(list3);
			MyStruct f = MyList.modifier.first(list3);
			MyList.modifier.remove(list3, f);
			MyList.modifier.replace(list3, l, f);
			{
				int i = 111;
				Iterator<MyStruct> it = MyList.modifier.readIterator(list3);
				for (; it.hasNext(); i++) {
					MyStruct s = it.next();
					if (i != s.m && !(i == 199 && 110 == s.m))
						err(list3, "MyList.modifier.readIterator(list3+list4) after replace: [" + i + "] = " + s.m);
				}
				if (200 != i)
					err(list3, "MyList.modifier.readIterator(list3+list4) after replace: " + i + " != 200");
			}
			MyList.modifier.replace(list3, f, l);
			MyList.modifier.addFront(list3, f);
		}
		{
			int i = 110;
			Iterator<MyStruct> it = MyList.modifier.iterator(list3);
			for (; it.hasNext(); i++) {
				MyStruct s = it.next();
				if (i != s.m)
					err(list3, "MyList.modifier.iterator(list3+list4) after unreplace: [" + i + "] = " + s.m);
			}
			if (200 != i)
				err(list3, "MyList.modifier.readIterator(list3+list4) after unreplace: " + i + " != 200");
		}
		MyList.modifier.insertListAfter(list3, MyList.modifier.first(list3), list1);
		if (!MyList.modifier.isEmpty(list1))
			err(list4, "MyList.modifier.isEmpty(list1): false after insertListAfter()");
		{
			int i = 0;
			Iterator<MyStruct> it = MyList.modifier.readIterator(list3);
			for (; it.hasNext(); i++) {
				MyStruct s = it.next();
				if ((i == 0 && s.m != 110) ||
					(i > 0 && i <= 133 && s.m != i - 1) ||
					(i > 133 && s.m != i - 23))
					err(list3, "MyList.modifier.readIterator(list3+list1): [" + i + "] = " + s.m);
			}
			if (223 != i)
				err(list3, "MyList.modifier.readIterator(list3+list1): " + i + " != 223");
		}
		{
			Iterator<MyStruct> it = MyList.modifier.iterator(list3);
			for (int i = 0; it.hasNext(); i++)
				it.next().m = i;
		}
		{
			Collection<MyStruct> c1 = MyList.modifier.collection(list3);
			int sz = c1.size();
			ListIterator<MyStruct> it = (ListIterator<MyStruct>)c1.iterator();
			int ff = 0;
			for (;;) {
				{
					Object o = null;
					while (it.hasNext())
						o = it.next();
					if (null == o)
						break;
				}
				it.remove(); // remove last entry
				{
					int i = ff;
					Iterator<MyStruct> it2 = MyList.modifier.readIterator(list3);
					for (; it2.hasNext(); i++) {
						MyStruct s = it2.next();
						if (i != s.m)
							err(list3, "check after removing last entry MyList.modifier.readIterator(list3): [" + i + "] = " + s.m);
					}
					if (sz - 1 != i - ff)
						err(list3, "check after removing last entry MyList.modifier.readIterator(list3): " + (i - ff) + " != " + (sz - 1));
				}
				{
					int sz2 = c1.size();
					if (sz2 != --sz)
						err(list3, "bad collection size after removing last entry: " + sz2 + " != " + sz);
				}
				if (it.nextIndex() != sz)
					err(list3, "bad it.nextIndex() after removing last entry: " + it.nextIndex() + " != " + sz);
				{
					Object o = null;
					while (it.hasPrevious())
						o = it.previous();
					if (null == o)
						break;
				}
				it.remove(); // remove first entry
				{
					int i = ++ff;
					Iterator<MyStruct> it2 = MyList.modifier.readIterator(list3);
					for (; it2.hasNext(); i++) {
						MyStruct s = it2.next();
						if (i != s.m)
							err(list3, "check after removing first entry MyList.modifier.readIterator(list3): [" + i + "] = " + s.m);
					}
					if (sz - 1 != i - ff)
						err(list3, "check after removing first entry MyList.modifier.readIterator(list3): " + (i - ff) + " != " + (sz - 1));
				}
				{
					int sz2 = c1.size();
					if (sz2 != --sz)
						err(list3, "bad collection size after removing first entry: " + sz2 + " != " + sz);
				}
				if (it.previousIndex() != -1)
					err(list3, "bad it.previousIndex() after removing first entry: " + it.previousIndex() + " != -1");
			}
		}
		if (!MyList.modifier.isEmpty(list3))
			err(list3, "MyList.modifier.isEmpty(list3): false after removing all entries");
	}

	public static void main(String args[]) {
		test();
		System.out.println("all tests ok.");
	}
}
