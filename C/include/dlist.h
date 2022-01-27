#ifndef DLIST_H_INCLUDED
#define DLIST_H_INCLUDED

/**********************************************************************************
* Embedded doubly-linked circular list
* Copyright (C) 2012-2022 Michael M. Builov, https://github.com/mbuilov/collections
* Licensed under LGPL version 3 or any later version, see COPYING
**********************************************************************************/

/* dlist.h */

/* DLIST_RESTRICT - annotates pointer pointing to memory that is not writable via other pointers */
#ifndef DLIST_RESTRICT
#ifdef A_Restrict
#define DLIST_RESTRICT A_Restrict
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define DLIST_RESTRICT restrict
#elif defined(_MSC_VER) && (_MSC_VER >= 1600)
#define DLIST_RESTRICT __restrict
#elif defined(__GNUC__) && (__GNUC__ >= 3)
#define DLIST_RESTRICT __restrict__
#elif defined(__clang__)
#define DLIST_RESTRICT __restrict__
#else
#define DLIST_RESTRICT
#endif
#endif

/* DLIST_ASSUME - assume condition is always true */
#ifndef DLIST_ASSUME
#ifdef ASSUME
#define DLIST_ASSUME(cond) ASSUME(cond)
#elif defined _MSC_VER
#define DLIST_ASSUME(cond) __assume(!!(cond))
#elif defined __clang_analyzer__
#define DLIST_ASSUME(cond) ((void)(!(cond) ? __builtin_unreachable(), 0 : 1))
#elif defined __clang__
#define DLIST_ASSUME(cond) __builtin_assume(!!(cond))
#elif defined __INTEL_COMPILER
#define DLIST_ASSUME(cond) ((void)0) /* ICC compiles calls to __builtin_unreachable() as jumps somewhere... */
#elif defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define DLIST_ASSUME(cond) ((void)(!(cond) ? __builtin_unreachable(), 0 : 1))
#else
#define DLIST_ASSUME(cond) ((void)0) /* assume condition is always true */
#endif
#endif

/* expr - do not compares pointers */
#ifndef DLIST_ASSERT
#ifdef ASSERT
#define DLIST_ASSERT(expr) ASSERT(expr)
#else
#define DLIST_ASSERT(expr) DLIST_ASSUME(expr)
#endif
#endif

/* check that pointer is not NULL */
#ifndef DLIST_ASSERT_PTR
#ifdef ASSERT_PTR
#define DLIST_ASSERT_PTR(ptr) ASSERT_PTR(ptr)
#else
#define DLIST_ASSERT_PTR(ptr) DLIST_ASSERT(ptr)
#endif
#endif

/* expr - may compare pointers for equality */
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

/* initialize doubly-linked list */
static inline struct dlist *dlist_init(
	struct dlist *const dl/*!=NULL*/)
{
	DLIST_ASSERT_PTR(dl);
	dl->dlist_first = NULL;
	dl->dlist_last = NULL;
	return dl;
}

/* initialize doubly-linked circular list */
static inline struct dlist_circular *dlist_circular_init(
	struct dlist_circular *const dlc/*!=NULL*/)
{
	DLIST_ASSERT_PTR(dlc);
	dlc->dlist_circular_first = &dlc->l.e;
	dlc->dlist_circular_last = &dlc->l.e;
	return dlc;
}

/* statically initialize doubly-linked list */
#define DLIST_INITIALIZER               {{NULL, NULL}}
#define DLIST_DECLARE(dl)               struct dlist dl = DLIST_INITIALIZER

/* statically initialize doubly-linked circular list */
#define DLIST_CIRCULAR_INITIALIZER(dlc) {{{&dlc.l.e, &dlc.l.e}}}
#define DLIST_CIRCULAR_DECLARE(dlc)     struct dlist_circular dlc = DLIST_CIRCULAR_INITIALIZER(dlc)

/* check if doubly-linked list is empty */
/* note: do not checks dl->dlist_last */
static inline int dlist_is_empty(
	const struct dlist *const dl/*!=NULL*/)
{
	DLIST_ASSERT_PTR(dl);
	DLIST_ASSERT(!dl->dlist_first || !dl->dlist_first->prev);
	DLIST_ASSERT_PTRS(&dl->e != dl->dlist_first);
	return !dl->dlist_first;
}

