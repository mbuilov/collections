/* test.c */

#include <stdio.h>
#include "dlist.h"

static DLIST_DECLARE(dl);
static DLIST_CIRCULAR_DECLARE(dlc);

#define TEST(expr) do { \
	if (!(expr)) { \
		printf("test %u failed (at line = %d)\n", test_number, __LINE__); \
		return 1; \
	} \
	printf("test %u ok\n", test_number); \
	test_number++; \
} while (0)

/* to make chained additions to look nicer, e.g.:
  dlist_add_front_r(e0,
  dlist_add_front_r(e1,
  dlist_add_front_r(e2,
  dl))) */
#define dlist_add_front_r(e, dl)   dlist_add_front(dl, e)
#define dlist_add_back_r(e, dl)    dlist_add_back(dl, e)

/* to make chained additions to look nicer, e.g.:
  dlist_remove_r(e1,
  dlist_remove_r(e0,
  dlist_remove_r(e2,
  dl))) */
#define dlist_remove_r(oe, dl)   dlist_remove(dl, oe)

/* to make chained additions to look nicer, e.g.:
  dlist_circular_add_front_r(e0,
  dlist_circular_add_front_r(e1,
  dlist_circular_add_front_r(e2,
  dl))) */
#define dlist_circular_add_front_r(e, dlc)   dlist_circular_add_front(dlc, e)
#define dlist_circular_add_back_r(e, dlc)    dlist_circular_add_back(dlc, e)

#define dlist_entry_link_before_r(e, h)      dlist_entry_link_before(h, e)
#define dlist_entry_link_after_r(e, t)       dlist_entry_link_after(t, e)

