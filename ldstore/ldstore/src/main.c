#define _BSD_SOURCE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../lib/store.h"

#define DB_PATH "./ldstore.db"

static void usage(void)
{
	fprintf(stderr,
		   "usage: ldctl <entity> <action> [args]\n\n"
		   " user add <name> <password>\n"
		   " user del <name>\n"
		   " user list\n"
		   " user auth <name> <password>\n\n"
		   " group add <name>\n"
		   " group del <name>\n"
		   " group adduser <group> <user>\n"
		   " group members <name>\n"
		   " group list\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	if (pledge("stdio rpath wpath cpath", NULL) == -1)
	{
	perror("pledge"); return 1;
	}

	if (argc < 3)
		usage();
	ld_db_t *db = ld_open(DB_PATH);

	if (!db)
	{
		fprintf(stderr, "error: no se pudo abrir %s\n", DB_PATH);
		return 1;
	}
	const char *ent = argv[1], *act = argv[2];
	int ret = 0;

	if (strcmp(ent, "user") == 0)
	{
		if (!strcmp(act, "add") && argc == 5)
			ret = ld_user_add(db, argv[3], argv[4]);
		else if (!strcmp(act, "del") && argc == 4)
			ret = ld_user_del(db, argv[3]);
		else if (!strcmp(act, "list") && argc == 3)
			ret = ld_user_list(db);
		else if (!strcmp(act, "auth") && argc == 5)
		{
			ret = ld_user_auth(db, argv[3], argv[4]);
			printf("%s\n", ret ? "autenticado" : "fallo");
		}
		else
			usage();
	}

	else if (strcmp(ent, "group") == 0)
	{
		if (!strcmp(act, "add") && argc == 4)
			ret = ld_group_add(db, argv[3]);
		else if (!strcmp(act, "del") && argc == 4)
			ret = ld_group_del(db, argv[3], NULL);
		else if (!strcmp(act, "adduser") && argc == 5)
			ret = ld_group_adduser(db, argv[3], argv[4]);
		else if (!strcmp(act, "members") && argc == 4)
			ret = ld_group_members(db, argv[3]);
		else if (!strcmp(act, "list") && argc == 3)
			ret = ld_group_list(db);

		else
			usage();
	}
	else
		usage();
	ld_close(db);
	return (ret == 0) ? 0 : 1;
}
