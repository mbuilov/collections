#ifndef DLIST_H_INCLUDED
#define DLIST_H_INCLUDED

/**********************************************************************************
* Embedded doubly-linked circular list
* Copyright (C) 2012-2019 Michael M. Builov, https://github.com/mbuilov/collections
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

/* compare pointers */
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
	struct dlist_entry *next; /* never NULL for circular dlist */
	struct dlist_entry *prev; /* never NULL for circular dlist */
	/* ... user data ... */
};

/* doubly-linked list */
struct dlist {
	struct dlist_entry e;
};

/* first/last entry of list */
#define dlist_first e.next
#define dlist_last  e.prev

/* doubly-linked circular list */
struct dlist_circular {
	struct dlist l;       /* &l.e is the end of the list */
};

/* first/last entry of circular list */
#define dlist_circular_first l.e.next
#define dlist_circular_last  l.e.prev

/* check that pointer is not NULL */
#ifdef _MSC_VER
#define dlist_assert_ptr_(p) DLIST_ASSERT(p)
#else
/* do not declare 'p' as non-NULL, so gcc/clang will not complain about comparison of non-NULL pointer with 0 */
#if (defined(__GNUC__) && (__GNUC__ >= 4)) || \
  (defined(__clang__) && (__clang_major__ > 3 || (3 == __clang_major__  && __clang_minor__ >= 7)))
__attribute__ ((pure))
__attribute__ ((always_inline))
#endif
static inline void dlist_assert_ptr_(const void *const p)
{
	DLIST_ASSERT(p);
}
#endif

/* initialize doubly-linked list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Out)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_init(
	struct dlist *const dl)
{
	dlist_assert_ptr_(dl);
	dl->dlist_first = NULL;
	dl->dlist_last = NULL;
	return dl;
}

/* initialize doubly-linked circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_Out)
A_Ret_never_null
A_Ret_range(==,dlc)
A_Ret_valid
#endif
static inline struct dlist_circular *dlist_circular_init(
	struct dlist_circular *const dlc)
{
	dlist_assert_ptr_(dlc);
	dlc->dlist_circular_first = &dlc->l.e;
	dlc->dlist_circular_last = &dlc->l.e;
	return dlc;
}

/* statically initialize doubly-linked list */
#define DLIST_INITIALIZER               {{NULL, NULL}}
#define DLIST_DECLARE(dl)               struct dlist dl = DLIST_INITIALIZER

/* statically initialize doubly-linked circular list */
#define DLIST_CIRCULAR_INITIALIZER(dlc) {{{&dlc.l.e, &dlc.l.e}}}
#define DLIST_DECLARE_CIRCULAR(dlc)     struct dlist_circular dlc = DLIST_CIRCULAR_INITIALIZER(dlc)

/* check if doubly-linked list is empty */
/* note: do not checks dl->dlist_last */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_In)
A_Ret_range(0,1)
A_Check_return
#endif
static inline int dlist_is_empty(
	const struct dlist *const dl)
{
	dlist_assert_ptr_(dl);
	DLIST_ASSERT(!dl->dlist_first || !dl->dlist_first->prev);
	DLIST_ASSERT_PTRS(&dl->e != dl->dlist_first);
	return !dl->dlist_first;
}

