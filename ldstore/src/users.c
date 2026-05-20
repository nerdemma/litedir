#include <string.h>
#include <stdio.h>
#include "../lib/store.h"
#include "../lib/db.h"
#include "../lib/auth.h"

static int find_user(ld_db_t *db, const char *name)
{
    ld_user_t *u = db_users(db);
    uint32_t   n = db_header(db)->user_count;

    for (uint32_t i = 0; i < n; i++) {
        if (u[i].active && strcmp(u[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

int find_user_idx(ld_db_t *db, const char *name)
{
    return find_user(db, name);
}

int ld_user_add(ld_db_t *db, const char *name, const char *password)
{
    ld_header_t *h = db_header(db);

    if (find_user(db, name) >= 0) {
        fprintf(stderr, "error: el usuario '%s' ya existe\n", name);
        return -1;
    }

    if (h->user_count >= LD_MAX_USERS) {
        fprintf(stderr, "error: limite de usuarios alcanzado (%d)\n",
                LD_MAX_USERS);
        return -1;
    }

    ld_user_t *u = &db_users(db)[h->user_count];
    memset(u, 0, sizeof(*u));

    u->uid    = h->next_uid++;
    u->active = 1;
    strlcpy(u->name, name, LD_MAX_NAME);


    if (ld_hash_password(password, u->pw_hash, LD_MAX_HASH) != 0)
        return -1;
        
    h->user_count++;

    return ld_sync(db);
}

int ld_user_del(ld_db_t *db, const char *name)
{
    int i = find_user(db, name);
    if (i < 0) {
        fprintf(stderr, "error: usuario '%s' no encontrado\n", name);
        return -1;
    }

    db_users(db)[i].active = 0;
    return ld_sync(db);
}

int ld_user_auth(ld_db_t *db, const char *name, const char *password)
{
    int i = find_user(db, name);
    if (i < 0) return -1; 
    return ld_verify_password(password, db_users(db)[i].pw_hash);
}

int ld_user_list(ld_db_t *db)
{
    ld_user_t *u = db_users(db);
    uint32_t n = db_header(db)->user_count;

    printf("%-6s  %-32s  %s\n", "UID", "Nombre", "Grupos");
    printf("%.6s  %.32s  %.6s\n",
           "------", "--------------------------------", "------");

    for (uint32_t i = 0; i < n; i++) {
        if (!u[i].active) continue;

        printf("%-6u  %-32s  %u\n",
               u[i].uid, u[i].name, u[i].gid_count);
    }
    return 0;
}




