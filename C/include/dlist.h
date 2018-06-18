#ifndef DLIST_H_INCLUDED
#define DLIST_H_INCLUDED

/**********************************************************************************
* Embedded doubly-linked circular list
* Copyright (C) 2012-2018 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* dlist.h */

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define A_Restrict restrict
#elif defined(_MSC_VER) && (_MSC_VER >= 1600)
#define A_Restrict __restrict
#elif defined(__GNUC__) && (__GNUC__ >= 3)
#define A_Restrict __restrict__
#elif defined(__clang__)
#define A_Restrict __restrict__
#else
#define A_Restrict
#endif
#endif /* !SAL_DEFS_H_INCLUDED */

#ifndef DLIST_ASSERT
#ifdef ASSERT
#define DLIST_ASSERT(expr) ASSERT(expr)
#else
#define DLIST_ASSERT(expr) ((void)0)
#endif
#endif

#ifndef DLIST_ASSERT_PTRS
#define DLIST_ASSERT_PTRS(expr) DLIST_ASSERT(expr)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* doubly-linked list:
    --------------------------------------------------------------------
   /     entryN           dlist           entry0            entryN-1    \
   \     ------          -------          ------             ------     /
    ---> |next|---> NULL |first|--------> |next|---> ... --> |next|-----
    -----|prev| <--------|last | NULL <---|prev| <-- ... <---|prev| <---
   /     |data|          -------          |data|             |data|     \
   \     ------                           ------             ------     /
    -------------------------------------------------------------------- */

/* doubly-linked circular list:
    --------------------------------------------------------------------
   /     entryN           dlist           entry0            entryN-1    \
   \     ------          -------          ------             ------     /
    ---> |next|--------> |first|--------> |next|---> ... --> |next|-----
    -----|prev| <--------|last | <--------|prev| <-- ... <---|prev| <---
   /     |data|          -------          |data|             |data|     \
   \     ------                           ------             ------     /
    -------------------------------------------------------------------- */

/* Embedded doubly-linked list:
  one object may encapsulate multiple list entries - to reference the object from multiple lists,
  for example an object of class 'apple' may be referenced from 'fruits' and 'food' lists simultaneously:

  struct apple {
    struct dlist_entry fruit_entry;
    struct dlist_entry food_entry;
  };

  struct dlist fruits;
  struct dlist food;

  struct apple my_apple;

  dlist_add_back(&fruits, &my_apple.fruit_entry);
  dlist_add_back(&food, &my_apple.food_entry);
*/

struct dlist_entry {
	struct dlist_entry *next;
	struct dlist_entry *prev;
	/* ... user data ... */
};

/* list */
struct dlist {
	struct dlist_entry e;
};

/* first/last entry of the list */
#define dlist_first e.next
#define dlist_last  e.prev

/* check that pointer is not NULL */
#ifdef _MSC_VER
#define dlist_assert_ptr_(p) DLIST_ASSERT(p)
#else
/* do not declare 'p' as non-NULL, so gcc/clang will not complain about comparison of non-NULL pointer with 0 */
static inline void dlist_assert_ptr_(const void *p)
{
	DLIST_ASSERT(p);
}
#endif

/* initialize doubly-linked list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Out)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_init(
	struct dlist *dlist)
{
	dlist_assert_ptr_(dlist);
	dlist->dlist_first = NULL;
	dlist->dlist_last = NULL;
	return dlist;
}

/* initialize doubly-linked circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Out)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_circular_init(
	struct dlist *dlist)
{
	dlist_assert_ptr_(dlist);
	dlist->dlist_first = &dlist->e;
	dlist->dlist_last = &dlist->e;
	return dlist;
}

/* statically initialize dlist */
#define DLIST_INITIALIZER                {{NULL, NULL}}
#define DLIST_DECLARE(list)              struct dlist list = DLIST_INITIALIZER

#define DLIST_CIRCULAR_INITIALIZER(list) {{&list.e, &list.e}}
#define DLIST_DECLARE_CIRCULAR(list)     struct dlist list = DLIST_CIRCULAR_INITIALIZER(list)

