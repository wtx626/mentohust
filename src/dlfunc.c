/*
* Copyright (C) 2009, HustMoon Studio
*
* 文件名称：dlfunc.c
* 摘	要：动态载入库函数
* 作	者：HustMoon@BYHH
* 邮	箱：www.ehust@gmail.com
* 日	期：2009.11.11
*/
#ifndef NODLL
#include "dlfunc.h"
#include <dlfcn.h>

int (*pcap_findalldevs)(pcap_if_t **, char *);
void (*pcap_freealldevs)(pcap_if_t *);
pcap_t *(*pcap_open_live)(const char *, int, int, int, char *);
int (*pcap_compile)(pcap_t *, struct bpf_program *, const char *, int, bpf_u_int32);
int (*pcap_setfilter)(pcap_t *, struct bpf_program *);
char *(*pcap_geterr)(pcap_t *);
void (*pcap_freecode)(struct bpf_program *);
int (*pcap_loop)(pcap_t *, int, pcap_handler, unsigned char *);
void (*pcap_close)(pcap_t *);
void (*pcap_breakloop)(pcap_t *);
int (*pcap_sendpacket)(pcap_t *, const unsigned char *, int);

static void *libpcap = NULL;

int load_libpcap(void) {
	float ver;
	char file[20], *error;
	for (ver=1.1; ver>0.7; ver-=0.1) {
		sprintf(file, "libpcap.so.%.1f", ver);
		libpcap = dlopen(file, RTLD_LAZY);
		error = dlerror();
		if (libpcap != NULL)
			break;
	}
	if (libpcap == NULL) {
		printf("!! 打开libpcap.so.1.1、libpcap.so.1.0、libpcap.so.0.9、libpcap.so.0.8失败，请检查是否已安装某个版本的libpcap。\n");
		return -1;
	}
	if ((pcap_findalldevs = dlsym(libpcap, "pcap_findalldevs"), error = dlerror()) != NULL
		|| (pcap_freealldevs = dlsym(libpcap, "pcap_freealldevs"), error = dlerror()) != NULL
		|| (pcap_open_live = dlsym(libpcap, "pcap_open_live"), error = dlerror()) != NULL
		|| (pcap_compile = dlsym(libpcap, "pcap_compile"), error = dlerror()) != NULL
		|| (pcap_setfilter = dlsym(libpcap, "pcap_setfilter"), error = dlerror()) != NULL
		|| (pcap_geterr = dlsym(libpcap, "pcap_geterr"), error = dlerror()) != NULL
		|| (pcap_freecode = dlsym(libpcap, "pcap_freecode"), error = dlerror()) != NULL
		|| (pcap_loop = dlsym(libpcap, "pcap_loop"), error = dlerror()) != NULL
		|| (pcap_close = dlsym(libpcap, "pcap_close"), error = dlerror()) != NULL
		|| (pcap_breakloop = dlsym(libpcap, "pcap_breakloop"), error = dlerror()) != NULL
		|| (pcap_sendpacket = dlsym(libpcap, "pcap_sendpacket"), error = dlerror()) != NULL) {
		printf("!! 从%s获取函数失败: %s\n", file, error);
		free_libpcap();
		return -1;
	}
	return 0;
}

void free_libpcap(void) {
	if (libpcap) {
		dlclose(libpcap);
		dlerror();
		libpcap = NULL;
	}
}

#endif	/* NODLL */

#ifndef NONOTIFY
#include "dlfunc.h"
#include <dlfcn.h>

typedef void NotifyNotification, GtkWidget, GError;
typedef char gchar;
typedef int gint, gboolean;

static gboolean (*notify_notification_update)(NotifyNotification *, const gchar *,
				const gchar *, const gchar *);
static void (*notify_notification_set_timeout)(NotifyNotification *, gint);
static gboolean (*notify_notification_show)(NotifyNotification *, GError **);

static void *libnotify = NULL;
static NotifyNotification *notify = NULL;

int load_libnotify(void) {
	char *error;
	gboolean (*notify_init)(const char *);
	NotifyNotification *(*notify_notification_new)(const gchar *, const gchar *,
					const gchar *, GtkWidget *);
	libnotify = dlopen("libnotify.so.1", RTLD_LAZY);
	error = dlerror();
	if (libnotify == NULL) {
		printf("!! 打开libnotify.so.1失败: %s\n", error);
		return -1;
	}
	if ((notify_init = dlsym(libnotify, "notify_init"), error = dlerror()) != NULL
		|| (notify_notification_new = dlsym(libnotify, "notify_notification_new"), error = dlerror()) != NULL
		|| (notify_notification_show = dlsym(libnotify, "notify_notification_show"), error = dlerror()) != NULL
		|| (notify_notification_update = dlsym(libnotify, "notify_notification_update"), error = dlerror()) != NULL
		|| (notify_notification_set_timeout = dlsym(libnotify, "notify_notification_set_timeout"), error = dlerror()) != NULL) {
		printf("!! 从libnotify.so.1获取函数失败: %s\n", error);
		free_libnotify();
		return -1;
	}
	if (!notify_init("mentohust")) {
		printf("!! 初始化libnotify失败。\n");
		free_libnotify();
		return -1;
	}
	notify = notify_notification_new("MentoHUST", NULL, NULL, NULL);
	return 0;
}

void free_libnotify(void) {
	void (*notify_uninit)(void);
	if (notify) {
		notify_uninit = dlsym(libnotify, "notify_uninit");
		if (!dlerror())
			notify_uninit();
		notify = NULL;
	}
	if (libnotify) {
		dlclose(libnotify);
		dlerror();
		libnotify = NULL;
	}
}

void set_timeout(int timeout) {
	notify_notification_set_timeout(notify, timeout);
}

void show_notify(const char *summary, char *body) {
	notify_notification_update(notify, summary, body, NULL);
	notify_notification_show(notify, NULL);
}

#endif	/* NONOTIFY */