/* get end of doubly-linked circular list */
/* note: dlc may be not valid/destroyed before the call */
static inline const struct dlist_entry *dlist_circular_end(
	const struct dlist_circular *const dlc/*!=NULL*/)
{
	DLIST_ASSERT_PTR(dlc);
	return &dlc->l.e;
}

/* check if doubly-linked circular list is empty */
/* note: do not checks dlc->dlist_circular_last */
static inline int dlist_circular_is_empty(
	const struct dlist_circular *const dlc/*!=NULL*/)
{
	DLIST_ASSERT_PTR(dlc);
	DLIST_ASSERT_PTR(dlc->dlist_circular_first);
	return dlist_circular_end(dlc) == dlc->dlist_circular_first;
}

static inline struct dlist *dlist_const_cast_(
	const struct dlist *const dl/*NULL?*/)
{
#ifdef __cplusplus
	return const_cast<struct dlist*>(dl);
#else
#if defined(__GNUC__) && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual" /* casting away qualifiers */
#endif
	return (struct dlist*)dl;
#if defined(__GNUC__) && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
}

static inline struct dlist_circular *dlist_circular_const_cast_(
	const struct dlist_circular *const dlc/*NULL?*/)
{
#ifdef __cplusplus
	return const_cast<struct dlist_circular*>(dlc);
#else
#if defined(__GNUC__) && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual" /* casting away qualifiers */
#endif
	return (struct dlist_circular*)dlc;
#if defined(__GNUC__) && __GNUC__ > 4 - (__GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
}

static inline struct dlist *dlist_check_non_circular(
	const struct dlist *const dl/*!=NULL*/)
{
	DLIST_ASSERT_PTR(dl);
	DLIST_ASSERT(!dl->dlist_first || !dl->dlist_first->prev);
	DLIST_ASSERT(!dl->dlist_last || !dl->dlist_last->next);
	DLIST_ASSERT(!dl->dlist_first == !dl->dlist_last);
	DLIST_ASSERT_PTRS(&dl->e != dl->dlist_first);
	DLIST_ASSERT_PTRS(&dl->e != dl->dlist_last);
	return dlist_const_cast_(dl);
}

static inline struct dlist_circular *dlist_check_circular(
	const struct dlist_circular *const dlc/*!=NULL*/)
{
	DLIST_ASSERT_PTR(dlc);
	DLIST_ASSERT_PTRS(dlc->dlist_circular_first && &dlc->l.e == dlc->dlist_circular_first->prev);
	DLIST_ASSERT_PTRS(dlc->dlist_circular_last && &dlc->l.e == dlc->dlist_circular_last->next);
	DLIST_ASSERT_PTRS((&dlc->l.e == dlc->dlist_circular_first) == (&dlc->l.e == dlc->dlist_circular_last));
	return dlist_circular_const_cast_(dlc);
}

