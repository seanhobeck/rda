/**
 *
 *	@author Sean Hobeck
 *	@date 16/09/2025
 *
 */
#ifndef LRDA_DYNL_H
#define LRDA_DYNL_H

/*! @uses size_t */
#include <stddef.h>

/// @note a structure for a dynamic list.
typedef struct {
    // array of data in the list.
    void** data;
    // length (count) and capacity of the dynamic list.
    size_t length, capacity, isize;
    // element size; sizeof the data in the list.
} _int_dynl_t;

/**
 * @brief create a _int_dynl_t structure with a set item size.
 *
 * @param isize item size.
 * @return an allocated dynamic list.
 */
_int_dynl_t*
_int_dynl_create(size_t isize);

/**
 * @brief destroying / freeing a dynamically allocated list.
 *
 * @param list pointer to a dynamically allocated list.
 */
void
_int_dynl_destroy(_int_dynl_t* list);

/**
 * @brief push a new item <data> onto a dynamically allocated list.
 *
 * @param list pointer to a dynamically allocated list.
 * @param data data to be pushed onto the top of the allocated list.
 */
void
_int_dynl_push(_int_dynl_t* list, void* data);

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
_int_dynl_pop(_int_dynl_t* list, size_t index);

/** @brief shrinks the list to the length via realloc. */
void
_int_dynl_shrink(_int_dynl_t* list);
#endif //LRDA_DYNL_H
