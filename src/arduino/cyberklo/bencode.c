/*
 * bencode.c 
 * 	(c) 2011, Leon Szpilewski, http://jsz.github.com
 * 	LICENSE: GPL3
 *
 * 	A simple bencoding encoder with support for floats.
 * 	This is a quick and dirty implementation. Watch out to allocate enough
 * 	memory! There is no bounds checking done.
 *
 * 	Don't use float encoding if you try to do something with torrents. Floats
 * 	are not part of the bencoding spec! I need them to transmit fuzzy values 
 * 	whose accuracy isn't critical. If you use this for any bencoding stream that 
 * 	needs to be	hashed you will fail!
 *
 *  Note: The dictionary creation does not conform to the protocol specs. Keys
 *  are not sorted lexically!
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct benc_t 
{
	char *buf;		//the otuput buffer
	size_t len;					
	size_t cap;
};

//creates a new builder and allocates memory
struct benc_t benc_new(size_t cap) 
{
	struct benc_t sb;
	sb.buf = malloc(cap);
	memset(sb.buf, 0x00, cap);
	sb.len = 0;
	sb.cap = cap;
	return sb;
}

//creates a new builder but uses memory @ buf.
//memory will be initialized with 0x00
struct benc_t benc_new_mem(char *buf, size_t len)
{
	struct benc_t sb;
	sb.buf = buf;
	memset(sb.buf, 0x00, len);
	sb.len = 0;
	sb.cap = len;
	return sb;
}

//reset buf's contents to 0x00 and the position pointer to start
void benc_reset(struct benc_t *b)
{
	memset(b->buf, 0x00, b->cap);
	b->len = 0;
}

//frees memory allocated by builder_new
void benc_free(struct benc_t *b) 
{
	free(b->buf);
	b->buf = 0;
	b->len = 0;
	b->cap = 0;
}

/////////////////////////////////////////////elements
void benc_int(struct benc_t *b, int i)
{
	char *p = b->buf + b->len;
	int n = sprintf(p, "i%ie", i);
	b->len += n;
}

void benc_str(struct benc_t *b, const char *s)
{
	char *p = b->buf + b->len;
	int n = sprintf(p, "%lu:%s", strlen(s), s);
	b->len += n;
}

//meant to append other bencoded streams to b
void benc_raw(struct benc_t *b, void *raw, size_t len)
{
	char *p = b->buf + b->len;
	memcpy(p, raw, len);
	b->len += len;
}

//float is not part of the bencoding spec but i want!
void benc_float(struct benc_t *b, float f)
{
	char *p = b->buf + b->len;
	int n = sprintf(p, "f%fe", f);
	b->len += n;
}

//////////////////////////////////////// DICTIONARY
void benc_dict_start(struct benc_t *b)
{
	char *p = b->buf + b->len;
	strcat(p, "d");
	b->len++;
}

//key, integer val
void benc_dict_kv_i(struct benc_t *b, const char *key, int val)
{
	benc_str(b, key);
	benc_int(b, val);
}

void benc_dict_kv_f(struct benc_t *b, const char *key, float val)
{
	benc_str(b, key);
	benc_float(b, val);
}

void benc_dict_kv_s(struct benc_t *b, const char *key, const char *val)
{
	benc_str(b, key);
	benc_str(b, val);
}

//that's what you want to use for lists, dictionaries
void benc_dict_kv(struct benc_t *b, const char *key, struct benc_t *val)
{
	benc_str(b, key);
	benc_raw(b, val->buf, val->len);
}

void benc_dict_end(struct benc_t *b)
{
	char *p = b->buf + b->len;
	strcat(p, "e");
	b->len ++;
}

///////////////////////////////// LIST
void benc_list_start(struct benc_t *b)
{
	char *p = b->buf + b->len;
	strcat(p, "l");
	b->len ++;
}

void benc_list_add(struct benc_t *b, struct benc_t *e)
{
	benc_raw(b, e->buf, e->len);
}

void benc_list_end(struct benc_t *b)
{
	char *p = b->buf + b->len;
	strcat(p, "e");
	b->len ++;
}


#if 0
int main(int argc, char **argv)
{
	//memory setup
	void *big_buffer[0x1000];
	void *buf1 = big_buffer;
	void *buf2 = buf1+0x250;
	void *buf3 = buf2+0x250;
	void *buf4 = buf3+0x250;
	size_t chunk_len = 0x250;

	//let's encode some simple stuff!
	struct benc_t b = benc_new_mem(buf1, chunk_len);
	benc_int(&b, 23);
	printf("1. 23: %s\n", b.buf);

	benc_str(&b, "ficken");
	printf("2. 23+ficken: %s\n", b.buf);

	benc_float(&b, 44.245);
	printf("3. 23,'ficken',44.245: %s\n", b.buf);
	
	//now a dict
	benc_reset(&b);
	benc_dict_start(&b);
	benc_dict_kv_i(&b,"age",23);
	benc_dict_kv_f(&b,"weight", 88.4f);
	benc_dict_end(&b);
	printf("4. dict: %s\n", b.buf);

	//let's put that dict into another dict
	struct benc_t c = benc_new_mem(buf2, chunk_len);
	benc_dict_start(&c);
	benc_dict_kv_i(&c,"version",2);
	benc_dict_kv(&c,"info_dict", &b);
	benc_dict_end(&c);
	printf("5. dict2: %s\n", c.buf);

	//now a list
	struct benc_t d = benc_new_mem(buf3, chunk_len);
	benc_list_start(&d);
	
		struct benc_t tmp = benc_new_mem(buf4, chunk_len);
		benc_int(&tmp, 44);
		benc_list_add(&d, &tmp);
	
		benc_reset(&tmp);
		benc_float(&tmp, 23.5);
		benc_list_add(&d, &tmp);

		benc_reset(&tmp);
		benc_str(&tmp, "lol");
		benc_list_add(&d, &tmp);

	//let's add the dict to the list
	benc_list_add(&d, &c);

	benc_list_end(&d);
	printf("6. list: %s\n", d.buf);

	return 0;
}
#endif