/* check if list is empty */
/* note: do not checks dlist->dlist_last */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_In)
A_Ret_range(0,1)
A_Check_return
#endif
static inline int dlist_is_empty(
	const struct dlist *dlist)
{
	dlist_assert_ptr_(dlist);
	DLIST_ASSERT(!dlist->dlist_first || !dlist->dlist_first->prev);
	DLIST_ASSERT_PTRS(&dlist->e != dlist->dlist_first);
	return !dlist->dlist_first;
}

/* get end of circular dlist */
/* Note: dlist may be not valid/destroyed before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Notnull)
A_Ret_never_null
A_Ret_range(==,&dlist->e)
A_Check_return
#endif
static inline const struct dlist_entry *dlist_circular_end(
	const struct dlist *dlist)
{
	return &dlist->e;
}

/* check if circular list is empty */
/* note: do not checks dlist->dlist_last */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_In)
A_Ret_range(0,1)
A_Check_return
#endif
static inline int dlist_circular_is_empty(
	const struct dlist *dlist)
{
	dlist_assert_ptr_(dlist);
	DLIST_ASSERT(dlist->dlist_first);
	return dlist_circular_end(dlist) == dlist->dlist_first;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_In)
A_Ret_range(==,dlist)
A_Ret_never_null
A_Check_return
#endif
static inline struct dlist *dlist_const_cast_(
	const struct dlist *dlist)
{
#ifdef __cplusplus
	return const_cast<struct dlist*>(dlist);
#else
#if defined(__GNUC__) && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 2))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual" /* casting away qualifiers */
#endif
	return (struct dlist*)dlist;
#if defined(__GNUC__) && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 2))
#pragma GCC diagnostic pop
#endif
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_In)
A_Ret_range(==,dlist)
A_Ret_never_null
#endif
static inline struct dlist *dlist_check_non_circular(
	const struct dlist *dlist)
{
	dlist_assert_ptr_(dlist);
	DLIST_ASSERT(!dlist->dlist_first || !dlist->dlist_first->prev);
	DLIST_ASSERT(!dlist->dlist_last || !dlist->dlist_last->next);
	DLIST_ASSERT(!dlist->dlist_first == !dlist->dlist_last);
	DLIST_ASSERT_PTRS(&dlist->e != dlist->dlist_first);
	DLIST_ASSERT_PTRS(&dlist->e != dlist->dlist_last);
	return dlist_const_cast_(dlist);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_In)
A_Ret_range(==,dlist)
A_Ret_never_null
#endif
static inline struct dlist *dlist_check_circular(
	const struct dlist *dlist)
{
	dlist_assert_ptr_(dlist);
	DLIST_ASSERT(dlist->dlist_first && &dlist->e == dlist->dlist_first->prev);
	DLIST_ASSERT(dlist->dlist_last && &dlist->e == dlist->dlist_last->next);
	DLIST_ASSERT_PTRS((&dlist->e == dlist->dlist_first) == (&dlist->e == dlist->dlist_last));
	return dlist_const_cast_(dlist);
}

/* make dlist circular */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_make_circular(
	struct dlist *dlist)
{
	dlist_check_non_circular(dlist);
	if (dlist->dlist_first) {
		struct dlist_entry *f = dlist->dlist_first;
		struct dlist_entry *l = dlist->dlist_last;
		f->prev = &dlist->e;
		l->next = &dlist->e;
	}
	else {
		dlist->dlist_first = &dlist->e;
		dlist->dlist_last = &dlist->e;
	}
	return dlist;
}

/* make dlist non-circular */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_make_uncircular(
	struct dlist *dlist)
{
	dlist_check_circular(dlist);
	{
		struct dlist_entry *f = dlist->dlist_first;
		struct dlist_entry *l = dlist->dlist_last;
		/* note: if list is empty, then:
		  setting f->prev to NULL also resets dlist->dlist_last to NULL;
		  setting l->next to NULL also resets dlist->dlist_first to NULL */
		f->prev = NULL;
		l->next = NULL;
	}
	return dlist;
}

