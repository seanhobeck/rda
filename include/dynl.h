/**
 *	@author Sean Hobeck
 *	@date 18/09/2025
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
} rda_dynl_t;

/**
 * @brief create a rda_dynl_t structure with a set item size.
 *
 * @param isize item size.
 * @return an allocated dynamic list.
 */
rda_dynl_t*
rda_dynl_create(size_t isize);

/**
 * @brief destroying / freeing a dynamically allocated list.
 *
 * @param list pointer to a dynamically allocated list.
 */
void
rda_dynl_destroy(rda_dynl_t* list);

/**
 * @brief push a new item <data> onto a dynamically allocated list.
 *
 * @param list pointer to a dynamically allocated list.
 * @param data data to be pushed onto the top of the allocated list.
 */
void
rda_dynl_push(rda_dynl_t* list, void* data);

/**
 * @brief pop data out of a dynamically allocated list,
 *  while also making sure to shift down / coalesce the memory.
 *  this function does not SHRINK the allocated memory, see
 *  @ref rda_dynl_shrink().
 *
 * @param list pointer to a dynamically allocated list.
 * @param index index at which to pop the item.
 * @return data at <index>, popped off the list.
 */
void*
rda_dynl_pop(rda_dynl_t* list, size_t index);

/**
 * @brief get the data at the index specified from a dynamically
 *  allocated list.
 *
 * @param list the dynamically allocated list to get information from.
 * @param index the index in <list> that we are to retrieve data from.
 * @return 0x0 if the index is out of bounds or the data at <index> in <list>.
 */
void*
rda_dynl_get(rda_dynl_t* list, size_t index);

/**
 * @brief shrinks the list to the length via realloc.
 *
 * @param list the dynamically allocated list to be shrinked.
 */
void
rda_dynl_shrink(rda_dynl_t* list);
#endif //LRDA_DYNL_H
