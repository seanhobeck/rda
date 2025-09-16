/**
 *
 *	@author Sean Hobeck
 *	@date 15/09/2025
 *
 */
#ifndef LRDA_DIS_H
#define LRDA_DIS_H

/*! @uses size_t */
#include <stddef.h>

/**
 * @brief analyze bytes using the amd64 architecture,
 *  given some bytes and a size.
 *
 * @param code the bytes in memory to analyze.
 * @param size the size of bytes in memory to analyze.
 * @return
 */
size_t
rda_analyze_iamd64(const unsigned char* code, size_t size);

#endif //LRDA_DIS_H
