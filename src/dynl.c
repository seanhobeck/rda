/**
 *
 *	@author Sean Hobeck
 *	@date 16/09/2025
 *
 */
#include "dynl.h"

/*! @uses fprintf */
#include <stdio.h>

/*! @uses calloc, realloc, free, exit */
#include <stdlib.h>

/*! @uses assert */
#include <assert.h>

/**
 * @brief create a _int_dynl_t structure with a set item size.
 *
 * @param isize item size.
 * @return an allocated dynamic list.
 */
_int_dynl_t*
_int_dynl_create(const size_t isize) {
    // assert if the isize == 0.
    assert(isize != 0);

    // allocate the list and set all data to be 0, except for isize.
    _int_dynl_t* list = calloc(1, sizeof *list);
    list->data = 0x0;
    list->length = 0;
    list->capacity = 0;
    list->isize = isize;
    return list;
};

/**
 * @brief destroying / freeing a dynamically allocated list.
 *
 * @param list pointer to a dynamically allocated list.
 */
void
_int_dynl_destroy(_int_dynl_t* list) {
    // assert if the list is 0x0.
    assert(list != 0x0);

    // iterate & free.
    for (size_t i = 0; i < list->length; i++)
        free(list->data[i]);
    free(list);
};

/**
 * @brief push a new item <data> onto a dynamically allocated list.
 *
 * @param list pointer to a dynamically allocated list.
 * @param data data to be pushed onto the top of the allocated list.
 */
void
_int_dynl_push(_int_dynl_t* list, void* data) {
    // assert if the list or data == 0x0.
    assert(list != 0x0 && data != 0x0);

    // compare length and capacity.
    if (list->length == list->capacity) {
        size_t _capacity = list->capacity == 0 ? 16 : list->capacity * 2;
        void** _data = realloc(list->data, list->isize * _capacity);
        if (!_data) {
            fprintf(stderr, "realloc failed; could not allocate memory for list.");
            exit(EXIT_FAILURE);
        }
        list->data = _data;
    }
    list->data[list->length++] = data;
};

/**
 * @brief pop data out of a dynamically allocated list,
 *  while also making sure to shift down / coalesce the memory.
 *  this function does not SHRINK the allocated memory, see
 *  @ref<_int_dynl_shrink>[("_int_dynl_shrink")].
 *
 * @param list pointer to a dynamically allocated list.
 * @param index index at which to pop the item.
 * @return data at <index>, popped off the list.
 */
void*
_int_dynl_pop(_int_dynl_t* list, size_t index) {
    // assert if the list == 0x0 or index > list->length.
    assert(list != 0x0);
    assert(list->length >= index);

    // capture the element
    void* item = calloc(1u, list->isize);
    // free, shift down and then decrement length.

    return 0x0;
};

/** @brief shrinks the list to the length via realloc. */
void
_int_dynl_shrink(_int_dynl_t* list) {
    // do a realloc down where capacity = length.
    return;
};