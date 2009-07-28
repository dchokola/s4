#include "s4_be.h"
#include "xcu.h"
#include <stdio.h>

s4be_t *be;
char *name;

SETUP (s4_strstore) {
	if (!g_thread_get_initialized ())
		g_thread_init (NULL);
	name = tmpnam (NULL);
	be = s4be_open (name);
	return be == NULL;
}

CLEANUP () {
	s4be_close (be);
	unlink (name);
	return 0;
}

CASE (test_st_ref) {
	const char *strings[] =	{"a", "b", "c", "d", NULL};
	int nodes[100];
	int i;

	for (i = 0; strings[i] != NULL; i++) {
		nodes[i] = s4be_st_ref (be, strings[i]);

		CU_ASSERT_STRING_EQUAL (strings[i], s4be_st_reverse (be, nodes[i]));
		CU_ASSERT_EQUAL (nodes[i], s4be_st_lookup (be, strings[i]));

		CU_ASSERT_EQUAL (s4be_st_ref (be, strings[i]), nodes[i]);
	}

	CU_ASSERT_EQUAL (s4be_st_lookup (be, "asdf"), 0);

	for (i = 0; strings[i] != NULL; i++) {
		CU_ASSERT_EQUAL (s4be_st_unref (be, strings[i]), 1);
		CU_ASSERT_EQUAL (s4be_st_unref (be, strings[i]), 0);
		CU_ASSERT_EQUAL (s4be_st_unref (be, strings[i]), -1);
	}
}

static void regexp_test (const char *pat, ...)
{
	va_list ap;
	const char *arg;
	GList *list, *start;
	int found;

	start = s4be_st_regexp (be, pat);

	va_start (ap, pat);

	for (arg = va_arg (ap, const char*);
			arg != NULL;
			arg = va_arg (ap, const char*)) {
		for (found = 0, list = start
				; list != NULL && !found
				; list = g_list_next (list))
		{
			if (!strcasecmp (list->data, arg)) {
				start = g_list_delete_link (start, list);
				found = 1;
			}
		}

		CU_ASSERT_TRUE (found);
	}

	va_end (ap);

	CU_ASSERT_PTR_NULL (start);
}

CASE (test_st_regexp) {
	const char *strings[] = {"foo", "fooba", "10", "11", "FOOBAR", NULL};
	int i;

	for (i = 0; strings[i] != NULL; i++)
		s4be_st_ref (be, strings[i]);

	regexp_test ("foo", "foo", "fooba", "FOOBAR", NULL);
	regexp_test ("(?-i)FOO", "FOOBAR", NULL);
	regexp_test ("1(1|0)", "10", "11", NULL);
}