/* -------------- non-circular dlist methods ------------ */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_In)
A_At(c, A_In)
#endif
static inline void dlist_entry_check_non_circular(
	const struct dlist *dlist,
	const struct dlist_entry *c)
{
	(void)c;
	dlist_assert_ptr_(c);
	DLIST_ASSERT_PTRS(&dlist->e != c->next);
	DLIST_ASSERT_PTRS(&dlist->e != c->prev);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(s, A_In)
A_At(e, A_In)
#endif
static inline void dlist_check_sublist(
	const struct dlist_entry *s,
	const struct dlist_entry *e)
{
	(void)s;
	(void)e;
	dlist_assert_ptr_(s);
	dlist_assert_ptr_(e);
	DLIST_ASSERT_PTRS(s == e || s->next);
	DLIST_ASSERT_PTRS(s == e || e->prev);
}

/* insert a list of linked entries after the current entry c, if c == &dlist->e, then at front of the list */
/* NOTE: caller should set, possibly before the call, s->prev to c (or to NULL, if inserting at front of the list) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(c, A_Inout)
A_At(s, A_In)
A_At(e, A_Inout)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_insert_list_after_(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *A_Restrict e)
{
	dlist_check_non_circular(dlist);
	dlist_entry_check_non_circular(dlist, c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	{
		struct dlist_entry *A_Restrict n = c->next;
		DLIST_ASSERT_PTRS(n != c);
		DLIST_ASSERT_PTRS(n != s);
		DLIST_ASSERT_PTRS(n != e);
		c->next = s;
		e->next = n;
		if (n)
			n->prev = e;
		else
			dlist->dlist_last = e;
	}
	/*caller should do: s->prev = c or NULL;*/
	return dlist;
}

/* insert a list of linked entries before the current entry c, if c == &dlist->e, then at back of the list */
/* NOTE: caller should set, possibly before the call, e->next to c (or to NULL, if inserting at back of the list) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_In)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_insert_list_before_(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *A_Restrict s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_check_non_circular(dlist);
	dlist_entry_check_non_circular(dlist, c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	{
		struct dlist_entry *A_Restrict p = c->prev;
		DLIST_ASSERT_PTRS(p != c);
		DLIST_ASSERT_PTRS(p != s);
		DLIST_ASSERT_PTRS(p != e);
		c->prev = e;
		s->prev = p;
		if (p)
			p->next = s;
		else
			dlist->dlist_first = s;
	}
	/*caller should do: e->next = c or NULL;*/
	return dlist;
}

/* prepend a list of linked entries at front of the list */
/* NOTE: caller should set s->prev to NULL, possibly before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(s, A_In)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_add_list_front_(
	struct dlist *dlist,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	/*caller should do: s->prev = NULL;*/
	return dlist_insert_list_after_(dlist, &dlist->e, s, e);
}

/* append a list of linked entries at back of the list */
/* NOTE: caller should set e->next to NULL, possibly before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(s, A_Inout)
A_At(e, A_In)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_add_list_back_(
	struct dlist *dlist,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	/*caller should do: e->next = NULL;*/
	return dlist_insert_list_before_(dlist, &dlist->e, s, e);
}

/* prepend a list of linked entries at front of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_add_list_front(
	struct dlist *dlist,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_add_list_front_(dlist, s, e);
	s->prev = NULL;
	return dlist;
}

/* append a list of linked entries at back of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_add_list_back(
	struct dlist *dlist,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_add_list_back_(dlist, s, e);
	e->next = NULL;
	return dlist;
}

/* prepend an entry at front of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_add_front(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict e)
{
	return dlist_add_list_front(dlist, e, e);
}

/* append an entry at back of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_add_back(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict e)
{
	return dlist_add_list_back(dlist, e, e);
}

/* insert a list of linked entries after the current entry */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != &dlist->e)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_insert_list_after(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_assert_ptr_(dlist);
	DLIST_ASSERT_PTRS(&dlist->e != c);
	dlist_insert_list_after_(dlist, c, s, e);
	s->prev = c; /* if c == &dlist->e, s->prev should be NULL - use dlist_add_list_front() */
	return dlist;
}

