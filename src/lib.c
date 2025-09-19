/**
 *	@author Sean Hobeck
 *	@date 18/09/2025
 *
 *	@file lib.c
 */
#include "lib.h"

/**
 * @brief begin providing context to librda.
 *
 * @param ctx context provided to librda; if null,
 *	librda will assume default settings (0 on all-
 *	options in rda_context_t).
 */
void
rda_begin(rda_context_t *ctx) {
    if (!ctx) {
        // set to default values.
        g_ctx = (rda_context_t) {
            .verbose = 0,
        };
        return;
    };

    // otherwise, set the context.
    g_ctx = *ctx;
};

#pragma region .ctor/dtor
/// @brief load anything required on usage of the library.
__attribute__((constructor))
static void load() {};

/// @brief unload anything we loaded for this library in <load>.
__attribute__((destructor))
static void unload() {};
#pragma endregion