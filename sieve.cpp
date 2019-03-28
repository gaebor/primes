#include <cstdio>
#include <cstdlib>
#include <cstring>

#include<iostream>

#include <string>
#include <cinttypes>

#include "Bitset.h"

template<class Storage>
void sieve(size_t n, const std::string& savefilename)
{
    Bitset<Storage> table(n);
    table.set_every(1);

    size_t pm1 = 0; // the zeroth index is '1', the 'n-1'-th index is 'n'
    table.reset(pm1); // one is not a prime
    while (++pm1 < n)
    {
        while (!table[pm1])
        {
            ++pm1;
            if (pm1 == n)
                goto finished;
        }
        const size_t p = pm1 + 1;
        printf("%zu\n", p);
        table.reset_every(p, p*p - 1);
    }
finished:
    if (!savefilename.empty())
    {
        const auto file = fopen(savefilename.c_str(), "wb");
        if (file)
        {
            auto ptr = table.GetContent();
            const auto end = ptr + table.GetnumberOfWords();
            for (; ptr != end; ++ptr)
                if (1 != fwrite(ptr, sizeof(Storage), 1, file))
                    break;
            fclose(file);
        }
        else
            std::cerr << "error opening: " << savefilename << std::endl;
    }
}

int main(int argc, const char* argv[])
{
    std::string outfilename = "";
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
        else if ((strcmp("-o", *argv) == 0 || strcmp("--output", *argv) == 0) && *(argv + 1))
        {
            outfilename = *++argv;
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
    case 8: sieve<std::uint8_t>(n, outfilename); break;
    case 16: sieve<std::uint16_t>(n, outfilename); break;
    case 32: sieve<std::uint32_t>(n, outfilename); break;
    case 64: sieve<std::uint64_t>(n, outfilename); break;
    }

	return 0;
}