/* insert a list of linked entries before the current entry */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != &dlist->e)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_insert_list_before(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_assert_ptr_(dlist);
	DLIST_ASSERT_PTRS(&dlist->e != c);
	dlist_insert_list_before_(dlist, c, s, e);
	e->next = c; /* if c == &dlist->e, e->next should be NULL - use dlist_add_list_back() */
	return dlist;
}

/* insert an entry after the current one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(c, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_insert_after(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *A_Restrict e)
{
	return dlist_insert_list_after(dlist, c, e, e);
}

/* insert an entry before the current one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(c, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_insert_before(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *A_Restrict e)
{
	return dlist_insert_list_before(dlist, c, e, e);
}

/* remove a list of linked entries from the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(os, A_In)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_remove_list(
	struct dlist *dlist,
	struct dlist_entry *os,
	struct dlist_entry *oe)
{
	dlist_check_non_circular(dlist);
	dlist_entry_check_non_circular(dlist, os);
	dlist_entry_check_non_circular(dlist, oe);
	{
		struct dlist_entry *A_Restrict n = oe->next;
		struct dlist_entry *A_Restrict p = os->prev;
		DLIST_ASSERT_PTRS(n != p);
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		if (p)
			p->next = n;
		else
			dlist->dlist_first = n;
		if (n)
			n->prev = p;
		else
			dlist->dlist_last = p;
	}
	return dlist;
}

/* remove an entry from the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_remove(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict oe)
{
	return dlist_remove_list(dlist, oe, oe);
}

/* restore previously removed/replaced sub-list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(os, A_In)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_restore_list_(
	struct dlist *dlist,
	struct dlist_entry *os,
	struct dlist_entry *oe)
{
	dlist_check_non_circular(dlist);
	dlist_entry_check_non_circular(dlist, os);
	dlist_entry_check_non_circular(dlist, oe);
	{
		struct dlist_entry *A_Restrict n = oe->next;
		struct dlist_entry *A_Restrict p = os->prev;
		DLIST_ASSERT_PTRS(n != p);
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		if (p)
			p->next = os;
		else
			dlist->dlist_first = os;
		if (n)
			n->prev = oe;
		else
			dlist->dlist_last = oe;
	}
	return dlist;
}

/* if there are no entries between os->prev and oe->next in the list,
  may restore previously removed sub-list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(os, A_In)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_restore_list(
	struct dlist *dlist,
	struct dlist_entry *os,
	struct dlist_entry *oe)
{
	dlist_assert_ptr_(dlist);
	dlist_assert_ptr_(os);
	dlist_assert_ptr_(oe);
	/* must be no entries between os->prev and oe->next in the list */
	DLIST_ASSERT((os->prev ? os->prev->next : dlist->dlist_first) == oe->next);
	DLIST_ASSERT((oe->next ? oe->next->prev : dlist->dlist_last) == os->prev);
	return dlist_restore_list_(dlist, os, oe);
}

/* if there are no entries between oe->prev and oe->next in the list,
  may restore previously removed entry */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_restore(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict oe)
{
	return dlist_restore_list(dlist, oe, oe);
}

