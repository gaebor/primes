#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("Naive prime counting application, author: borbely@math.bme.hu\n"
                "USAGE: \"%s\" \"up-to\" > output.txt\n", argv[0]);
        printf("\"up-to\" means that the program calculates the primes up to the given number\n");
        return 0;
    }
    
    size_t n_max = strtol(argv[1], NULL, 10);
    size_t i, n, sqrt_n;
    int is_prime;
    
    putchar('0');

    for (n = 2; n <= n_max; ++n)
    {
        sqrt_n = (size_t)pow((double)n, 0.5);
        for (i = 2, is_prime = 1; i  <= sqrt_n && is_prime; ++i)
            if (n % i == 0)
                is_prime = 0;

        putchar(is_prime ? '1' : '0');
    }
	return 0;
}