/* get end of doubly-linked circular list */
/* note: dlc may be not valid/destroyed before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_In)
A_Ret_never_null
A_Ret_range(==,&dlc->l.e)
A_Ret_valid
A_Check_return
#endif
#ifdef __GNUC__
__attribute__ ((pure))
#endif
static inline const struct dlist_entry *dlist_circular_end(
	const struct dlist_circular *const dlc)
{
	dlist_assert_ptr_(dlc);
	return &dlc->l.e;
}

/* check if doubly-linked circular list is empty */
/* note: do not checks dlc->dlist_circular_last */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_In)
A_Ret_range(0,1)
A_Check_return
#endif
static inline int dlist_circular_is_empty(
	const struct dlist_circular *const dlc)
{
	dlist_assert_ptr_(dlc);
	DLIST_ASSERT(dlc->dlist_circular_first);
	return dlist_circular_end(dlc) == dlc->dlist_circular_first;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_Check_return
A_At(dl, A_In_opt)
A_When(dl, A_Ret_valid)
A_When(!dl, A_Ret_null)
A_Ret_range(==,dl)
#endif
static inline struct dlist *dlist_const_cast_(
	const struct dlist *const dl/*NULL?*/)
{
#ifdef __cplusplus
	return const_cast<struct dlist*>(dl);
#else
#if defined(__GNUC__) && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual" /* casting away qualifiers */
#endif
	return (struct dlist*)dl;
#if defined(__GNUC__) && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic pop
#endif
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Const_function
A_Check_return
A_At(dlc, A_In_opt)
A_When(dlc, A_Ret_valid)
A_When(!dlc, A_Ret_null)
A_Ret_range(==,dlc)
#endif
static inline struct dlist_circular *dlist_circular_const_cast_(
	const struct dlist_circular *const dlc/*NULL?*/)
{
#ifdef __cplusplus
	return const_cast<struct dlist_circular*>(dlc);
#else
#if defined(__GNUC__) && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual" /* casting away qualifiers */
#endif
	return (struct dlist_circular*)dlc;
#if defined(__GNUC__) && (__GNUC__ > 4 || (4 == __GNUC__ && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic pop
#endif
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_In)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_check_non_circular(
	const struct dlist *const dl)
{
	dlist_assert_ptr_(dl);
	DLIST_ASSERT(!dl->dlist_first || !dl->dlist_first->prev);
	DLIST_ASSERT(!dl->dlist_last || !dl->dlist_last->next);
	DLIST_ASSERT(!dl->dlist_first == !dl->dlist_last);
	DLIST_ASSERT_PTRS(&dl->e != dl->dlist_first);
	DLIST_ASSERT_PTRS(&dl->e != dl->dlist_last);
	return dlist_const_cast_(dl);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_In)
A_Ret_never_null
A_Ret_range(==,dlc)
A_Ret_valid
#endif
#if (defined(__GNUC__) && (__GNUC__ >= 4)) || \
  (defined(__clang__) && (__clang_major__ > 3 || (3 == __clang_major__  && __clang_minor__ >= 7)))
__attribute__ ((pure))
#endif
static inline struct dlist_circular *dlist_check_circular(
	const struct dlist_circular *const dlc)
{
	dlist_assert_ptr_(dlc);
	DLIST_ASSERT(dlc->dlist_circular_first && &dlc->l.e == dlc->dlist_circular_first->prev);
	DLIST_ASSERT(dlc->dlist_circular_last && &dlc->l.e == dlc->dlist_circular_last->next);
	DLIST_ASSERT_PTRS((&dlc->l.e == dlc->dlist_circular_first) == (&dlc->l.e == dlc->dlist_circular_last));
	return dlist_circular_const_cast_(dlc);
}

/* make doubly-linked list circular */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist_circular *dlist_make_circular(
	struct dlist *const dl)
{
	dlist_check_non_circular(dl);
	if (dl->dlist_first) {
		struct dlist_entry *const f = dl->dlist_first;
		struct dlist_entry *const l = dl->dlist_last;
		f->prev = &dl->e;
		l->next = &dl->e;
	}
	else {
		dl->dlist_first = &dl->e;
		dl->dlist_last = &dl->e;
	}
	{
		void *const p = dl;
		return (struct dlist_circular*)p;
	}
}

/* make doubly-linked circular list non-circular */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlc)
A_Ret_valid
#endif
static inline struct dlist *dlist_make_uncircular(
	struct dlist_circular *const dlc)
{
	(void)dlist_check_circular(dlc);
	{
		struct dlist_entry *const f = dlc->dlist_circular_first;
		struct dlist_entry *const l = dlc->dlist_circular_last;
		/* note: if list is empty, then:
		  setting f->prev to NULL also resets dlc->dlist_circular_last to NULL;
		  setting l->next to NULL also resets dlc->dlist_circular_first to NULL */
		f->prev = NULL;
		l->next = NULL;
	}
	{
		void *const p = dlc;
		return (struct dlist*)p;
	}
}

/* check doubly-linked sub-list: s<=>a<=>...<=>b<=>e */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(s, A_In)
A_At(e, A_In)
#endif
#if (defined(__GNUC__) && (__GNUC__ >= 4)) || \
  (defined(__clang__) && (__clang_major__ > 3 || (3 == __clang_major__  && __clang_minor__ >= 7)))
__attribute__ ((pure))
#endif
static inline void dlist_check_sublist(
	const struct dlist_entry *const s/*==e?*/,
	const struct dlist_entry *const e)
{
	(void)s;
	(void)e;
	dlist_assert_ptr_(s);
	dlist_assert_ptr_(e);
	DLIST_ASSERT_PTRS(s == e || s->next);
	DLIST_ASSERT_PTRS(s == e || e->prev);
}

/* -------------- non-circular dlist methods ------------ */

/* check that 'c' is not an entry of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_In)
A_At(c, A_In)
#endif
static inline void dlist_entry_check_non_circular(
	const struct dlist *const dl,
	const struct dlist_entry *const c)
{
	(void)c;
	dlist_assert_ptr_(c);
	DLIST_ASSERT_PTRS(&dl->e != c->next);
	DLIST_ASSERT_PTRS(&dl->e != c->prev);
}

/* insert a list of linked entries after the current entry c, if c == &dl->e, then at front of the list */
/* NOTE: caller should set, possibly before the call, s->prev to c (or to NULL, if inserting at front of the list) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(c, A_Inout)
A_At(s, A_In)
A_At(e, A_Inout)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_insert_list_after_(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_check_non_circular(dl);
	dlist_entry_check_non_circular(dl, c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	{
		struct dlist_entry *A_Restrict const n = c->next;
		DLIST_ASSERT_PTRS(n != c);
		DLIST_ASSERT_PTRS(n != s);
		DLIST_ASSERT_PTRS(n != e);
		c->next = s;
		e->next = n;
		if (n)
			n->prev = e;
		else
			dl->dlist_last = e;
	}
	/*caller should do: s->prev = c or NULL;*/
	return dl;
}