/* replace old sub-list in the list with a new one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(os, A_In)
A_At(oe, A_In)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(os != s)
A_Pre_satisfies(os != e)
A_Pre_satisfies(oe != s)
A_Pre_satisfies(oe != e)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_replace_list(
	struct dlist *dlist,
	struct dlist_entry *os,
	struct dlist_entry *oe,
	struct dlist_entry *s,
	struct dlist_entry *e)
{
	dlist_check_non_circular(dlist);
	dlist_entry_check_non_circular(dlist, os);
	dlist_entry_check_non_circular(dlist, oe);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(os != s);
	DLIST_ASSERT_PTRS(os != e);
	DLIST_ASSERT_PTRS(oe != s);
	DLIST_ASSERT_PTRS(oe != e);
	{
		struct dlist_entry *A_Restrict n = oe->next;
		struct dlist_entry *A_Restrict p = os->prev;
		DLIST_ASSERT_PTRS(n != p);
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(n != s);
		DLIST_ASSERT_PTRS(n != e);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		DLIST_ASSERT_PTRS(p != s);
		DLIST_ASSERT_PTRS(p != e);
		e->next = n;
		s->prev = p;
		if (p)
			p->next = s;
		else
			dlist->dlist_first = s;
		if (n)
			n->prev = e;
		else
			dlist->dlist_last = e;
	}
	return dlist;
}

/* replace an old entry in the list with a new one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(o, A_In)
A_At(e, A_Inout)
A_Pre_satisfies(o != e)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_replace(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict o,
	struct dlist_entry *A_Restrict e)
{
	return dlist_replace_list(dlist, o, o, e, e);
}

/* move all items from src list to dst list */
/* note: initializes dst list, it may be not initialized before the call */
/* note: src list will not be changed, it will still reference moved entries after the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dst, A_Out)
A_At(src, A_In)
A_Pre_satisfies(dst != src)
#endif
static inline void dlist_move(
	struct dlist *A_Restrict dst/*initialized?*/,
	const struct dlist *A_Restrict src)
{
	dlist_check_non_circular(src);
	dlist_assert_ptr_(dst);
	DLIST_ASSERT_PTRS(dst != src);
	dst->dlist_first = src->dlist_first;
	dst->dlist_last = src->dlist_last;
}

/* -------------- circular dlist methods ------------ */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_In)
#endif
static inline void dlist_entry_check_circular(const struct dlist_entry *c)
{
	(void)c;
	dlist_assert_ptr_(c);
	DLIST_ASSERT(c->next && c->prev);
}

/* insert a list of linked entries after the current entry c of circular list, if c == &dlist->e, then at front of the list */
/* NOTE: caller should set, possibly before the call, s->prev to c */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(s, A_In)
A_At(e, A_Inout)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,c)
#endif
static inline struct dlist_entry *dlist_circular_insert_list_after_(
	struct dlist_entry *c,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *A_Restrict e)
{
	dlist_entry_check_circular(c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	{
		struct dlist_entry *n = c->next;
		/* n may be == c if circular list is empty */
		DLIST_ASSERT_PTRS(n != s);
		DLIST_ASSERT_PTRS(n != e);
		c->next = s;
		e->next = n;
		n->prev = e;
	}
	/*caller should do: s->prev = c;*/
	return c;
}

/* insert a list of linked entries before the current entry c of circular list, if c == &dlist->e, then at back of the list */
/* NOTE: caller should set, possibly before the call, e->next to c */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(s, A_In)
A_At(e, A_Inout)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,c)
#endif
static inline struct dlist_entry *dlist_circular_insert_list_before_(
	struct dlist_entry *c,
	struct dlist_entry *A_Restrict s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_entry_check_circular(c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	{
		struct dlist_entry *p = c->prev;
		/* p may be == c if circular list is empty */
		DLIST_ASSERT_PTRS(p != s);
		DLIST_ASSERT_PTRS(p != e);
		c->prev = e;
		s->prev = p;
		p->next = s;
	}
	/*caller should do: e->next = c;*/
	return c;
}

/* prepend a list of linked entries at front of circular list */
/* NOTE: caller should set s->prev to &dlist->e, possibly before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(s, A_In)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_circular_add_list_front_(
	struct dlist *dlist,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_check_circular(dlist);
	/*caller should do: s->prev = &dlist->e;*/
	dlist_circular_insert_list_after_(&dlist->e, s, e);
	return dlist;
}

/* append a list of linked entries at back of circular list */
/* NOTE: caller should set e->next to &dlist->e, possibly before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(s, A_Inout)
A_At(e, A_In)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_circular_add_list_back_(
	struct dlist *dlist,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_check_circular(dlist);
	/*caller should do: e->next = &dlist->e;*/
	dlist_circular_insert_list_before_(&dlist->e, s, e);
	return dlist;
}

