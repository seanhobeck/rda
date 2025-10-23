/**
 *	@author Sean Hobeck
 *	@date 18/09/2025
 *
 *	@file lib.c
 */
#include "lib.h"

/// @note static context for librda.
rda_context_t g_ctx;

/**
 * @brief begin providing context to librda.
 *
 * @param ctx context provided to librda; if null,
 *	librda will assume default settings (0 on all-
 *	options in rda_context_t).
 */
void
rda_begin(rda_context_t ctx) {
    // set our global context.
    g_ctx = ctx;
};

/**
 * @brief get the current context used by librda.
 *
 * @return the current context used by librda.
 */
rda_context_t
rda_get_context() {
    return g_ctx;
};

#pragma region .ctor/dtor
/// @brief load anything required on usage of the library.
__attribute__((constructor))
static void load() {};

/// @brief unload anything we loaded for this library in <load>.
__attribute__((destructor))
static void unload() {};
#pragma endregion