/* insert a list of linked entries before the current entry c, if c == &dl->e, then at back of the list */
/* NOTE: caller should set, possibly before the call, e->next to c (or to NULL, if inserting at back of the list) */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_In)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_insert_list_before_(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_check_non_circular(dl);
	dlist_entry_check_non_circular(dl, c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	{
		struct dlist_entry *A_Restrict const p = c->prev;
		DLIST_ASSERT_PTRS(p != c);
		DLIST_ASSERT_PTRS(p != s);
		DLIST_ASSERT_PTRS(p != e);
		c->prev = e;
		s->prev = p;
		if (p)
			p->next = s;
		else
			dl->dlist_first = s;
	}
	/*caller should do: e->next = c or NULL;*/
	return dl;
}

/* prepend a list of linked entries at front of the list */
/* NOTE: caller should set s->prev to NULL, possibly before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(s, A_In)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_add_list_front_(
	struct dlist *const dl,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	/*caller should do: s->prev = NULL;*/
	return dlist_insert_list_after_(dl, &dl->e, s, e);
}

/* append a list of linked entries at back of the list */
/* NOTE: caller should set e->next to NULL, possibly before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(s, A_Inout)
A_At(e, A_In)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_add_list_back_(
	struct dlist *const dl,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	/*caller should do: e->next = NULL;*/
	return dlist_insert_list_before_(dl, &dl->e, s, e);
}

/* prepend a list of linked entries at front of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_add_list_front(
	struct dlist *const dl,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_add_list_front_(dl, s, e);
	s->prev = NULL;
	return dl;
}

/* append a list of linked entries at back of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_add_list_back(
	struct dlist *const dl,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_add_list_back_(dl, s, e);
	e->next = NULL;
	return dl;
}

/* prepend an entry at front of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_add_front(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const e)
{
	return dlist_add_list_front(dl, e, e);
}

/* append an entry at back of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_add_back(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const e)
{
	return dlist_add_list_back(dl, e, e);
}

/* insert a list of linked entries after the current entry */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != &dl->e)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_insert_list_after(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_assert_ptr_(dl);
	DLIST_ASSERT_PTRS(&dl->e != c);
	dlist_insert_list_after_(dl, c, s, e);
	s->prev = c; /* if c == &dl->e, s->prev should be NULL - use dlist_add_list_front() instead */
	return dl;
}