/* prepend a list of linked entries at front of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_circular_add_list_front(
	struct dlist *dlist,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_circular_add_list_front_(dlist, s, e);
	s->prev = &dlist->e;
	return dlist;
}

/* append a list of linked entries at back of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_circular_add_list_back(
	struct dlist *dlist,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_circular_add_list_back_(dlist, s, e);
	e->next = &dlist->e;
	return dlist;
}

/* prepend an entry at front of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_circular_add_front(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict e)
{
	return dlist_circular_add_list_front(dlist, e, e);
}

/* append an entry at back of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlist, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist *dlist_circular_add_back(
	struct dlist *dlist,
	struct dlist_entry *A_Restrict e)
{
	return dlist_circular_add_list_back(dlist, e, e);
}

/* insert a list of linked entries after the current entry c of circular list, if c == &dlist->e, then at front of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,c)
#endif
static inline struct dlist_entry *dlist_circular_insert_list_after(
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_circular_insert_list_after_(c, s, e);
	s->prev = c;
	return c;
}

/* insert a list of linked entries before the current entry c of circular list, if c == &dlist->e, then at back of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dlist)
#endif
static inline struct dlist_entry *dlist_circular_insert_list_before(
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_circular_insert_list_before_(c, s, e);
	e->next = c;
	return c;
}

/* insert an entry after the current entry c of circular list, if c == &dlist->e, then at front of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,c)
#endif
static inline struct dlist_entry *dlist_circular_insert_after(
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *A_Restrict e)
{
	return dlist_circular_insert_list_after(c, e, e);
}

/* insert an entry before the current entry c of circular list, if c == &dlist->e, then at back of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,c)
#endif
static inline struct dlist_entry *dlist_circular_insert_before(
	struct dlist_entry *A_Restrict c,
	struct dlist_entry *A_Restrict e)
{
	return dlist_circular_insert_list_before(c, e, e);
}

/* remove a list of linked entries from circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(os, A_In)
A_At(oe, A_In)
#endif
static inline void dlist_circular_remove_list(
	struct dlist_entry *os,
	struct dlist_entry *oe)
{
	dlist_entry_check_circular(os);
	dlist_entry_check_circular(oe);
	{
		struct dlist_entry *n = oe->next;
		struct dlist_entry *p = os->prev;
		/* n may be == p if removing all entries */
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		p->next = n;
		n->prev = p;
	}
}

/* remove an entry from circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,oe)
#endif
static inline struct dlist_entry *dlist_circular_remove(
	struct dlist_entry *oe)
{
	dlist_circular_remove_list(oe, oe);
	return oe;
}

/* restore previously removed/replaced sub-list of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(os, A_In)
A_At(oe, A_In)
#endif
static inline void dlist_circular_restore_list_(
	struct dlist_entry *os,
	struct dlist_entry *oe)
{
	dlist_entry_check_circular(os);
	dlist_entry_check_circular(oe);
	{
		struct dlist_entry *n = oe->next;
		struct dlist_entry *p = os->prev;
		/* n may be == p if circular list is empty */
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		p->next = os;
		n->prev = oe;
	}
}

/* if there are no entries between os->prev and oe->next in circular list,
  may restore previously removed sub-list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(os, A_In)
A_At(oe, A_In)
#endif
static inline void dlist_circular_restore_list(
	struct dlist_entry *os,
	struct dlist_entry *oe)
{
	dlist_assert_ptr_(os);
	dlist_assert_ptr_(oe);
	/* must be no entries between os->prev and oe->next in circular list */
	DLIST_ASSERT(os->prev && os->prev->next == oe->next);
	DLIST_ASSERT(oe->next && oe->next->prev == os->prev);
	dlist_circular_restore_list_(os, oe);
}

/* if there are no entries between oe->prev and oe->next in circular list,
  may restore previously removed entry */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,oe)
#endif
static inline struct dlist_entry *dlist_circular_restore(
	struct dlist_entry *A_Restrict oe)
{
	dlist_circular_restore_list(oe, oe);
	return oe;
}

