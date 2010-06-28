/*  S4 - An XMMS2 medialib backend
 *  Copyright (C) 2009, 2010 Sivert Berg
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 */

#include <s4.h>
#include "s4_priv.h"
#include <stdlib.h>
#include <glib.h>

struct s4_resultset_St {
	int col_count;
	int row_count;

	GPtrArray *results;
};

/**
 * @defgroup ResultSet Result Set
 * @ingroup S4
 * @brief A set of results
 *
 * @{
 */

/**
 * @{
 * @internal
 */

/**
 * Creates a new resultset
 *
 * @param col_count The number of columns in every result
 * @return A new resultset with the column count set to col_count
 */
s4_resultset_t *s4_resultset_create (int col_count)
{
	s4_resultset_t *ret = malloc (sizeof (s4_resultset_t));

	ret->col_count = col_count;
	ret->row_count = 0;

	ret->results = g_ptr_array_new ();

	return ret;
}

/**
 * @}
 */

/**
 * Adds a row to a resultset
 * @param set The resultset to add to
 * @param results An array of results to add
 */
void s4_resultset_add_row (s4_resultset_t *set, s4_result_t **results)
{
	g_ptr_array_add (set->results, results);
	set->row_count++;
}

/**
 * Gets a result from a resultset
 * @param set The set to get the result from
 * @param row The row
 * @param col The column
 * @return The result at (row,col), or NULL if it does not exist
 */
const s4_result_t *s4_resultset_get_result (const s4_resultset_t *set, int row, int col)
{
	s4_result_t **res;
	if (row >= set->row_count || row < 0 || col >= set->col_count || col < 0)
		return NULL;

	res = g_ptr_array_index (set->results, row);
	return res[col];
}

/**
 * Gets the column count for a resultset
 * @param set The set to find the column count of
 * @return The column count
 */
int s4_resultset_get_colcount (const s4_resultset_t *set)
{
	return set->col_count;
}

/**
 * Gets the row count for a resultset
 * @param set The set to find the row count of
 * @return The row count
 */
int s4_resultset_get_rowcount (const s4_resultset_t *set)
{
	return set->row_count;
}

/**
 * Frees a resultset and all the results in it
 * @param set The set to free
 */
void s4_resultset_free (s4_resultset_t *set)
{
	int i,j;

	for (i = 0; i < set->row_count; i++) {
		s4_result_t **results = g_ptr_array_index (set->results, i);
		for (j = 0; j < set->col_count; j++) {
			s4_result_t *prev,*res = results[j];

			while (res != NULL) {
				prev = res;
				res = (s4_result_t*)s4_result_next (res);
				s4_result_free (prev);
			}
		}

		free (results);
	}

	g_ptr_array_free (set->results, TRUE);
	free (set);
}

/**
 * @}
 */
