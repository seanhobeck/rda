/**
 *
 *	@author Sean Hobeck
 *	@date 15/09/2025
 *
 */
#ifndef LRDA_H
#define LRDA_H

/// @note a structure specific to the context provided to librda.
typedef struct {
	// whether to print to stdout or not (1 to print, 0 to not).
	int verbose:1;
	// if unrecognized opcodes should also be listed (1 if not, 0 if they should).
	int ignore_uopc:1;
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
#endif //LRDA_H