/* replace old sub-list in circular list with a new one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(os, A_In)
A_At(oe, A_In)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(os != s)
A_Pre_satisfies(os != e)
A_Pre_satisfies(oe != s)
A_Pre_satisfies(oe != e)
#endif
static inline void dlist_circular_replace_list(
	struct dlist_entry *os,
	struct dlist_entry *oe,
	struct dlist_entry *s,
	struct dlist_entry *e)
{
	dlist_entry_check_circular(os);
	dlist_entry_check_circular(oe);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(os != s);
	DLIST_ASSERT_PTRS(os != e);
	DLIST_ASSERT_PTRS(oe != s);
	DLIST_ASSERT_PTRS(oe != e);
	{
		struct dlist_entry *n = oe->next;
		struct dlist_entry *p = os->prev;
		/* n may be == p if replacing all entries */
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(n != s);
		DLIST_ASSERT_PTRS(n != e);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		DLIST_ASSERT_PTRS(p != s);
		DLIST_ASSERT_PTRS(p != e);
		{
			struct dlist_entry *A_Restrict ss = s;
			struct dlist_entry *A_Restrict nn = n;
			ss->prev = p;
			nn->prev = e;
		}
		{
			struct dlist_entry *A_Restrict pp = p;
			struct dlist_entry *A_Restrict ee = e;
			pp->next = s;
			ee->next = n;
		}
	}
}

/* replace an old entry in circular list with a new one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(o, A_In)
A_At(e, A_Inout)
A_Pre_satisfies(o != e)
#endif
static inline void dlist_circular_replace(
	struct dlist_entry *A_Restrict o,
	struct dlist_entry *A_Restrict e)
{
	dlist_circular_replace_list(o, o, e, e);
}

/* move all items from src list to dst list */
/* note: initializes dst list, it may be not initialized before the call */
/* note: src list, if empty, will be changed so src->dlist_first and src->dlist_last will point to &dst->e */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dst, A_Out)
A_At(src, A_Inout)
A_Pre_satisfies(dst != src)
#endif
static inline void dlist_circular_move(
	struct dlist *A_Restrict dst/*initialized?*/,
	struct dlist *A_Restrict src)
{
	dlist_check_circular(src);
	dlist_assert_ptr_(dst);
	DLIST_ASSERT_PTRS(dst != src);
	src->dlist_first->prev = &dst->e;
	src->dlist_last->next = &dst->e;
	/* note: if src list is empty, then:
	  setting src->dlist_first->prev to &dst->e will set src->dlist_last to &dst->e;
	  setting src->dlist_last->next to &dst->e will set src->dlist_first to &dst->e */
	dst->dlist_first = src->dlist_first;
	dst->dlist_last = src->dlist_last;
}

/* -------------- making sub-list ------------ */

/* link abcd before 123 at head:
   123 + abcd  -> abcd123
   ^h    ^s ^e    ^s       */
