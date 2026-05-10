int ld_group_adduser(ld_db_t *db, const char *gname, const char *uname)
{
int gi = find_group(db, gname);
int ui = find_user_idx(db, uname);
	if(gi < 0){ fprintf(stderr, "error: grupo no encontrado  \n");}
	if(ui < 0){ fprintf(stderr, "error: usuario no encontrado\n");}

	
ld_group_t *g = &db_groups(db)[gi]; 
ld_user_t *u = &db_users(db)[gi];

	if(g->uid_count >= LD_MAX_GMEMBERS)
	{
	fprintf(stderr,"error: grupo lleno max (%d)\n ", LX_MAX_GMEMBERS);
	return -1;
	}
	
	for(uint8_t i = 0; i < g->uid_count; i++)
		if(g->uids[i] == u->uid) return 0;

	g->uids[g->uid_count++] = u->uid;
	
	if (u->gid_count < LD_MAX_UGROUPS)
	u->gids[u->gid_count++] = g-> gid;
return ld_sync(db);
}	
