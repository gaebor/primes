#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <cinttypes>
#include <cmath>
#include <algorithm>

#include "Bitset.h"

template<class Storage>
void sieve(const size_t n)
{
    const size_t sqrtn(std::ceil(std::sqrt(n)));
    std::vector<size_t> found_primes;
    {
        Bitset<Storage> table(sqrtn);
        table.set_every(1);

        size_t pm1 = 0; // the zeroth index is '1', the 'n-1'-th index is 'n'
        table.reset(pm1); // one is not a prime
        while (++pm1 < sqrtn)
        {
            while (!table[pm1])
            {
                ++pm1;
                if (pm1 == sqrtn)
                    goto next1;
            }
            const size_t p = pm1 + 1;
            found_primes.push_back(p);
            printf("%zu\n", p);
            table.reset_every(p, p*p - 1);
        }
    }
next1:
    for (size_t m = sqrtn; m < n; m += sqrtn)
    {
        const auto mm = std::min(n, m + sqrtn);
        Bitset<Storage> table(mm - m);
        table.set_every(1);
        for (auto p : found_primes)
        {
            if (p*p >= mm)
                break;
            size_t start = (m + 1) % p;
            table.reset_every(p, start == 0 ? 0 : p - start);
        }
        for (size_t i = 0; i < table.GetSize(); ++i)
            if (table[i])
                printf("%zu\n", m + i + 1);
    }
}

int main(int argc, const char* argv[])
{
    size_t storage_size = 32;

    size_t n = 1 << 10;
    for (++argv; *argv; ++argv)
    {
        if (strcmp("-h", *argv) == 0 || strcmp("--help", *argv) == 0)
        {
            printf("Prime counting application using Sieve of Eratosthenes\n"
                "contact: borbely@math.bme.hu\n");
            return 0;
        }
        else if (strcmp("-n", *argv) == 0 && *(argv + 1) && atoll(*(argv + 1)) > 0)
        {
            n = atoll(*++argv);
        }
        else if ((strcmp("-s", *argv) == 0 || strcmp("--storage", *argv) == 0 || 
            strcmp("-w", *argv) == 0 || strcmp("--word", *argv) == 0) && *(argv + 1))
        {
            auto w = atoi(*++argv);
            switch (w)
            {
            case  8: storage_size =  8; break;
            case 16: storage_size = 16; break;
            case 32: storage_size = 32; break;
            case 64: storage_size = 64; break;
                break;
            default:
                fprintf(stderr, "Invalid word size: \"%s\"\n", *argv);
                break;
            }
        }
        else
        {
            fprintf(stderr, "Unknown argument: \"%s\"\n", *argv);
        }
    }

    switch (storage_size)
    {
    case 8: sieve<std::uint8_t>(n); break;
    case 16: sieve<std::uint16_t>(n); break;
    case 32: sieve<std::uint32_t>(n); break;
    case 64: sieve<std::uint64_t>(n); break;
    }

	return 0;
}
