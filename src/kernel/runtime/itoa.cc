#include <os.h>


/*
 * Convertit un nombre en chaine de caract�re
 */

extern "C" {
	void itoa(char *buf, unsigned long int n, int base)
	{
		unsigned long int tmp;
		int i, j;

		tmp = n;
		i = 0;

		do {
			tmp = n % base;
			buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);//tmp >= 10：如果 tmp 大于或等于 10，意味着它是一个 10 以上的值，在 16 进制或更高的进制中，这些值用字母表示（例如 10 表示为 'a'，11 表示为 'b'，等等）。
		} while (n /= base);
		buf[i--] = 0;

		for (j = 0; j < i; j++, i--) {
			tmp = buf[j];
			buf[j] = buf[i];
			buf[i] = tmp;
		}
	}
}

