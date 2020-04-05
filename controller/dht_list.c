#include <sys/socket.h>
#include "../include/public.h"
#include "./dht/dht.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <nng/nng.h>
#include <nng/protocol/reqrep0/rep.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/supplemental/util/platform.h>

// Parallel is the maximum number of outstanding requests we can handle.
// This is *NOT* the number of threads in use, but instead represents
// outstanding work items.  Select a small number to reduce memory size.
// (Each one of these can be thought of as a request-reply loop.)
#ifndef PARALLEL
#define PARALLEL 32
#endif

// The server keeps a list of work items, sorted by expiration time,
// so that we can use this to set the timeout to the correct value for
// use in poll.
struct work {
    enum { INIT, RECV, WAIT, SEND } state;
    nng_aio *  aio;
    nng_socket sock;
    nng_msg *  msg;
};

void fatal(const char *func, int rv)
{
    fprintf(stderr, "%s: %s\n", func, nng_strerror(rv));
    exit(1);
}

/* The call-back function is called by the DHT whenever something
   interesting happens.  Right now, it only happens when we get a new value or
   when a search completes, but this may be extended in future versions. */
static void callback(void *closure,
         int event,
         const unsigned char *info_hash,
         const void *data, size_t data_len)
{
    if(event == DHT_EVENT_SEARCH_DONE)
        printf("Search done.\n");
    else if(event == DHT_EVENT_SEARCH_DONE6)
        printf("IPv6 search done.\n");
    else if(event == DHT_EVENT_VALUES)
        printf("Received %d values.\n", (int)(data_len / 6));
    else if(event == DHT_EVENT_VALUES6)
        printf("Received %d IPv6 values.\n", (int)(data_len / 18));
    else
        printf("Unknown DHT event %d.\n", event);
}

void server_cb(void *arg)
{
    struct work *work = arg;
    nng_msg *    msg;
    int          rv;
    uint32_t     when;

    switch (work->state) {
        case INIT:
            work->state = RECV;
            nng_recv_aio(work->sock, work->aio);
            break;
        case RECV:
            if ((rv = nng_aio_result(work->aio)) != 0) {
                fatal("nng_recv_aio", rv);
            }
            msg = nng_aio_get_msg(work->aio);
            if ((rv = nng_msg_trim_u32(msg, &when)) != 0) {
                // bad message, just ignore it.
                nng_msg_free(msg);
                nng_recv_aio(work->sock, work->aio);
                return;
            }
            work->msg   = msg;
            work->state = WAIT;
            nng_sleep_aio(when, work->aio);
            break;
        case WAIT:
            // We could add more data to the message here.
            nng_aio_set_msg(work->aio, work->msg);
            work->msg   = NULL;
            work->state = SEND;
            nng_send_aio(work->sock, work->aio);
            break;
        case SEND:
            if ((rv = nng_aio_result(work->aio)) != 0) {
                nng_msg_free(work->msg);
                fatal("nng_send_aio", rv);
            }
            work->state = RECV;
            nng_recv_aio(work->sock, work->aio);
            break;
        default:
            fatal("bad state!", NNG_ESTATE);
            break;
    }

    // todo: getfd
    if(rc > 0) {
        buf[rc] = '\0';
        rc = dht_periodic(buf, rc, (struct sockaddr*)&from, fromlen,
                          &tosleep, callback, NULL);
    } else {
        rc = dht_periodic(NULL, 0, NULL, 0, &tosleep, callback, NULL);
    }

    /* This is how you trigger a search for a torrent hash.  If port
   (the second argument) is non-zero, it also performs an announce.
   Since peers expire announced data after 30 minutes, it is a good
   idea to reannounce every 28 minutes or so. */
    if(searching) {
        if(s >= 0)
            dht_search(hash, 0, AF_INET, callback, NULL);
        if(s6 >= 0)
            dht_search(hash, 0, AF_INET6, callback, NULL);
        searching = 0;
    }

    /* For debugging, or idle curiosity. */
    if(dumping) {
        dht_dump_tables(stdout);
        dumping = 0;
    }
    {
        struct sockaddr_in sin[500];
        struct sockaddr_in6 sin6[500];
        int num = 500, num6 = 500;
        int i;
        i = dht_get_nodes(sin, &num, sin6, &num6);
        printf("Found %d (%d + %d) good nodes.\n", i, num, num6);
    }

}

struct work *
alloc_work(nng_socket sock)
{
    struct work *w;
    int          rv;

    if ((w = nng_alloc(sizeof(*w))) == NULL) {
        fatal("nng_alloc", NNG_ENOMEM);
    }
    if ((rv = nng_aio_alloc(&w->aio, server_cb, w)) != 0) {
        fatal("nng_aio_alloc", rv);
    }
    w->state = INIT;
    w->sock  = sock;
    return (w);
}

