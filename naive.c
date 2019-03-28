#include <stdio.h>
#include <stdlib.h>

volatile int run = 1;

#ifdef _MSC_VER
#include <windows.h>
BOOL WINAPI consoleHandler(DWORD signal) {
    switch (signal)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
        run = 0;
        break;
    default:
        break;
    }

    return TRUE;
}
#else
#include <signal.h>
void new_handler(int signum)
{
    run = 0;
}
#endif

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        printf("Naive prime counting application, author: borbely@math.bme.hu\n"
                "USAGE: \"%s\" > primes.txt\n", argv[0]);
        printf("CTRL+C halts the calculation");
        return 0;
    }
#ifdef _MSC_VER
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        fprintf(stderr, "\nCould not set control handler!\n");
    }
#else
    if (signal(SIGINT, new_handler) == SIG_IGN)
        signal(SIGINT, SIG_IGN);
    if (signal(SIGHUP, new_handler) == SIG_IGN)
        signal(SIGHUP, SIG_IGN);
    if (signal(SIGTERM, new_handler) == SIG_IGN)
        signal(SIGTERM, SIG_IGN);
    if (signal(SIGQUIT, new_handler) == SIG_IGN)
        signal(SIGQUIT, SIG_IGN);
#endif
    size_t prime_pi = 1;
    size_t allocated = 1024;
    size_t* primes = (size_t*)malloc(allocated * sizeof(size_t));
    primes[0] = 2;

    size_t p, i;
    for (i = 0; i < prime_pi; ++i)
        printf("%zu\n", primes[i]);
    
    p = primes[i-1];

    while (run > 0 && p > 0) // stops at unsigned overflow!
    {
        ++p;
        for (i = 0; i < prime_pi; ++i)
        {
            if (primes[i] * primes[i] > p) // no need to search further
                break;
            else if (p % primes[i] == 0)
                goto next_num;
        }
        printf("%zu\n", p);
        if (prime_pi >= allocated)
        {
            allocated *= 2;
            primes = (size_t*)realloc(primes, allocated * sizeof(size_t));
            if (primes == NULL)
                break;
        }
        primes[prime_pi++] = p;
    next_num:
        continue;
    }
    free(primes);
	return 0;
}
