/**
 *	@author Sean Hobeck
 *	@date 23/10/2025
 *
 *	@file lib.h
 */
#ifndef LRDA_LIB_H
#define LRDA_LIB_H

/*! @uses bool, true, false */
#include <stdbool.h>

/// @note a structure specific to the context provided to librda.
typedef struct {
	// whether to print to stdout or not.
	bool verbose;
	// whether to use simd instructions in decoding.
	bool use_simd;
} rda_context_t;

/**
 * @brief begin providing context to librda.
 *
 * @param ctx context provided to librda; if null,
 *	librda will assume default settings (0 on all-
 *	options in rda_context_t).
 */
void
rda_begin(rda_context_t ctx);

/**
 * @brief get the current context used by librda.
 *
 * @return the current context used by librda.
 */
rda_context_t
rda_get_context();

/*!
 * @note all internal functions are labeled with this, the attribute
 *	essentially tells gcc to not allow other shared objects/.so to
 *	link against this function at all.
 */
#define rda_internal __attribute__((visibility("internal")))
#endif //LRDA_LIB_H
