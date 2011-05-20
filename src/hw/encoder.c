#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

int encode_str(const char *str, char *out) {
	int n = sprintf(out, "%lu:%s",strlen(str),str);
	return n;
}

int encode_int(int i, char *out) {
	int n = sprintf(out, "i%ie", i);
	return n;
}

int encode_float(float f, char *out) {
	int n = sprintf(out, "f%fe", f);
	return n;
}

int main(int argc, char **argv) {
	char out[255];

	encode_str("test", out);
	printf("%s\n", out);

	memset(out,0x00,255);
	encode_int(23, out);
	printf("%s\n", out);

	memset(out,0x00,255);
	encode_float(23.5, out);
	printf("%s\n", out);

	return 0;
}