int main(int argc, char *argv[])
{
	unsigned test_number = 0;
	(void)argc, (void)argv;
	TEST(dlist_is_empty(&dl));
	TEST(dlist_circular_is_empty(&dlc));
	{
		struct dlist_circular *const x = dlist_make_circular(&dl);
		TEST(dlist_circular_is_empty(x));
		(void)dlist_make_uncircular(x);
		TEST(dlist_is_empty(&dl));
	}
	{
		struct dlist *const x = dlist_make_uncircular(&dlc);
		TEST(dlist_is_empty(x));
		(void)dlist_make_circular(x);
		TEST(dlist_circular_is_empty(&dlc));
	}
	{
		struct dlist_entry entries[5];
		(void)dlist_add_front_r(&entries[0],
		dlist_add_front_r(&entries[1],
		dlist_add_front_r(&entries[2],
		dlist_add_front_r(&entries[3],
		dlist_add_front_r(&entries[4],
		&dl)))));
		{
			struct dlist_entry *e;
			unsigned idx = 0;
			dlist_iterate(&dl, e) {
				TEST(&entries[idx] == e);
				idx++;
			}
			dlist_iterate_backward(&dl, e) {
				idx--;
				TEST(&entries[idx] == e);
			}
		}
		{
			struct dlist_entry *e;
			struct dlist_circular *const x = dlist_make_circular(&dl);
			unsigned idx = 0;
			dlist_circular_iterate(x, e) {
				TEST(&entries[idx] == e);
				idx++;
			}
			dlist_circular_iterate_backward(x, e) {
				idx--;
				TEST(&entries[idx] == e);
			}
			(void)dlist_make_uncircular(x);
		}
		dlist_remove_r(&entries[4],
		dlist_remove_r(&entries[2],
		dlist_remove_r(&entries[1],
		dlist_remove_r(&entries[3],
		dlist_remove_r(&entries[0],
		&dl)))));
		TEST(dlist_is_empty(&dl));
	}
	{
		struct dlist_entry entries[5];
		(void)dlist_circular_add_front_r(&entries[0],
		dlist_circular_add_front_r(&entries[1],
		dlist_circular_add_front_r(&entries[2],
		dlist_circular_add_front_r(&entries[3],
		dlist_circular_add_front_r(&entries[4],
		&dlc)))));
		{
			struct dlist_entry *e;
			unsigned idx = 0;
			dlist_circular_iterate(&dlc, e) {
				TEST(&entries[idx] == e);
				idx++;
			}
			dlist_circular_iterate_backward(&dlc, e) {
				idx--;
				TEST(&entries[idx] == e);
			}
		}
		{
			struct dlist_entry *e;
			struct dlist *const x = dlist_make_uncircular(&dlc);
			unsigned idx = 0;
			dlist_iterate(x, e) {
				TEST(&entries[idx] == e);
				idx++;
			}
			dlist_iterate_backward(x, e) {
				idx--;
				TEST(&entries[idx] == e);
			}
			(void)dlist_make_circular(x);
		}
		dlist_circular_remove(&entries[4]);
		dlist_circular_remove(&entries[2]);
		dlist_circular_remove(&entries[1]);
		dlist_circular_remove(&entries[3]);
		dlist_circular_remove(&entries[0]);
		TEST(dlist_circular_is_empty(&dlc));
	}
	{
		struct dlist_entry entries[5];
		(void)dlist_add_back_r(&entries[0],
		dlist_add_back_r(&entries[1],
		dlist_add_back_r(&entries[2],
		dlist_add_back_r(&entries[3],
		dlist_add_back_r(&entries[4],
		&dl)))));
		{
			struct dlist_entry *e;
			unsigned idx = 5;
			dlist_iterate(&dl, e) {
				idx--;
				TEST(&entries[idx] == e);
			}
			dlist_iterate_backward(&dl, e) {
				TEST(&entries[idx] == e);
				idx++;
			}
		}
		{
			struct dlist_entry *e, *n;
			unsigned idx = 5;
			dlist_iterate_delete(&dl, e, n) {
				idx--;
				TEST(&entries[idx] == e);
				dlist_remove(&dl, e);
			}
			TEST(dlist_is_empty(&dl));
		}
		(void)dlist_add_front_r(&entries[4],
		dlist_add_front_r(&entries[3],
		dlist_add_back_r(&entries[0],
		dlist_add_back_r(&entries[1],
		dlist_add_back_r(&entries[2],
		&dl)))));
		{
			struct dlist_entry *e, *n;
			unsigned idx = 0;
			dlist_iterate_delete_backward(&dl, e, n) {
				TEST(&entries[idx] == e);
				dlist_remove(&dl, e);
				idx++;
			}
			TEST(dlist_is_empty(&dl));
		}
	}
	{
		struct dlist_entry entries[5];
		(void)dlist_circular_add_back_r(&entries[0],
		dlist_circular_add_back_r(&entries[1],
		dlist_circular_add_back_r(&entries[2],
		dlist_circular_add_back_r(&entries[3],
		dlist_circular_add_back_r(&entries[4],
		&dlc)))));
		{
			struct dlist_entry *e;
			unsigned idx = 5;
			dlist_circular_iterate(&dlc, e) {
				idx--;
				TEST(&entries[idx] == e);
			}
			dlist_circular_iterate_backward(&dlc, e) {
				TEST(&entries[idx] == e);
				idx++;
			}
		}
		{
			struct dlist_entry *e, *n;
			unsigned idx = 5;
			dlist_circular_iterate_delete(&dlc, e, n) {
				idx--;
				TEST(&entries[idx] == e);
				dlist_circular_remove(e);
			}
			TEST(dlist_circular_is_empty(&dlc));
		}
		(void)dlist_circular_add_front_r(&entries[4],
		dlist_circular_add_front_r(&entries[3],
		dlist_circular_add_back_r(&entries[0],
		dlist_circular_add_back_r(&entries[1],
		dlist_circular_add_back_r(&entries[2],
		&dlc)))));
		{
			struct dlist_entry *e, *n;
			unsigned idx = 0;
			dlist_circular_iterate_delete_backward(&dlc, e, n) {
				TEST(&entries[idx] == e);
				dlist_circular_remove(e);
				idx++;
			}
			TEST(dlist_circular_is_empty(&dlc));
		}
	}
	{
		struct dlist_entry entries[5];
		struct dlist_entry *h =
		dlist_entry_link_before_r(&entries[3],
		dlist_entry_link_before_r(&entries[2],
		dlist_entry_link_before_r(&entries[1],
		&entries[0])));
		dlist_add_back(&dl, &entries[4]);
		dlist_add_list_back(&dl, h, &entries[0]);
		{
			struct dlist_entry *e, *n;
			unsigned idx = 5;
			dlist_iterate_delete(&dl, e, n) {
				idx--;
				TEST(&entries[idx] == e);
				dlist_remove(&dl, e);
			}
		}
		TEST(dlist_is_empty(&dl));
	}
	{
		struct dlist_entry entries[5];
		struct dlist_entry *t =
		dlist_entry_link_after_r(&entries[3],
		dlist_entry_link_after_r(&entries[2],
		dlist_entry_link_after_r(&entries[1],
		&entries[0])));
		dlist_add_front(&dl, &entries[4]);
		dlist_add_list_front(&dl, &entries[0], t);
		{
			struct dlist_entry *e, *n;
			unsigned idx = 5;
			dlist_iterate_delete_backward(&dl, e, n) {
				idx--;
				TEST(&entries[idx] == e);
				dlist_remove(&dl, e);
			}
		}
		TEST(dlist_is_empty(&dl));
	}
	{
		struct dlist_entry entries[5];
		struct dlist_entry *h =
		dlist_entry_link_before_r(&entries[3],
		dlist_entry_link_before_r(&entries[2],
		dlist_entry_link_before_r(&entries[1],
		&entries[0])));
		dlist_circular_add_back(&dlc, &entries[4]);
		dlist_circular_add_list_back(&dlc, h, &entries[0]);
		{
			struct dlist_entry *e, *n;
			unsigned idx = 5;
			dlist_circular_iterate_delete(&dlc, e, n) {
				idx--;
				TEST(&entries[idx] == e);
				dlist_circular_remove(e);
			}
		}
		TEST(dlist_circular_is_empty(&dlc));
	}
	{
		struct dlist_entry entries[5];
		struct dlist_entry *t =
		dlist_entry_link_after_r(&entries[3],
		dlist_entry_link_after_r(&entries[2],
		dlist_entry_link_after_r(&entries[1],
		&entries[0])));
		dlist_circular_add_front(&dlc, &entries[4]);
		dlist_circular_add_list_front(&dlc, &entries[0], t);
		{
			struct dlist_entry *e, *n;
			unsigned idx = 5;
			dlist_circular_iterate_delete_backward(&dlc, e, n) {
				idx--;
				TEST(&entries[idx] == e);
				dlist_circular_remove(e);
			}
		}
		TEST(dlist_circular_is_empty(&dlc));
	}
	{
		struct dlist_entry entries[10];
		dlist_add_back(&dl, &entries[0]);
		dlist_add_front(&dl, &entries[1]);
		dlist_add_back(&dl, &entries[3]);
		dlist_add_front(&dl, &entries[9]);
		dlist_add_back(&dl, &entries[2]);
		dlist_add_front(&dl, &entries[7]);
		/* 7,[9,1,0],3,2 -> 7,[6,5,4],3,2 */
		{
			struct dlist_entry *h = &entries[5];
			struct dlist_entry *t = h;
			h = dlist_entry_link_before(h, &entries[6]);
			t = dlist_entry_link_after(t, &entries[4]);
			dlist_replace_list(&dl, &entries[9], &entries[0], h, t);
			{
				struct dlist x;
				struct dlist_entry *e;
				unsigned idx = 2;
				dlist_move(&x, &dl);
				dlist_iterate_backward(&x, e) {
					TEST(&entries[idx] == e);
					dlist_remove(&x, e);
					dlist_restore(&x, e);
					idx++;
				}
			}
			{
				struct dlist_entry *e, *n;
				unsigned idx = 2;
				dlist_iterate_delete_backward(&dl, e, n) {
					TEST(&entries[idx] == e);
					dlist_remove(&dl, e);
					idx++;
				}
			}
		}
		TEST(dlist_is_empty(&dl));
	}
	{
		struct dlist_entry entries[10];
		dlist_circular_add_back(&dlc, &entries[0]);
		dlist_circular_add_front(&dlc, &entries[1]);
		dlist_circular_add_back(&dlc, &entries[3]);
		dlist_circular_add_front(&dlc, &entries[9]);
		dlist_circular_add_back(&dlc, &entries[2]);
		dlist_circular_add_front(&dlc, &entries[7]);
		/* 7,[9,1,0],3,2 -> 7,[6,5,4],3,2 */
		{
			struct dlist_entry *h = &entries[5];
			struct dlist_entry *t = h;
			h = dlist_entry_link_before(h, &entries[6]);
			t = dlist_entry_link_after(t, &entries[4]);
			dlist_circular_replace_list(&entries[9], &entries[0], h, t);
			{
				struct dlist_circular x;
				struct dlist_entry *e;
				unsigned idx = 2;
				dlist_circular_move(&x, &dlc);
				dlist_circular_iterate_backward(&x, e) {
					TEST(&entries[idx] == e);
					dlist_circular_remove(e);
					dlist_circular_restore(e);
					idx++;
				}
				dlist_circular_move(&dlc, &x);
			}
			{
				struct dlist_entry *e, *n;
				unsigned idx = 2;
				dlist_circular_iterate_delete_backward(&dlc, e, n) {
					TEST(&entries[idx] == e);
					dlist_circular_remove(e);
					idx++;
				}
			}
		}
		TEST(dlist_circular_is_empty(&dlc));
	}
	printf("all tests OK\n");
	return 0;
}
