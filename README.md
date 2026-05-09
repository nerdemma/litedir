''LightDir
Light weight user directory


dependences
main.c -> store.h  ' only know public API
users.c-> db.h, auth.h 'read/write users register
groups.c-> db.h 'only use crypt(3) from the base system.
db.c -> none. only POSIX pread/pwrite/open/fsync