/* make doubly-linked list circular */
static inline struct dlist_circular *dlist_make_circular(
	struct dlist *const dl/*!=NULL*/)
{
	(void)dlist_check_non_circular(dl);
	if (dl->dlist_first) {
		/* note: l may be == f */
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
static inline struct dlist *dlist_make_uncircular(
	struct dlist_circular *const dlc/*!=NULL*/)
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
static inline void dlist_check_sublist(
	const struct dlist_entry *const s/*!=NULL,==e?*/,
	const struct dlist_entry *const e/*!=NULL,==s?*/)
{
	DLIST_ASSERT_PTR(s);
	DLIST_ASSERT_PTR(e);
	DLIST_ASSERT_PTRS(s == e || s->next);
	DLIST_ASSERT_PTRS(s == e || e->prev);
}

/* -------------- non-circular dlist methods ------------ */

/* check that 'c' is not an entry of circular list */
static inline void dlist_entry_check_non_circular(
	const struct dlist *const dl/*!=NULL*/,
	const struct dlist_entry *const c/*!=NULL,==&dl->e?*/)
{
	DLIST_ASSERT_PTR(dl);
	DLIST_ASSERT_PTR(c);
	DLIST_ASSERT_PTRS(&dl->e != c->next);
	DLIST_ASSERT_PTRS(&dl->e != c->prev);
}

/* insert a list of linked entries after the current entry c, if c == &dl->e, then at front of the list */
/* NOTE: caller should set, possibly before the call, s->prev to c (or to NULL, if inserting at front of the list) */
static inline struct dlist *dlist_insert_list_after_(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const c/*!=NULL,==&dl->e?*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_check_non_circular(dl);
	dlist_entry_check_non_circular(dl, c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	DLIST_ASSERT_PTRS(&dl->e != s);
	DLIST_ASSERT_PTRS(&dl->e != e);
	{
		struct dlist_entry *DLIST_RESTRICT const n = c->next;
		DLIST_ASSERT_PTRS(n != c);
		DLIST_ASSERT_PTRS(n != s);
		DLIST_ASSERT_PTRS(n != e);
		DLIST_ASSERT_PTRS(n != &dl->e);
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
static inline struct dlist *dlist_insert_list_before_(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const c/*!=NULL,==&dl->e?*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_check_non_circular(dl);
	dlist_entry_check_non_circular(dl, c);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(c != s);
	DLIST_ASSERT_PTRS(c != e);
	DLIST_ASSERT_PTRS(&dl->e != s);
	DLIST_ASSERT_PTRS(&dl->e != e);
	{
		struct dlist_entry *DLIST_RESTRICT const p = c->prev;
		DLIST_ASSERT_PTRS(p != c);
		DLIST_ASSERT_PTRS(p != s);
		DLIST_ASSERT_PTRS(p != e);
		DLIST_ASSERT_PTRS(p != &dl->e);
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
static inline struct dlist *dlist_add_list_front_(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	/*caller should do: s->prev = NULL;*/
	return dlist_insert_list_after_(dl, &dl->e, s, e);
}

/* append a list of linked entries at back of the list */
/* NOTE: caller should set e->next to NULL, possibly before the call */
static inline struct dlist *dlist_add_list_back_(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	/*caller should do: e->next = NULL;*/
	return dlist_insert_list_before_(dl, &dl->e, s, e);
}

/* prepend a list of linked entries at front of the list */
static inline struct dlist *dlist_add_list_front(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_add_list_front_(dl, s, e);
	s->prev = NULL;
	return dl;
}

/* append a list of linked entries at back of the list */
static inline struct dlist *dlist_add_list_back(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_add_list_back_(dl, s, e);
	e->next = NULL;
	return dl;
}

/* prepend an entry at front of the list */
static inline struct dlist *dlist_add_front(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const e/*!=NULL*/)
{
	return dlist_add_list_front(dl, e, e);
}

/* append an entry at back of the list */
static inline struct dlist *dlist_add_back(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const e/*!=NULL*/)
{
	return dlist_add_list_back(dl, e, e);
}

/* insert a list of linked entries after the current entry */
static inline struct dlist *dlist_insert_list_after(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *DLIST_RESTRICT const c/*!=NULL,!=&dl->e*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	DLIST_ASSERT_PTR(dl);
	DLIST_ASSERT_PTRS(&dl->e != c);
	(void)dlist_insert_list_after_(dl, c, s, e);
	s->prev = c; /* if c == &dl->e, s->prev must be NULL - use dlist_add_list_front() instead */
	return dl;
}

/* insert a list of linked entries before the current entry */
static inline struct dlist *dlist_insert_list_before(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *DLIST_RESTRICT const c/*!=NULL,!=&dl->e*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	DLIST_ASSERT_PTR(dl);
	DLIST_ASSERT_PTRS(&dl->e != c);
	(void)dlist_insert_list_before_(dl, c, s, e);
	e->next = c; /* if c == &dl->e, e->next must be NULL - use dlist_add_list_back() instead */
	return dl;
}

/* insert an entry after the current one */
static inline struct dlist *dlist_insert_after(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *DLIST_RESTRICT const c/*!=NULL,!=&dl->e*/,
	struct dlist_entry *DLIST_RESTRICT const e/*!=NULL,!=c*/)
{
	return dlist_insert_list_after(dl, c, e, e);
}

/* insert an entry before the current one */
static inline struct dlist *dlist_insert_before(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *DLIST_RESTRICT const c/*!=NULL,!=&dl->e*/,
	struct dlist_entry *DLIST_RESTRICT const e/*!=NULL,!=c*/)
{
	return dlist_insert_list_before(dl, c, e, e);
}

/* remove a list of linked entries from the list */
static inline struct dlist *dlist_remove_list(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const os/*!=NULL,==oe?*/,
	struct dlist_entry *const oe/*!=NULL,==os?*/)
{
	(void)dlist_check_non_circular(dl);
	DLIST_ASSERT_PTRS(&dl->e != os);
	DLIST_ASSERT_PTRS(&dl->e != oe);
	dlist_entry_check_non_circular(dl, os);
	dlist_entry_check_non_circular(dl, oe);
	{
		struct dlist_entry *DLIST_RESTRICT const n = oe->next;
		struct dlist_entry *DLIST_RESTRICT const p = os->prev;
		DLIST_ASSERT_PTRS(n != p);
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		DLIST_ASSERT_PTRS(n != &dl->e);
		DLIST_ASSERT_PTRS(p != &dl->e);
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
static inline struct dlist *dlist_remove(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const oe/*!=NULL*/)
{
	return dlist_remove_list(dl, oe, oe);
}

/* restore previously removed/replaced sub-list */
static inline struct dlist *dlist_restore_list_(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const os/*!=NULL,==oe?*/,
	struct dlist_entry *const oe/*!=NULL,==os?*/)
{
	(void)dlist_check_non_circular(dl);
	DLIST_ASSERT_PTRS(&dl->e != os);
	DLIST_ASSERT_PTRS(&dl->e != oe);
	dlist_entry_check_non_circular(dl, os);
	dlist_entry_check_non_circular(dl, oe);
	{
		struct dlist_entry *DLIST_RESTRICT const n = oe->next;
		struct dlist_entry *DLIST_RESTRICT const p = os->prev;
		DLIST_ASSERT_PTRS(n != p);
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		DLIST_ASSERT_PTRS(n != &dl->e);
		DLIST_ASSERT_PTRS(p != &dl->e);
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

/* only if there are no entries between os->prev and oe->next in the list,
  may restore previously removed sub-list */
static inline struct dlist *dlist_restore_list(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const os/*!=NULL,==oe?*/,
	struct dlist_entry *const oe/*!=NULL,==os?*/)
{
	DLIST_ASSERT_PTR(dl);
	DLIST_ASSERT_PTR(os);
	DLIST_ASSERT_PTR(oe);
	/* must be no entries between os->prev and oe->next in the list */
	DLIST_ASSERT_PTRS((os->prev ? os->prev->next : dl->dlist_first) == oe->next);
	DLIST_ASSERT_PTRS((oe->next ? oe->next->prev : dl->dlist_last) == os->prev);
	return dlist_restore_list_(dl, os, oe);
}

/* only if there are no entries between oe->prev and oe->next in the list,
  may restore previously removed entry */
static inline struct dlist *dlist_restore(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const oe/*!=NULL*/)
{
	return dlist_restore_list(dl, oe, oe);
}

/* replace old sub-list in the list with a new one */
static inline struct dlist *dlist_replace_list(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *const os/*!=NULL,==oe?*/,
	struct dlist_entry *const oe/*!=NULL,==os?*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_check_non_circular(dl);
	DLIST_ASSERT_PTRS(&dl->e != os);
	DLIST_ASSERT_PTRS(&dl->e != oe);
	DLIST_ASSERT_PTRS(&dl->e != s);
	DLIST_ASSERT_PTRS(&dl->e != e);
	dlist_entry_check_non_circular(dl, os);
	dlist_entry_check_non_circular(dl, oe);
	dlist_check_sublist(s, e);
	DLIST_ASSERT_PTRS(os != s);
	DLIST_ASSERT_PTRS(os != e);
	DLIST_ASSERT_PTRS(oe != s);
	DLIST_ASSERT_PTRS(oe != e);
	{
		struct dlist_entry *DLIST_RESTRICT const n = oe->next;
		struct dlist_entry *DLIST_RESTRICT const p = os->prev;
		DLIST_ASSERT_PTRS(n != p);
		DLIST_ASSERT_PTRS(n != os);
		DLIST_ASSERT_PTRS(n != oe);
		DLIST_ASSERT_PTRS(n != s);
		DLIST_ASSERT_PTRS(n != e);
		DLIST_ASSERT_PTRS(p != os);
		DLIST_ASSERT_PTRS(p != oe);
		DLIST_ASSERT_PTRS(p != s);
		DLIST_ASSERT_PTRS(p != e);
		DLIST_ASSERT_PTRS(p != &dl->e);
		DLIST_ASSERT_PTRS(n != &dl->e);
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
static inline struct dlist *dlist_replace(
	struct dlist *const dl/*!=NULL*/,
	struct dlist_entry *DLIST_RESTRICT const o/*!=NULL*/,
	struct dlist_entry *DLIST_RESTRICT const e/*!=NULL*/)
{
	return dlist_replace_list(dl, o, o, e, e);
}

/* move all items from src list to dst list */
/* note: initializes dst list, it may be not initialized before the call */
/* note: src list will not be changed, it will still reference moved entries after the call */
static inline struct dlist *dlist_move(
	struct dlist *DLIST_RESTRICT const dst/*!=NULL,!=src,initialized?*/,
	const struct dlist *DLIST_RESTRICT const src/*!=NULL,!=dst*/)
{
	(void)dlist_check_non_circular(src);
	DLIST_ASSERT_PTR(dst);
	DLIST_ASSERT_PTRS(dst != src);
	dst->dlist_first = src->dlist_first;
	dst->dlist_last = src->dlist_last;
	return dst;
}

/* -------------- circular dlist methods ------------ */

/* check that 'c' is an entry of circular list */
static inline void dlist_entry_check_circular(
	const struct dlist_entry *const c/*!=NULL*/)
{
	DLIST_ASSERT_PTR(c);
	DLIST_ASSERT(c->next && c->prev);
}

/* insert a list of linked entries after the current entry c of circular list, if c == &dlc->l.e, then at front of the list */
/* NOTE: caller should set, possibly before the call, s->prev to c */
static inline struct dlist_entry *dlist_circular_insert_list_after_(
	struct dlist_entry *const c/*!=NULL,!=s,!=e*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
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
static inline struct dlist_entry *dlist_circular_insert_list_before_(
	struct dlist_entry *const c/*!=NULL,!=s,!=e*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
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
static inline struct dlist_circular *dlist_circular_add_list_front_(
	struct dlist_circular *const dlc/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_check_circular(dlc);
	/*caller should do: s->prev = &dlc->l.e;*/
	(void)dlist_circular_insert_list_after_(&dlc->l.e, s, e);
	return dlc;
}

/* append a list of linked entries at back of circular list */
/* NOTE: caller should set e->next to &dlc->l.e, possibly before the call */
static inline struct dlist_circular *dlist_circular_add_list_back_(
	struct dlist_circular *const dlc/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_check_circular(dlc);
	/*caller should do: e->next = &dlc->l.e;*/
	(void)dlist_circular_insert_list_before_(&dlc->l.e, s, e);
	return dlc;
}

/* prepend a list of linked entries at front of circular list */
static inline struct dlist_circular *dlist_circular_add_list_front(
	struct dlist_circular *const dlc/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_circular_add_list_front_(dlc, s, e);
	s->prev = &dlc->l.e; /* dlist_circular_end(dlc) */
	return dlc;
}

/* append a list of linked entries at back of circular list */
static inline struct dlist_circular *dlist_circular_add_list_back(
	struct dlist_circular *const dlc/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_circular_add_list_back_(dlc, s, e);
	e->next = &dlc->l.e; /* dlist_circular_end(dlc) */
	return dlc;
}

/* prepend an entry at front of circular list */
static inline struct dlist_circular *dlist_circular_add_front(
	struct dlist_circular *const dlc/*!=NULL*/,
	struct dlist_entry *const e/*!=NULL,!=&dlc->l.e*/)
{
	return dlist_circular_add_list_front(dlc, e, e);
}

/* append an entry at back of circular list */
static inline struct dlist_circular *dlist_circular_add_back(
	struct dlist_circular *const dlc/*!=NULL*/,
	struct dlist_entry *const e/*!=NULL,!=&dlc->l.e*/)
{
	return dlist_circular_add_list_back(dlc, e, e);
}

/* insert a list of linked entries after the current entry c of circular list, if c == &dlc->e, then at front of the list */
static inline struct dlist_entry *dlist_circular_insert_list_after(
	struct dlist_entry *DLIST_RESTRICT const c/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_circular_insert_list_after_(c, s, e);
	s->prev = c;
	return c;
}

/* insert a list of linked entries before the current entry c of circular list, if c == &dlc->e, then at back of the list */
static inline struct dlist_entry *dlist_circular_insert_list_before(
	struct dlist_entry *DLIST_RESTRICT const c/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	(void)dlist_circular_insert_list_before_(c, s, e);
	e->next = c;
	return c;
}

/* insert an entry after the current entry c of circular list, if c == &dlc->e, then at front of the list */
static inline struct dlist_entry *dlist_circular_insert_after(
	struct dlist_entry *DLIST_RESTRICT const c/*!=NULL,!=e*/,
	struct dlist_entry *DLIST_RESTRICT const e/*!=NULL,!=c*/)
{
	return dlist_circular_insert_list_after(c, e, e);
}

/* insert an entry before the current entry c of circular list, if c == &dlc->e, then at back of the list */
static inline struct dlist_entry *dlist_circular_insert_before(
	struct dlist_entry *DLIST_RESTRICT const c/*!=NULL,!=e*/,
	struct dlist_entry *DLIST_RESTRICT const e/*!=NULL,!=c*/)
{
	return dlist_circular_insert_list_before(c, e, e);
}

/* remove a list of linked entries from circular list */
static inline void dlist_circular_remove_list(
	struct dlist_entry *const os/*!=NULL,==oe?*/,
	struct dlist_entry *const oe/*!=NULL,==os?*/)
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
static inline struct dlist_entry *dlist_circular_remove(
	struct dlist_entry *const oe/*!=NULL*/)
{
	dlist_circular_remove_list(oe, oe);
	return oe;
}

/* restore previously removed/replaced sub-list of circular list */
static inline void dlist_circular_restore_list_(
	struct dlist_entry *const os/*!=NULL,==oe?*/,
	struct dlist_entry *const oe/*!=NULL,==os?*/)
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

/* only if there are no entries between os->prev and oe->next in circular list,
  may restore previously removed sub-list */
static inline void dlist_circular_restore_list(
	struct dlist_entry *const os/*!=NULL,==oe?*/,
	struct dlist_entry *const oe/*!=NULL,==os?*/)
{
	DLIST_ASSERT_PTR(os);
	DLIST_ASSERT_PTR(oe);
	/* must be no entries between os->prev and oe->next in circular list */
	DLIST_ASSERT_PTRS(os->prev && os->prev->next == oe->next);
	DLIST_ASSERT_PTRS(oe->next && oe->next->prev == os->prev);
	dlist_circular_restore_list_(os, oe);
}

/* only if there are no entries between oe->prev and oe->next in circular list,
  may restore previously removed entry */
static inline struct dlist_entry *dlist_circular_restore(
	struct dlist_entry *const oe/*!=NULL*/)
{
	dlist_circular_restore_list(oe, oe);
	return oe;
}

/* replace old sub-list in circular list with a new one */
static inline void dlist_circular_replace_list(
	struct dlist_entry *const os/*!=NULL,==oe?*/,
	struct dlist_entry *const oe/*!=NULL,==os?*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
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
		s->prev = p;
		n->prev = e;
		p->next = s;
		e->next = n;
	}
}

/* replace an old entry in circular list with a new one */
static inline void dlist_circular_replace(
	struct dlist_entry *DLIST_RESTRICT const o/*!=NULL,!=e*/,
	struct dlist_entry *DLIST_RESTRICT const e/*!=NULL,!=o*/)
{
	dlist_circular_replace_list(o, o, e, e);
}

/* move all items from src list to dst list */
/* note: initializes dst list, it may be not initialized before the call */
/* note: src list, if empty, will be changed so src->dlist_circular_first and src->dlist_circular_last will point to &dst->l.e */
/* note: if src list is not empty, first and last entries of the list will reference dst list */
static inline struct dlist_circular *dlist_circular_move(
	struct dlist_circular *DLIST_RESTRICT const dst/*!=NULL,!=src,initialized?*/,
	struct dlist_circular *DLIST_RESTRICT const src/*!=NULL,!=dst*/)
{
	(void)dlist_check_circular(src);
	DLIST_ASSERT_PTR(dst);
	DLIST_ASSERT_PTRS(dst != src);
	src->dlist_circular_first->prev = &dst->l.e;
	src->dlist_circular_last->next = &dst->l.e;
	/* note: if src list is empty, then:
	  setting src->dlist_circular_first->prev to &dst->l.e will set src->dlist_circular_last to &dst->l.e;
	  setting src->dlist_circular_last->next to &dst->l.e will set src->dlist_circular_first to &dst->l.e */
	dst->dlist_circular_first = src->dlist_circular_first;
	dst->dlist_circular_last = src->dlist_circular_last;
	return dst;
}

/* -------------- making sub-list - to add to a list later ------------ */

/* link abcd before 123 at head:
   123 + abcd  -> abcd123
   ^h    ^s ^e    ^s       */
/* returns new head */
static inline struct dlist_entry *dlist_entry_link_list_before(
	struct dlist_entry *DLIST_RESTRICT const h/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	DLIST_ASSERT_PTR(h);
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
static inline struct dlist_entry *dlist_entry_link_list_after(
	struct dlist_entry *DLIST_RESTRICT const t/*!=NULL*/,
	struct dlist_entry *const s/*!=NULL,==e?*/,
	struct dlist_entry *const e/*!=NULL,==s?*/)
{
	DLIST_ASSERT_PTR(t);
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
static inline struct dlist_entry *dlist_entry_link_before(
	struct dlist_entry *DLIST_RESTRICT const h/*!=NULL,!=e*/,
	struct dlist_entry *DLIST_RESTRICT const e/*!=NULL,!=h*/)
{
	return dlist_entry_link_list_before(h, e, e);
}

/* link x after 123 at tail:
   123 + x  -> 123x
     ^t  ^e       ^e */
/* returns new tail */
static inline struct dlist_entry *dlist_entry_link_after(
	struct dlist_entry *DLIST_RESTRICT const t/*!=NULL,!=e*/,
	struct dlist_entry *DLIST_RESTRICT const e/*!=NULL,!=t*/)
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

  NOTE: at end of iteration, iterator e will be NULL
    (if iteration wasn't interrupted by 'break').
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
		(NULL != (e = n)) ? (n = n->next), 1 : 0;)

#define dlist_iterate_delete_backward(dl, e, p) \
	for (p = dlist_check_non_circular(DLIST_CHECK_CONSTNESS(dl, p))->dlist_last; \
		(NULL != (e = p)) ? (p = p->prev), 1 : 0;)

/* -------------- iterating over entries of circular doubly-linked list ------------ */

#define dlist_circular_iterate(dlc, e) \
	for (e = dlist_check_circular(DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, e))->dlist_circular_first; \
		(dlist_circular_end(dlc) != e) ? 1 : ((e = NULL), 0); e = e->next)

#define dlist_circular_iterate_backward(dlc, e) \
	for (e = dlist_check_circular(DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, e))->dlist_circular_last; \
		(dlist_circular_end(dlc) != e) ? 1 : ((e = NULL), 0); e = e->prev)

/* same as above, but allow to delete an entry referenced by the iterator */

#define dlist_circular_iterate_delete(dlc, e, n) \
	for (n = dlist_check_circular(DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, n))->dlist_circular_first; \
		(dlist_circular_end(dlc) != n) ? (e = n), (n = n->next), 1 : ((e = NULL), 0);)

#define dlist_circular_iterate_delete_backward(dlc, e, p) \
	for (p = dlist_check_circular(DLIST_CIRCULAR_CHECK_CONSTNESS(dlc, p))->dlist_circular_last; \
		(dlist_circular_end(dlc) != p) ? (e = p), (p = p->prev), 1 : ((e = NULL), 0);)

#ifdef __cplusplus
}
#endif

#endif /* DLIST_H_INCLUDED */
