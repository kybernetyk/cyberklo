#pragma once
#ifndef benc_h__
#define benc_h__ 123
#include <stddef.h>
#include <stdlib.h>


struct benc_t {
	char *buf;		//the otuput buffer
	size_t len;
	size_t cap;
};

extern void benc_list_end(struct benc_t *b);
extern void benc_list_add(struct benc_t *b, struct benc_t *e);
extern void benc_list_start(struct benc_t *b);
extern void benc_dict_end(struct benc_t *b);
extern void benc_dict_kv(struct benc_t *b, const char *key, struct benc_t *val);
extern void benc_dict_kv_s(struct benc_t *b, const char *key, const char *val);
extern void benc_dict_kv_f(struct benc_t *b, const char *key, float val);
extern void benc_dict_kv_i(struct benc_t *b, const char *key, int val);
extern void benc_dict_start(struct benc_t *b);
extern void benc_float(struct benc_t *b, float f);
extern void benc_raw(struct benc_t *b, void *raw, size_t len);
extern void benc_str(struct benc_t *b, const char *s);
extern void benc_int(struct benc_t *b, int i);
extern void benc_free(struct benc_t *b);
extern void benc_reset(struct benc_t *b);
extern struct benc_t benc_new_mem(char *buf, size_t len);
extern struct benc_t benc_new(size_t cap);

#endif