/* insert a list of linked entries before the current entry */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != &dl->e)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_insert_list_before(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_assert_ptr_(dl);
	DLIST_ASSERT_PTRS(&dl->e != c);
	dlist_insert_list_before_(dl, c, s, e);
	e->next = c; /* if c == &dl->e, e->next should be NULL - use dlist_add_list_back() instead */
	return dl;
}

/* insert an entry after the current one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(c, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_insert_after(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *A_Restrict const e)
{
	return dlist_insert_list_after(dl, c, e, e);
}

/* insert an entry before the current one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(c, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_insert_before(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *A_Restrict const e)
{
	return dlist_insert_list_before(dl, c, e, e);
}

/* remove a list of linked entries from the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(os, A_In)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_remove_list(
	struct dlist *const dl,
	struct dlist_entry *const os/*==oe?*/,
	struct dlist_entry *const oe)
{
	dlist_check_non_circular(dl);
	dlist_entry_check_non_circular(dl, os);
	dlist_entry_check_non_circular(dl, oe);
	{
		struct dlist_entry *A_Restrict const n = oe->next;
		struct dlist_entry *A_Restrict const p = os->prev;
		DLIST_ASSERT_PTRS(n != p);
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		if (p)
			p->next = n;
		else
			dl->dlist_first = n;
		if (n)
			n->prev = p;
		else
			dl->dlist_last = p;
	}
	return dl;
}

/* remove an entry from the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_remove(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const oe)
{
	return dlist_remove_list(dl, oe, oe);
}

/* restore previously removed/replaced sub-list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(os, A_In)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_restore_list_(
	struct dlist *const dl,
	struct dlist_entry *const os/*==oe?*/,
	struct dlist_entry *const oe)
{
	dlist_check_non_circular(dl);
	dlist_entry_check_non_circular(dl, os);
	dlist_entry_check_non_circular(dl, oe);
	{
		struct dlist_entry *A_Restrict const n = oe->next;
		struct dlist_entry *A_Restrict const p = os->prev;
		DLIST_ASSERT_PTRS(n != p);
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		if (p)
			p->next = os;
		else
			dl->dlist_first = os;
		if (n)
			n->prev = oe;
		else
			dl->dlist_last = oe;
	}
	return dl;
}

/* if there are no entries between os->prev and oe->next in the list,
  may restore previously removed sub-list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(os, A_In)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_restore_list(
	struct dlist *const dl,
	struct dlist_entry *const os/*==oe?*/,
	struct dlist_entry *const oe)
{
	dlist_assert_ptr_(dl);
	dlist_assert_ptr_(os);
	dlist_assert_ptr_(oe);
	/* must be no entries between os->prev and oe->next in the list */
	DLIST_ASSERT((os->prev ? os->prev->next : dl->dlist_first) == oe->next);
	DLIST_ASSERT((oe->next ? oe->next->prev : dl->dlist_last) == os->prev);
	return dlist_restore_list_(dl, os, oe);
}

/* if there are no entries between oe->prev and oe->next in the list,
  may restore previously removed entry */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(oe, A_In)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_restore(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const oe)
{
	return dlist_restore_list(dl, oe, oe);
}

/* replace old sub-list in the list with a new one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(os, A_In)
A_At(oe, A_In)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(os != s)
A_Pre_satisfies(os != e)
A_Pre_satisfies(oe != s)
A_Pre_satisfies(oe != e)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_replace_list(
	struct dlist *const dl,
	struct dlist_entry *const os/*==oe?*/,
	struct dlist_entry *const oe,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_check_non_circular(dl);
	dlist_entry_check_non_circular(dl, os);
	dlist_entry_check_non_circular(dl, oe);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(os != s);
	DLIST_ASSERT_PTRS(os != e);
	DLIST_ASSERT_PTRS(oe != s);
	DLIST_ASSERT_PTRS(oe != e);
	{
		struct dlist_entry *A_Restrict const n = oe->next;
		struct dlist_entry *A_Restrict const p = os->prev;
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
			dl->dlist_first = s;
		if (n)
			n->prev = e;
		else
			dl->dlist_last = e;
	}
	return dl;
}

