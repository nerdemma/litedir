#include <string.h>
#include <stdio.h>
#include "../lib/store.h"
#include "../lib/db.h"
 
 
static int find_group(ld_db_t *db, const char *name)
{
    ld_group_t *g = db_groups(db);
    uint32_t    n = db_header(db)->group_count;
 
    for (uint32_t i = 0; i < n; i++) {
        if (g[i].uid_count != 0 || g[i].gid != 0)
            if (strcmp(g[i].name, name) == 0)
                return (int)i;
    }
    return -1;
}
 
 
int ld_group_add(ld_db_t *db, const char *name)
{
    ld_header_t *h = db_header(db);
 
    if (find_group(db, name) >= 0) {
        fprintf(stderr, "error: el grupo '%s' ya existe\n", name);
        return -1;
    }
 
    if (h->group_count >= LD_MAX_GROUPS) {
        fprintf(stderr, "error: limite de grupos alcanzado (%d)\n",
                LD_MAX_GROUPS);
        return -1;
    }
 
    ld_group_t *g = &db_groups(db)[h->group_count];
    memset(g, 0, sizeof(*g));
 
    g->gid = h->next_gid++;
    strlcpy(g->name, name, LD_MAX_NAME);
    h->group_count++;
 
    return ld_sync(db);
}
 
int ld_group_del(ld_db_t *db, const char *name)
{
    int i = find_group(db, name);
    if (i < 0) {
        fprintf(stderr, "error: grupo '%s' no encontrado\n", name);
        return -1;
    }
 
    /* Marcar como eliminado limpiando el slot */
    memset(&db_groups(db)[i], 0, sizeof(ld_group_t));
    db_header(db)->group_count--;
 
    return ld_sync(db);
}
 
int ld_group_adduser(ld_db_t *db, const char *gname, const char *uname)
{
    int gi = find_group(db, gname);
    int ui = find_user_idx(db, uname);
 
    /* FIX: los bloques de error originales no tenian return -1
       — la funcion continuaba con indices invalidos            */
    if (gi < 0) {
        fprintf(stderr, "error: grupo '%s' no encontrado\n", gname);
        return -1;
    }
    if (ui < 0) {
        fprintf(stderr, "error: usuario '%s' no encontrado\n", uname);
        return -1;
    }
 
    ld_group_t *g = &db_groups(db)[gi];
    /* FIX: era &db_users(db)[gi] — usaba el indice del grupo
       en vez del indice del usuario                           */
    ld_user_t  *u = &db_users(db)[ui];
 
    /* FIX: era LX_MAX_GMEMBERS (typo) -> LD_MAX_GMEMBERS     */
    if (g->uid_count >= LD_MAX_GMEMBERS) {
        fprintf(stderr, "error: grupo lleno (max %d)\n", LD_MAX_GMEMBERS);
        return -1;
    }
 
    /* Verificar que el usuario no este ya en el grupo */
    for (uint8_t i = 0; i < g->uid_count; i++)
        if (g->uids[i] == u->uid) return 0;
 
    /* Agregar uid al grupo */
    g->uids[g->uid_count++] = u->uid;
 
    /* Agregar gid al usuario si hay lugar */
    if (u->gid_count < LD_MAX_UGROUPS)
        u->gids[u->gid_count++] = g->gid;
 
    return ld_sync(db);
}
 
int ld_group_members(ld_db_t *db, const char *name)
{
    int gi = find_group(db, name);
    if (gi < 0) {
        fprintf(stderr, "error: grupo '%s' no encontrado\n", name);
        return -1;
    }
 
    ld_group_t *g = &db_groups(db)[gi];
    ld_user_t  *u = db_users(db);
    uint32_t    n = db_header(db)->user_count;
 
    printf("grupo '%s' (gid=%u) — %u miembro(s):\n",
           g->name, g->gid, g->uid_count);
 
    for (uint8_t m = 0; m < g->uid_count; m++) {
        /* Buscar el nombre del usuario por uid */
        for (uint32_t i = 0; i < n; i++) {
            if (u[i].active && u[i].uid == g->uids[m]) {
                printf("  uid=%-6u  %s\n", u[i].uid, u[i].name);
                break;
            }
        }
    }
    return 0;
}
 
int ld_group_list(ld_db_t *db)
{
    ld_group_t *g = db_groups(db);
    uint32_t    n = db_header(db)->group_count;
 
    printf("%-6s  %-32s  %s\n", "GID", "Nombre", "Miembros");
    printf("%.6s  %.32s  %.8s\n",
           "------", "--------------------------------", "--------");
 
    for (uint32_t i = 0; i < n; i++) {
        if (g[i].gid == 0) continue;
        printf("%-6u  %-32s  %u\n",
               g[i].gid, g[i].name, g[i].uid_count);
    }
    return 0;
}
