/**
 *	@author Sean Hobeck
 *	@date 18/09/2025
 *
 *	@file lib.h
 */
#ifndef LRDA_LIB_H
#define LRDA_LIB_H

/// @note a structure specific to the context provided to librda.
typedef struct {
	// whether to print to stdout or not (1 to print, 0 to not).
	int verbose:1;
} rda_context_t;

/// @note static context for librda.
static rda_context_t g_ctx;

/**
 * @brief begin providing context to librda.
 *
 * @param ctx context provided to librda; if null,
 *	librda will assume default settings (0 on all-
 *	options in rda_context_t).
 */
void
rda_begin(rda_context_t *ctx);

/*!
 * @note all internal functions are labeled with this, the attribute
 *	essentially tells gcc to not allow other shared objects/.so to
 *	link against this function at all.
 */
#define rda_internal __attribute__((visibility("internal")))
#endif //LRDA_H
