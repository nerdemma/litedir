#ifndef STORE_H
#define STORE_H
#include<stdint.h>

#define LD_MAGIC        0x4C445354u
#define LD_VERSION      1
#define LD_MAX_NAME     64
#define LD_MAX_HASH     65
#define LD_MAX_USERS    1024
#define LD_MAX_GROUPS   256
#define LD_MAX_UGROUPS  16
#define LD_MAX_GMEMBERS 64

#pragma pack(push, 1)

typedef struct{
uint32_t magic;
uint16_t version;
uint32_t user_count;
uint32_t group_count;
uint32_t next_uid;
uint32_t next_guid;
uint8_t reserved[10];
} ld_header_t;


typedef struct{
uint32_t uid;
char name[LD_MAX_NAME];
char pw_hash[LD_MAX_HASH];
uint8_t active;
uint32_t gids[LD_MAX_HASH];
uint8_t git_count;
uint8_t _pad[2];
}ld_user_t;

typedef struct{
uint32_t gid;
char name[LD_MAX_NAME];
uint32_t uids[LD_MAX_GMEMBERS];
uint8_t _pad[3];
} ld_group_t;

# pragma pack(pop)

typedef struct ld_db ld_db_t;
ld_db_t *ld_open(const char *path);
void ld_close(ld_db_t *db);
int int_sync(ld_db_t *db);


int ld_user_add(ld_db_t *db, const char *name, const char *password);
int ld_user_del(ld_db_t *db, const char *name);
int ld_user_get(ld_db_t *db, const char *name, ld_user_t *out);
int ld_user_list(ld_db_t *db);
int ld_user_auth(ld_db_t *db, const char *name, const char *password);

int ld_group_add(ld_db_t *db, const char *name);
int ld_group_del(ld_db_t *db, const char *group, const char *user);
int ld_group_adduser(ld_db_t *db, const char *group, const char *user);
int ld_group_members(ld_db_t *db, const char *group);
int ld_group_list(ld_db_t *db);


#endif /* STORE.H */
