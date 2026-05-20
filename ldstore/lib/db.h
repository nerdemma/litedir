#ifndef DB_H
#define DB_H
#include "store.h"

ld_db_t *ld_open(const char *path);
void ld_close(ld_db_t *db);
int ld_sync(ld_db_t *db);

ld_header_t *db_header(ld_db_t *db);
ld_user_t *db_users(ld_db_t *db);
ld_group_t *db_groups(ld_db_t *db);

#endif /*DB_H*/
