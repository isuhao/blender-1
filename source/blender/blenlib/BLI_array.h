/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2008 Blender Foundation.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): Joseph Eagar.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

#ifndef __BLI_ARRAY_H__
#define __BLI_ARRAY_H__

/** \file BLI_array.h
 *  \ingroup bli
 *  \brief A (mainly) macro array library.
 */

/* -------------------------------------------------------------------- */
/* internal defines */

/* this returns the entire size of the array, including any buffering. */
#define _bli_array_totalsize_dynamic(arr)  (                                  \
	((arr) == NULL) ?                                                         \
	    0 :                                                                   \
	    MEM_allocN_len(arr) / sizeof(*arr)                                    \
)

#define _bli_array_totalsize_static(arr)  \
	(sizeof(_##arr##_static) / sizeof(*arr))

#define _bli_array_totalsize(arr)  (                                          \
	(size_t)                                                                  \
	(((void *)(arr) == (void *)_##arr##_static && (void *)(arr) != NULL) ?    \
	    _bli_array_totalsize_static(arr) :                                    \
	    _bli_array_totalsize_dynamic(arr))                                    \
)

/* BLI_array.c
 *
 * Doing the realloc in a macro isn't so simple,
 * so use a function the macros can use.
 */
void _bli_array_grow_func(void **arr_p, const void *arr_static,
                          const int sizeof_arr_p, const int arr_count, const int num,
                          const char *alloc_str);


/* -------------------------------------------------------------------- */
/* public defines */

#define BLI_array_declare(arr)                                                \
	int   _##arr##_count = 0;                                                 \
	void *_##arr##_static = NULL

/* this will use stack space, up to maxstatic array elements, before
 * switching to dynamic heap allocation */
#define BLI_array_staticdeclare(arr, maxstatic)                               \
	int   _##arr##_count = 0;                                                 \
	char  _##arr##_static[maxstatic * sizeof(arr)]

/* this returns the logical size of the array, not including buffering. */
#define BLI_array_count(arr) _##arr##_count

/* Grow the array by a fixed number of items. zeroes the new elements.
 *
 * Allow for a large 'num' value when the new size is more then double
 * to allocate the exact sized array. */

/* grow an array by a specified number of items */
#define BLI_array_grow_items(arr, num)  ((                                    \
	(((void *)(arr) == NULL) &&                                               \
	 ((void *)(_##arr##_static) != NULL) &&                                   \
	/* don't add _##arr##_count below because it must be zero */              \
	 (_bli_array_totalsize_static(arr) >= _##arr##_count + num)) ?            \
	/* we have an empty array and a static var big enough */                  \
	(arr = (void *)_##arr##_static)                                           \
	    :                                                                     \
	/* use existing static array or allocate */                               \
	(LIKELY(_bli_array_totalsize(arr) >= _##arr##_count + num) ?              \
	 (void)0 /* do nothing */ :                                               \
	 (_bli_array_grow_func((void **)&(arr), _##arr##_static,                  \
	                       sizeof(*arr), _##arr##_count, num,                 \
	                       "BLI_array." #arr),                                \
	 (void)0)) /* msvc2008 needs this */                                      \
	),                                                                        \
	/* increment the array count, all conditions above are accounted for. */  \
	(_##arr##_count += num))

/* returns length of array */
#define BLI_array_grow_one(arr)  BLI_array_grow_items(arr, 1)


/* appends an item to the array. */
#define BLI_array_append(arr, item)  (                                        \
	(void) BLI_array_grow_one(arr),                                           \
	(void) (arr[_##arr##_count - 1] = item)                                   \
)

/* appends an item to the array and returns a pointer to the item in the array.
 * item is not a pointer, but actual data value.*/
#define BLI_array_append_r(arr, item)  (                                      \
	(void) BLI_array_grow_one(arr),                                           \
	(void) (arr[_##arr##_count - 1] = item),                                  \
	(&arr[_##arr##_count - 1])                                                \
)

#define BLI_array_reserve(arr, num)                                           \
	BLI_array_grow_items(arr, num), (void)(_##arr##_count -= (num))


#define BLI_array_free(arr)                                                   \
	if (arr && (char *)arr != _##arr##_static) {                              \
	    BLI_array_fake_user(arr);                                             \
	    MEM_freeN(arr);                                                       \
	} (void)0

#define BLI_array_pop(arr)  (                                                 \
	(arr && _##arr##_count) ?                                                 \
	    arr[--_##arr##_count] :                                               \
	    NULL                                                                  \
)

/* resets the logical size of an array to zero, but doesn't
 * free the memory. */
#define BLI_array_empty(arr)                                                  \
	{ _##arr##_count = 0; } (void)0

/* set the count of the array, doesn't actually increase the allocated array
 * size.  don't use this unless you know what you're doing. */
#define BLI_array_length_set(arr, count)                                      \
	{ _##arr##_count = (count); }(void)0

/* only to prevent unused warnings */
#define BLI_array_fake_user(arr)                                              \
	(void)_##arr##_count,                                                     \
	(void)_##arr##_static


/* -------------------------------------------------------------------- */
/* other useful defines
 * (unrelated to the main array macros) */

/* not part of the 'API' but handy funcs,
 * same purpose as BLI_array_staticdeclare()
 * but use when the max size is known ahead of time */
#define BLI_array_fixedstack_declare(arr, maxstatic, realsize, allocstr)      \
	char _##arr##_static[maxstatic * sizeof(*(arr))];                         \
	const int _##arr##_is_static = ((void *)_##arr##_static) != (             \
	    arr = ((realsize) <= maxstatic) ?                                     \
	        (void *)_##arr##_static :                                         \
	        MEM_mallocN(sizeof(*(arr)) * (realsize), allocstr)                \
	    )                                                                     \

#define BLI_array_fixedstack_free(arr)                                        \
	if (_##arr##_is_static) {                                                 \
		MEM_freeN(arr);                                                       \
	} (void)0


/* alloca */
#ifdef _MSC_VER
#  define alloca _alloca
#endif

#if defined(__MINGW32__)
#  include <malloc.h>  /* mingw needs for alloca() */
#endif

#if defined(__GNUC__) || defined(__clang__)
#define BLI_array_alloca(arr, realsize) \
	(typeof(arr))alloca(sizeof(*arr) * (realsize))

#define BLI_array_alloca_and_count(arr, realsize) \
	(typeof(arr))alloca(sizeof(*arr) * (realsize));  \
	const int _##arr##_count = (realsize)

#else
#define BLI_array_alloca(arr, realsize) \
	alloca(sizeof(*arr) * (realsize))

#define BLI_array_alloca_and_count(arr, realsize) \
	alloca(sizeof(*arr) * (realsize));  \
	const int _##arr##_count = (realsize)
#endif

#endif  /* __BLI_ARRAY_H__ */
