#include <stdio.h>

int myfunc(char *str)
{
	/* Size: 4 + 32 + 8 + 8 = 70 */
	int c;
	char mybuf[32], *s, *d;

	for (s = str, d = mybuf, c = 0; *s != '\0'; s++, d++, c++) {
		if (*s == ':')
			*d = ';';
		else
			*d = *s;
	}
	*d = '\0';
	{
		char another_buf[128];
		sprintf(another_buf, "Processed string is `%s'\n", mybuf);
		puts(another_buf);
	}
	return c;
}

int main(int argc, char **argv)
{
	int c = 0;

	if (argc > 1)
		c = myfunc(argv[1]);
	printf("processed %d characters\n", c);
	return 0;
}
