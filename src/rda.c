/**
 *
 *	@author Sean Hobeck
 *	@date 15/09/2025
 *
 */

/// @brief load anything required on usage of the library.
__attribute__((constructor))
static void load() {};

/// @brief unload anything we loaded for this library in <load>.
__attribute__((destructor))
static void unload() {};