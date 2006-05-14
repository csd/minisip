/*
 Copyright (C) 2004-2006 the Minisip Team
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2006 
 *
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#include<config.h>

#include <stdio.h>
#include <stdint.h>
#include <openssl/evp.h>

#ifdef OPENSSL_SYS_WIN32
#include <windows.h>
#endif
#if defined SOLARIS && !defined HAVE_LIBPTHREAD
#include <synch.h>
#include <thread.h>
#endif
#if defined HAVE_LIBPTHREAD && !defined SOLARIS
#include <pthread.h>
#endif

static void thread_setup(void);
static void thread_cleanup(void);
static void my_locking_callback(int, int, const char *, int);


static uint8_t openSSLinit = 0;

static void thread_setup(void);


/*
 * Method:    initializeOpenSSL
 */
void initializeOpenSSL() {

    if (openSSLinit) {
	return;
    }
    openSSLinit = 1;
    thread_setup();
//    OpenSSL_add_all_ciphers();
//    OpenSSL_add_all_digests();
}


#ifdef OPENSSL_SYS_WIN32

static HANDLE *lock_cs;

static void thread_setup(void) {
    int i;

    lock_cs=OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
    for (i=0; i<CRYPTO_num_locks(); i++) {
	lock_cs[i]=CreateMutex(NULL,FALSE,NULL);
    }

    CRYPTO_set_locking_callback((void (*)(int,int,const char *,int))my_locking_callback);
    /* id callback defined */
}

static void thread_cleanup(void) {
    int i;

    CRYPTO_set_locking_callback(NULL);
    for (i=0; i<CRYPTO_num_locks(); i++) {
	CloseHandle(lock_cs[i]);
    }
    OPENSSL_free(lock_cs);
}

static void my_locking_callback(int mode, int type, const char *file, int line) {
    if (mode & CRYPTO_LOCK) {
	WaitForSingleObject(lock_cs[type],INFINITE);
    }
    else {
	ReleaseMutex(lock_cs[type]);
    }
}

#endif /* OPENSSL_SYS_WIN32 */


#if defined SOLARIS && !defined HAVE_LIBPTHREAD

static mutex_t *lock_cs;
static long *lock_count;

static void thread_setup(void) {
    int i;

    lock_cs=OPENSSL_malloc(CRYPTO_num_locks() * sizeof(mutex_t));
    lock_count=OPENSSL_malloc(CRYPTO_num_locks() * sizeof(long));
    for (i=0; i<CRYPTO_num_locks(); i++) {
	lock_count[i]=0;
	/* rwlock_init(&(lock_cs[i]),USYNC_THREAD,NULL); */
	mutex_init(&(lock_cs[i]),USYNC_THREAD,NULL);
    }

    // CRYPTO_set_id_callback((unsigned long (*)())solaris_thread_id);
    CRYPTO_set_locking_callback((void (*)(int,int,const char *,int))my_locking_callback);
}

static void thread_cleanup(void) {
    int i;

    CRYPTO_set_locking_callback(NULL);

    fprintf(stderr,"cleanup\n");

    for (i=0; i<CRYPTO_num_locks(); i++) {
	/* rwlock_destroy(&(lock_cs[i])); */
	mutex_destroy(&(lock_cs[i]));
	fprintf(stderr,"%8ld:%s\n",lock_count[i],CRYPTO_get_lock_name(i));
    }
    OPENSSL_free(lock_cs);
    OPENSSL_free(lock_count);
}

static void my_locking_callback(int mode, int type, const char *file, int line) {
#ifdef undef
    fprintf(stderr,"thread=%4d mode=%s lock=%s %s:%d\n",
	    CRYPTO_thread_id(),
	    (mode&CRYPTO_LOCK)?"l":"u",
	    (type&CRYPTO_READ)?"r":"w",file,line);
#endif

    /*
      if (CRYPTO_LOCK_SSL_CERT == type)
      fprintf(stderr,"(t,m,f,l) %ld %d %s %d\n",
      CRYPTO_thread_id(),
      mode,file,line);
    */
    if (mode & CRYPTO_LOCK) {
	mutex_lock(&(lock_cs[type]));
	lock_count[type]++;
    }
    else {
	mutex_unlock(&(lock_cs[type]));
    }
}

static unsigned long solaris_thread_id(void) {
    unsigned long ret;

    ret=(unsigned long)thr_self();
    return(ret);
}
#endif /* SOLARIS */


#if defined HAVE_LIBPTHREAD && !defined SOLARIS

static pthread_mutex_t *lock_cs;
static long *lock_count;

static void thread_setup(void) {
    int i;

    lock_cs = (pthread_mutex_t *)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
    lock_count = (long *)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(long));
    for (i=0; i<CRYPTO_num_locks(); i++) {
	lock_count[i]=0;
	pthread_mutex_init(&(lock_cs[i]),NULL);
    }

    // CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
    CRYPTO_set_locking_callback((void (*)(int,int,const char *,int))my_locking_callback);
}

static void thread_cleanup(void) {
    int i;

    CRYPTO_set_locking_callback(NULL);
    fprintf(stderr,"cleanup\n");
    for (i=0; i<CRYPTO_num_locks(); i++) {
	pthread_mutex_destroy(&(lock_cs[i]));
	fprintf(stderr,"%8ld:%s\n",lock_count[i],
		CRYPTO_get_lock_name(i));
    }
    OPENSSL_free(lock_cs);
    OPENSSL_free(lock_count);
}

static void my_locking_callback(int mode, int type, const char *file,
				int line) {
#ifdef undef
    fprintf(stderr,"thread=%4d mode=%s lock=%s %s:%d\n",
	    CRYPTO_thread_id(),
	    (mode&CRYPTO_LOCK)?"l":"u",
	    (type&CRYPTO_READ)?"r":"w",file,line);
#endif
    if (mode & CRYPTO_LOCK) {
	pthread_mutex_lock(&(lock_cs[type]));
	lock_count[type]++;
    }
    else {
	pthread_mutex_unlock(&(lock_cs[type]));
    }
}

static unsigned long pthreads_thread_id(void) {
    unsigned long ret;

    ret=(unsigned long)pthread_self();
    return(ret);
}

#endif /* LIBPTHREAD && !SOLARIS */