/* returns new head */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(h, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(s != h)
A_Pre_satisfies(e != h)
A_Ret_never_null
A_Ret_range(==,s)
#endif
static inline struct dlist_entry *dlist_entry_link_list_before(
	struct dlist_entry *A_Restrict h,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_assert_ptr_(h);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(s != h);
	DLIST_ASSERT_PTRS(e != h);
	h->prev = e;
	e->next = h;
	return s;
}

/* link abcd after 123 at tail:
   123 + abcd  -> 123abcd
     ^t  ^s ^e          ^e */
/* returns new tail */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(t, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(s != t)
A_Pre_satisfies(e != t)
A_Ret_never_null
A_Ret_range(==,e)
#endif
static inline struct dlist_entry *dlist_entry_link_list_after(
	struct dlist_entry *A_Restrict t,
	struct dlist_entry *s/*==e?*/,
	struct dlist_entry *e)
{
	dlist_assert_ptr_(t);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(s != t);
	DLIST_ASSERT_PTRS(e != t);
	t->next = s;
	s->prev = t;
	return e;
}

/* link x before 123 at head:
   123 + x  -> x123
   ^h    ^e    ^e    */
/* returns new head */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(h, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(e != h)
A_Ret_never_null
A_Ret_range(==,e)
#endif
static inline struct dlist_entry *dlist_entry_link_before(
	struct dlist_entry *A_Restrict h,
	struct dlist_entry *A_Restrict e)
{
	return dlist_entry_link_list_before(h, e, e);
}

/* link x after 123 at tail:
   123 + x  -> 123x
     ^t  ^e       ^e */
/* returns new tail */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(t, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(e != t)
A_Ret_never_null
A_Ret_range(==,e)
#endif
static inline struct dlist_entry *dlist_entry_link_after(
	struct dlist_entry *A_Restrict t,
	struct dlist_entry *A_Restrict e)
{
	return dlist_entry_link_list_after(t, e, e);
}

/* -------------- iterating over entries of non-circular dlist ------------ */

/* iterate on dlist in generic way:

  struct my_type {
    ...
    struct dlist_entry list_entry;
    ...
  };
  struct dlist_entry *e;
  dlist_iterate(dlist, e) {
    struct my_type *t = CONTAINER_OF(e, struct my_type, list_entry);
    t->...
  }

  NOTE: if iteration was not interrupted by 'break' - iterator e will be != NULL
*/

/* check that ptr is const if dlist is const */
#if defined __cplusplus && __cplusplus >= 201103L
#define DLIST_CHECK_CONSTNESS(dlist, ptr) ((dlist) + 0*sizeof(*(decltype(&(dlist)->e)*)0 = (ptr)))
#elif defined __GNUC__
#define DLIST_CHECK_CONSTNESS(dlist, ptr) ((dlist) + 0*sizeof(*(__typeof__(&(dlist)->e)*)0 = (ptr)))
#else
#define DLIST_CHECK_CONSTNESS(dlist, ptr) (dlist)
#endif

#define dlist_iterate(dlist, e) \
	for (e = dlist_check_non_circular(DLIST_CHECK_CONSTNESS(dlist, e))->dlist_first; e; e = e->next)

#define dlist_iterate_backward(dlist, e) \
	for (e = dlist_check_non_circular(DLIST_CHECK_CONSTNESS(dlist, e))->dlist_last; e; e = e->prev)

/* same as above, but allow to delete an entry referenced by iterator */

#define dlist_iterate_delete(dlist, e, n) \
	for (n = dlist_check_non_circular(DLIST_CHECK_CONSTNESS(dlist, n))->dlist_first; \
		n ? ((e = n), (n = n->next), 1) : ((e = NULL), 0);)

#define dlist_iterate_delete_backward(dlist, e, p) \
	for (p = dlist_check_non_circular(DLIST_CHECK_CONSTNESS(dlist, p))->dlist_last; \
		p ? ((e = p), (p = p->prev), 1) : ((e = NULL), 0);)

/* -------------- iterating over entries of circular dlist ------------ */

#define dlist_circular_iterate(dlist, e) \
	for (e = dlist_check_circular(DLIST_CHECK_CONSTNESS(dlist, e))->dlist_first; \
		dlist_circular_end(dlist) != e ? 1 : ((e = NULL), 0); e = e->next)

#define dlist_circular_iterate_backward(dlist, e) \
	for (e = dlist_check_circular(DLIST_CHECK_CONSTNESS(dlist, e))->dlist_last; \
		dlist_circular_end(dlist) != e ? 1 : ((e = NULL), 0); e = e->prev)

/* same as above, but allow to delete an entry referenced by iterator */

#define dlist_circular_iterate_delete(dlist, e, n) \
	for (n = dlist_check_circular(DLIST_CHECK_CONSTNESS(dlist, n))->dlist_first; \
		dlist_circular_end(dlist) != n ? ((e = n), (n = n->next), 1) : ((e = NULL), 0);)

#define dlist_circular_iterate_delete_backward(dlist, e, p) \
	for (p = dlist_check_circular(DLIST_CHECK_CONSTNESS(dlist, p))->dlist_last; \
		dlist_circular_end(dlist) != p ? ((e = p), (p = p->prev), 1) : ((e = NULL), 0);)

#ifdef __cplusplus
}
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#undef A_Restrict
#endif

#endif /* DLIST_H_INCLUDED */
