#ifndef _BE_H
#define _BE_H

#include <stdint.h>
#include <glib.h>
#include "s4_be.h"
#include <s4.h>
#include "pat.h"
#include "bpt.h"
#ifdef _WIN32
#include <windows.h>
#endif

struct s4be_St {
#ifdef _WIN32
	HANDLE fd;
	HANDLE fw;
#else
	int fd;
#endif
	void *map;
	int size;
	GStaticRWLock rwlock;
	GThread *s_thread;
	GCond *cond;
	GMutex *cond_mutex;
};


#define S4_PNT(s, i, t) ((t*)((char*)(s)->map + (i)))
#define S4_ALIGN(a, b) ((a) + (((b) - ((a) % (b))) % (b)))

#define S4_STRING_STORE 0
#define S4_INT_STORE (sizeof (pat_trie_t))
#define S4_REV_STORE (sizeof (pat_trie_t) + sizeof (bpt_t))

int32_t be_alloc (s4be_t *s4, int n);
void be_free (s4be_t *s4, int32_t off, int size);

void be_rlock (s4be_t *s4);
void be_runlock (s4be_t *s4);
void be_wlock (s4be_t *s4);
void be_wunlock (s4be_t *s4);

int _st_recover (s4be_t *old, s4be_t *rec);
int _ip_recover (s4be_t *old, s4be_t *rec);

#endif /* _BE_H */