/* replace an old entry in the list with a new one */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dl, A_Inout)
A_At(o, A_In)
A_At(e, A_Inout)
A_Pre_satisfies(o != e)
A_Ret_never_null
A_Ret_range(==,dl)
A_Ret_valid
#endif
static inline struct dlist *dlist_replace(
	struct dlist *const dl,
	struct dlist_entry *A_Restrict const o,
	struct dlist_entry *A_Restrict const e)
{
	return dlist_replace_list(dl, o, o, e, e);
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
	struct dlist *A_Restrict const dst/*initialized?*/,
	const struct dlist *A_Restrict const src)
{
	dlist_check_non_circular(src);
	dlist_assert_ptr_(dst);
	DLIST_ASSERT_PTRS(dst != src);
	dst->dlist_first = src->dlist_first;
	dst->dlist_last = src->dlist_last;
}

/* -------------- circular dlist methods ------------ */

/* check that 'c' is an entry of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_In)
#endif
#if (defined(__GNUC__) && (__GNUC__ >= 4)) || \
  (defined(__clang__) && (__clang_major__ > 3 || (3 == __clang_major__  && __clang_minor__ >= 7)))
__attribute__ ((pure))
#endif
static inline void dlist_entry_check_circular(const struct dlist_entry *const c)
{
	(void)c;
	dlist_assert_ptr_(c);
	DLIST_ASSERT(c->next && c->prev);
}

/* insert a list of linked entries after the current entry c of circular list, if c == &dlc->l.e, then at front of the list */
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
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_circular_insert_list_after_(
	struct dlist_entry *const c,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_entry_check_circular(c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	{
		struct dlist_entry *const n = c->next;
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

/* insert a list of linked entries before the current entry c of circular list, if c == &dlc->l.e, then at back of the list */
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
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_circular_insert_list_before_(
	struct dlist_entry *const c,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_entry_check_circular(c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	{
		struct dlist_entry *const p = c->prev;
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
/* NOTE: caller should set s->prev to &dlc->l.e, possibly before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_Inout)
A_At(s, A_In)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlc)
A_Ret_valid
#endif
static inline struct dlist_circular *dlist_circular_add_list_front_(
	struct dlist_circular *const dlc,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	(void)dlist_check_circular(dlc);
	/*caller should do: s->prev = &dlc->l.e;*/
	dlist_circular_insert_list_after_(&dlc->l.e, s, e);
	return dlc;
}

/* append a list of linked entries at back of circular list */
/* NOTE: caller should set e->next to &dlc->l.e, possibly before the call */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_Inout)
A_At(s, A_Inout)
A_At(e, A_In)
A_Ret_never_null
A_Ret_range(==,dlc)
A_Ret_valid
#endif
static inline struct dlist_circular *dlist_circular_add_list_back_(
	struct dlist_circular *const dlc,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	(void)dlist_check_circular(dlc);
	/*caller should do: e->next = &dlc->l.e;*/
	dlist_circular_insert_list_before_(&dlc->l.e, s, e);
	return dlc;
}

/* prepend a list of linked entries at front of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlc)
A_Ret_valid
#endif
static inline struct dlist_circular *dlist_circular_add_list_front(
	struct dlist_circular *const dlc,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_circular_add_list_front_(dlc, s, e);
	s->prev = &dlc->l.e; /* dlist_circular_end(dlc) */
	return dlc;
}

/* append a list of linked entries at back of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlc)
A_Ret_valid
#endif
static inline struct dlist_circular *dlist_circular_add_list_back(
	struct dlist_circular *const dlc,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_circular_add_list_back_(dlc, s, e);
	e->next = &dlc->l.e; /* dlist_circular_end(dlc) */
	return dlc;
}

/* prepend an entry at front of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlc)
A_Ret_valid
#endif
static inline struct dlist_circular *dlist_circular_add_front(
	struct dlist_circular *const dlc,
	struct dlist_entry *A_Restrict const e)
{
	return dlist_circular_add_list_front(dlc, e, e);
}

/* append an entry at back of circular list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dlc, A_Inout)
A_At(e, A_Inout)
A_Ret_never_null
A_Ret_range(==,dlc)
A_Ret_valid
#endif
static inline struct dlist_circular *dlist_circular_add_back(
	struct dlist_circular *const dlc,
	struct dlist_entry *A_Restrict const e)
{
	return dlist_circular_add_list_back(dlc, e, e);
}

/* insert a list of linked entries after the current entry c of circular list, if c == &dlc->e, then at front of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,c)
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_circular_insert_list_after(
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_circular_insert_list_after_(c, s, e);
	s->prev = c;
	return c;
}

/* insert a list of linked entries before the current entry c of circular list, if c == &dlc->e, then at back of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(s, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != s)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,c)
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_circular_insert_list_before(
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_circular_insert_list_before_(c, s, e);
	e->next = c;
	return c;
}

/* insert an entry after the current entry c of circular list, if c == &dlc->e, then at front of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,c)
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_circular_insert_after(
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *A_Restrict const e)
{
	return dlist_circular_insert_list_after(c, e, e);
}

/* insert an entry before the current entry c of circular list, if c == &dlc->e, then at back of the list */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(c, A_Inout)
A_At(e, A_Inout)
A_Pre_satisfies(c != e)
A_Ret_never_null
A_Ret_range(==,c)
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_circular_insert_before(
	struct dlist_entry *A_Restrict const c,
	struct dlist_entry *A_Restrict const e)
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
	struct dlist_entry *const os/*==oe?*/,
	struct dlist_entry *const oe)
{
	dlist_entry_check_circular(os);
	dlist_entry_check_circular(oe);
	{
		struct dlist_entry *const n = oe->next;
		struct dlist_entry *const p = os->prev;
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
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_circular_remove(
	struct dlist_entry *const oe)
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
	struct dlist_entry *const os/*==oe?*/,
	struct dlist_entry *const oe)
{
	dlist_entry_check_circular(os);
	dlist_entry_check_circular(oe);
	{
		struct dlist_entry *const n = oe->next;
		struct dlist_entry *const p = os->prev;
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
	struct dlist_entry *const os/*==oe?*/,
	struct dlist_entry *const oe)
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
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_circular_restore(
	struct dlist_entry *A_Restrict const oe)
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
	struct dlist_entry *const os/*==oe?*/,
	struct dlist_entry *const oe,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
{
	dlist_entry_check_circular(os);
	dlist_entry_check_circular(oe);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(os != s);
	DLIST_ASSERT_PTRS(os != e);
	DLIST_ASSERT_PTRS(oe != s);
	DLIST_ASSERT_PTRS(oe != e);
	{
		struct dlist_entry *const n = oe->next;
		struct dlist_entry *const p = os->prev;
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
			struct dlist_entry *A_Restrict const ss = s;
			struct dlist_entry *A_Restrict const nn = n;
			ss->prev = p;
			nn->prev = e;
		}
		{
			struct dlist_entry *A_Restrict const pp = p;
			struct dlist_entry *A_Restrict const ee = e;
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
	struct dlist_entry *A_Restrict const o,
	struct dlist_entry *A_Restrict const e)
{
	dlist_circular_replace_list(o, o, e, e);
}

/* move all items from src list to dst list */
/* note: initializes dst list, it may be not initialized before the call */
/* note: src list, if empty, will be changed so src->dlist_circular_first and src->dlist_circular_last will point to &dst->l.e */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(dst, A_Out)
A_At(src, A_Inout)
A_Pre_satisfies(dst != src)
#endif
static inline void dlist_circular_move(
	struct dlist_circular *A_Restrict const dst/*initialized?*/,
	struct dlist_circular *A_Restrict const src)
{
	(void)dlist_check_circular(src);
	dlist_assert_ptr_(dst);
	DLIST_ASSERT_PTRS(dst != src);
	src->dlist_circular_first->prev = &dst->l.e;
	src->dlist_circular_last->next = &dst->l.e;
	/* note: if src list is empty, then:
	  setting src->dlist_circular_first->prev to &dst->l.e will set src->dlist_circular_last to &dst->l.e;
	  setting src->dlist_circular_last->next to &dst->l.e will set src->dlist_circular_first to &dst->l.e */
	dst->dlist_circular_first = src->dlist_circular_first;
	dst->dlist_circular_last = src->dlist_circular_last;
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
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_entry_link_list_before(
	struct dlist_entry *A_Restrict const h,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
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
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_entry_link_list_after(
	struct dlist_entry *A_Restrict const t,
	struct dlist_entry *const s/*==e?*/,
	struct dlist_entry *const e)
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
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_entry_link_before(
	struct dlist_entry *A_Restrict const h,
	struct dlist_entry *A_Restrict const e)
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
A_Ret_valid
#endif
static inline struct dlist_entry *dlist_entry_link_after(
	struct dlist_entry *A_Restrict const t,
	struct dlist_entry *A_Restrict const e)
{
	return dlist_entry_link_list_after(t, e, e);
}

/* -------------- iterating over entries of non-circular doubly-linked list ------------ */

/* iterate on doubly-linked list in generic way:

  struct my_type {
    ...
    struct dlist_entry list_entry;
    ...
  };
  struct dlist_entry *e;
  dlist_iterate(dl, e) {
    struct my_type *t = CONTAINER_OF(e, struct my_type, list_entry);
    t->...
  }

  NOTE: if iteration was not interrupted by 'break' - iterator e will be != NULL
*/

/* check that ptr is const if dlist is const */
#if defined __cplusplus && __cplusplus >= 201103L
#define DLIST_CHECK_CONSTNESS(dl, ptr)           ((dl) + 0*sizeof(*(decltype(&(dl)->e)*)0 = (ptr)))
#define DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, ptr) ((dlc) + 0*sizeof(*(decltype(&(dlc)->l.e)*)0 = (ptr)))
#elif defined __GNUC__
#define DLIST_CHECK_CONSTNESS(dl, ptr)           ((dl) + 0*sizeof(*(__typeof__(&(dl)->e)*)0 = (ptr)))
#define DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, ptr) ((dlc) + 0*sizeof(*(__typeof__(&(dlc)->l.e)*)0 = (ptr)))
#else
#define DLIST_CHECK_CONSTNESS(dl, ptr)           (dl)
#define DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, ptr) (dlc)
#endif

#define dlist_iterate(dl, e) \
	for (e = dlist_check_non_circular(DLIST_CHECK_CONSTNESS(dl, e))->dlist_first; e; e = e->next)

#define dlist_iterate_backward(dl, e) \
	for (e = dlist_check_non_circular(DLIST_CHECK_CONSTNESS(dl, e))->dlist_last; e; e = e->prev)

/* same as above, but allow to delete an entry referenced by the iterator */

#define dlist_iterate_delete(dl, e, n) \
	for (n = dlist_check_non_circular(DLIST_CHECK_CONSTNESS(dl, n))->dlist_first; \
		n ? ((e = n), (n = n->next), 1) : ((e = NULL), 0);)

#define dlist_iterate_delete_backward(dl, e, p) \
	for (p = dlist_check_non_circular(DLIST_CHECK_CONSTNESS(dl, p))->dlist_last; \
		p ? ((e = p), (p = p->prev), 1) : ((e = NULL), 0);)

/* -------------- iterating over entries of circular doubly-linked list ------------ */

#define dlist_circular_iterate(dlc, e) \
	for (e = dlist_check_circular(DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, e))->dlist_circular_first; \
		dlist_circular_end(dlc) != e ? 1 : ((e = NULL), 0); e = e->next)

#define dlist_circular_iterate_backward(dlc, e) \
	for (e = dlist_check_circular(DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, e))->dlist_circular_last; \
		dlist_circular_end(dlc) != e ? 1 : ((e = NULL), 0); e = e->prev)

/* same as above, but allow to delete an entry referenced by the iterator */

#define dlist_circular_iterate_delete(dlc, e, n) \
	for (n = dlist_check_circular(DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, n))->dlist_circular_first; \
		dlist_circular_end(dlc) != n ? ((e = n), (n = n->next), 1) : ((e = NULL), 0);)

#define dlist_circular_iterate_delete_backward(dlc, e, p) \
	for (p = dlist_check_circular(DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, p))->dlist_circular_last; \
		dlist_circular_end(dlc) != p ? ((e = p), (p = p->prev), 1) : ((e = NULL), 0);)

#ifdef __cplusplus
}
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#undef A_Restrict
#endif

#endif /* DLIST_H_INCLUDED */
