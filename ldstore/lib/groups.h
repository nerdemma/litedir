#ifndef GROUPS_H
#define GROUPS_H

#include "store.h"

int ld_group_add(ld_db_t *db, const char *name);
int ld_group_del(ld_db_t *db, const char *name);
int ld_group_adduser(ld_db_t *db, const char *group, const char *user);
int ld_group_members(ld_db_t *db, const char *group);
int ld_group_list(ld_db_t *db);

#endif /* GROUPS_H */

