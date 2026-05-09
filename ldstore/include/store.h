#ifndef STORE_H
#define STORE_H
#include<stdint.h>

#define LD_MAGIC
#define LD_VERSION
#define LD_MAX_NAME
#define LD_MAX_HASH
#define LD_MAX_USERS
#define LD_MAX_GROUPS
#define LD_MAX_UGROUPS
#define LD_MAX_GMEMBERS

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
}id_user_t


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


int ld_user_add(ld_db_t, const char *name, const char *password);
int ld_user_del();
int ld_user_get();
int ld_user_list();
int ld_user_auth();

int ld_group_add();
int ld_group_del();
int ld_group_adduser();
int ld_group_members();
int ld_group_list();


#endif /* STORE.H */
