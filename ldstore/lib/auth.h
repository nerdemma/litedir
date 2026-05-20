#ifndef AUTH_H
#define AUTH_H

#include <stddef.h>
int ld_hash_password(const char *password, char *out, size_t out_len);
int ld_verify_password(const char *password, const char *stored_hash);

#endif /*AUTH_H*/
