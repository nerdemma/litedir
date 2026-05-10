#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../lib/store.h"

struct ld_db
{
int		fd;
ld_header_t	hdr;
ld_user_t	users[LD_MAX_USERS];
ld_group_t	groups[LD_MAX_GROUPS];
};

ld_header *db_header(ld_db_t *db) { return &db->hdr; }
ld_header *db_users(ld_db_t *db) { return &db->users; }
ld_header *db_group(ld_db_t *db) { return &db->groups; }

static off_t user_offset(int i) { return (off_t)sizeof(ld_header_t) + (off_t)i * (off_t)sizeof(ld_user_t); }	
static off_t group_offset(int j) { return user_offset(LD_MAX_USERS) + (off_t)j * (off_t)sizeof(ld_group_t); }

ld_db_t *ld_open(const char *path)
{
ld_db_t *db = calloc(1, sizeof(*db));
	if(!db) return NULL;

db->fd = open(path, O_RDWR | O_CREAT, 0600);
	if(db->fd < 0) { free(db); return NULL; }
ssize_t n = pread(db->fd, &db->hdr, sizeof(db->hdr), 0);
	
	if( n == 0 )
	{
	db->hdr.magic = LD_MAGIC;
	db->hdr.version = LD_VERSION;
       	db->hdr.next_uid = 1000;
	db->hdr.next_gid = 1000;
	ld_sync(db);	

	}

	else if(n != (ssize_t) sizeof(db->hdr) || db-> hdr.magic != LD_MAGIC)
	{
	fprintf(stderr, "ldstore: file corrupted or magic incorrect!\n");
	close(db->fd); free(db); return NULL;
	}
	else {
	pread(db->fd, db->users, sizeof(db->users), user_offset(0));
	pread(db->fd, db->groups, sizeof(db->groups), group_offset(0));
	}
return db;

}

int ld_sync(ld_db_t *db)
{
	if(pwrite(db->fd, &db->hdr, sizeof(db->hdr), 0) < 0) return -1;
	if(pwrite(db->fd, &db->users, sizeof(db->users), user_offset(0)) < 0) return -1;
	if(pwrite(db->fd, &db->groups, sizeof(db->groups), group_offset(0)) < 0) return -1;
	return fsync(db->fd);

}

void ld_close(ld_db_t *db)
{
if(!db) return;
ld_sync(db);
close(db->fd);
free(db);
}