// The server runs forever.
int
server(const char *url)
{
    nng_socket   sock;
    struct work *works[PARALLEL];
    int          rv;
    int          i;

    /*  Create the socket. */
    rv = nng_rep0_open_raw(&sock);
    if (rv != 0) {
        fatal("nng_rep0_open", rv);
    }

    for (i = 0; i < PARALLEL; i++) {
        works[i] = alloc_work(sock);
    }

    if ((rv = nng_listen(sock, url, NULL, 0)) != 0) {
        fatal("nng_listen", rv);
    }

    for (i = 0; i < PARALLEL; i++) {
        server_cb(works[i]); // this starts them going (INIT state)
    }

    for (;;) {
        nng_msleep(3600000); // neither pause() nor sleep() portable
    }
}

/*  The client runs just once, and then returns. */
int
client(const char *url, const char *msecstr, int len)
{
    nng_socket sock;
    int        rv;
    nng_msg *  msg;
    nng_time   start;
    nng_time   end;
    unsigned   msec;

    msec = atoi(msecstr) * 1000;

    if ((rv = nng_req0_open(&sock)) != 0) {
        fatal("nng_req0_open", rv);
    }

    if ((rv = nng_dial(sock, url, NULL, 0)) != 0) {
        fatal("nng_dial", rv);
    }

    start = nng_clock();

    if ((rv = nng_msg_alloc(&msg, 0)) != 0) {
        fatal("nng_msg_alloc", rv);
    }
    if ((rv = nng_msg_append_u32(msg, msec)) != 0) {
        fatal("nng_msg_append_u32", rv);
    }

    if ((rv = nng_sendmsg(sock, msg, 0)) != 0) {
        fatal("nng_send", rv);
    }

    if ((rv = nng_recvmsg(sock, &msg, 0)) != 0) {
        fatal("nng_recvmsg", rv);
    }
    end = nng_clock();
    nng_msg_free(msg);
    nng_close(sock);

    printf("Request took %u milliseconds.\n", (uint32_t)(end - start));
    return (0);
}

/* Functions called by the DHT. */

int dht_sendto(int sockfd, const void *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
    return client(url, buf, len)
    // return sendto(sockfd, buf, len, flags, to, tolen);
}

int
dht_blacklisted(const struct sockaddr *sa, int salen)
{
    return 0;
}

/* We need to provide a reasonably strong cryptographic hashing function.
   Here's how we'd do it if we had RSA's MD5 code. */
#if 0
void
dht_hash(void *hash_return, int hash_size,
         const void *v1, int len1,
         const void *v2, int len2,
         const void *v3, int len3)
{
    static MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, v1, len1);
    MD5Update(&ctx, v2, len2);
    MD5Update(&ctx, v3, len3);
    MD5Final(&ctx);
    if(hash_size > 16)
        memset((char*)hash_return + 16, 0, hash_size - 16);
    memcpy(hash_return, ctx.digest, hash_size > 16 ? 16 : hash_size);
}
#else
/* But for this toy example, we might as well use something weaker. */
void
dht_hash(void *hash_return, int hash_size,
         const void *v1, int len1,
         const void *v2, int len2,
         const void *v3, int len3)
{
    const char *c1 = v1, *c2 = v2, *c3 = v3;
    char key[9];                /* crypt is limited to 8 characters */
    int i;

    memset(key, 0, 9);
#define CRYPT_HAPPY(c) ((c % 0x60) + 0x20)

    for(i = 0; i < 2 && i < len1; i++)
        key[i] = CRYPT_HAPPY(c1[i]);
    for(i = 0; i < 4 && i < len1; i++)
        key[2 + i] = CRYPT_HAPPY(c2[i]);
    for(i = 0; i < 2 && i < len1; i++)
        key[6 + i] = CRYPT_HAPPY(c3[i]);
    //strncpy(hash_return, crypt(key, "jc"), hash_size);
}
#endif

int
dht_random_bytes(void *buf, size_t size)
{
    int fd, rc, save;

    fd = open("/dev/urandom", O_RDONLY);
    if(fd < 0)
        return -1;

    rc = read(fd, buf, size);

    save = errno;
    close(fd);
    errno = save;

    return rc;
}

static char *sock = null;
int dht_list_init(char *url)
{
    char myid[20] = "";

    server(url);
    sock = url;

    // step1: 初始化dht
    /* Init the dht. */
    int rc = dht_init(0, 0, myid, (unsigned char*)"JC\0\0");
    if(rc < 0) {
        perror("dht_init");
        exit(1);
    }

   // Step2: 找到 bootstrap nodes, 配置在配置文件里
    /* For bootstrapping, we need an initial list of nodes.  This could be
   hard-wired, but can also be obtained from the nodes key of a torrent
   file, or from the PORT bittorrent message.

   Dht_ping_node is the brutal way of bootstrapping -- it actually
   sends a message to the peer.  If you know the nodes' ids, it is
   better to use dht_insert_node. */
    for(int i = 0; i < num_bootstrap_nodes; i++) {
        socklen_t salen;
        if(bootstrap_nodes[i].ss_family == AF_INET)
            salen = sizeof(struct sockaddr_in);
        else
            salen = sizeof(struct sockaddr_in6);
        dht_ping_node((struct sockaddr*)&bootstrap_nodes[i], salen);
        /* Don't overload the DHT, or it will drop your nodes. */
        if(i <= 128)
            usleep(random() % 10000);
        else
            usleep(500000 + random() % 400000);
    }

    // step3: bind到一个端口




    return 0;
}

void dht_list_fini()
{

}
