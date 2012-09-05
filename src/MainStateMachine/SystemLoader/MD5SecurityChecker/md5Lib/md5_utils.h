/*
 * MD5 utilities.
 * $Rev: 258 $
 */

#ifndef __MD5_UTILS_H
#define __MD5_UTILS_H

#include "md5.h"

#define MIN_(a,b) (((a)<(b)) ? (a) : (b))


#ifdef __cplusplus
extern "C" 
{
#endif
/*
 * PRE: `digest' is not null and `len' is a non-zero positive
 * POST: returns a not-null buffer holding the hexadecimal representation of `digest'.
 *       The returned buffer must be freed by the user.
 */
char *to_hexa(md5_byte_t *digest, size_t len);

/*
 * PRE: `a_path' is a valid path name and points to a regular file.
 *      `offset' + `len' is less than the size of `a_path'.
 * POST: returns the hexadecimal representation of the MD5 hash for the
 *       slice starting at `offset', spanning `len' bytes, from file at `a_path'.
 *       The returned buffer must be freed by the user.
 */
char *slice_md5(const char *a_path, unsigned int offset, unsigned int len);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif /* __MD5_UTILS_H */
