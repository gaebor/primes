#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>

#include <string>
#include <cinttypes>

#include "Sieve.h"

template<class bitset>
void calculate_sieve(size_t n, const std::string& savefilename = "")
{
    bitset table(n);
    if (savefilename.empty())
    {  // write to stdout
        CalculateSieve(n, table, [](size_t x) {printf("%zu\n", x); });
    }
    else
    {
        // CalculateSieve(n, table); // try this 
        CalculateSieve(n, table, [](size_t x){});
        if (!table.WriteToFile(savefilename.c_str()))
            std::cerr << "error writing \"" << savefilename << "\"" << std::endl;
    }
}

int main(int argc, const char* argv[])
{
    bool batched = false;
    std::string outfilename = "";
    auto main_function = calculate_sieve<Bitset<std::uint32_t>>;
    size_t wordsize = 32;
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
        else if (strcmp("-b", *argv) == 0 || strcmp("--blocked", *argv) == 0 ||
            strcmp("-m", *argv) == 0 || strcmp("--masked", *argv) == 0 ||
            strcmp("--batched", *argv) == 0)
        {
            batched = true;
        }
        else if ((strcmp("-s", *argv) == 0 || strcmp("--storage", *argv) == 0 ||
            strcmp("-w", *argv) == 0 || strcmp("--word", *argv) == 0) && *(argv + 1))
        {
            auto w = atoi(*++argv);
            switch (w)
            {
            case 8:
            case 16:
            case 32:
            case 64:
                wordsize = w;
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

    if (batched)
    {
        switch (wordsize)
        {
        case 8:  main_function = calculate_sieve<BitsetBlocked<std::uint8_t>>; break;
        case 16: main_function = calculate_sieve<BitsetBlocked<std::uint16_t>>; break;
        case 32: main_function = calculate_sieve<BitsetBlocked<std::uint32_t>>; break;
        case 64: main_function = calculate_sieve<BitsetBlocked<std::uint64_t>>; break;
        };
    }
    else
    {
        switch (wordsize)
        {
        case 8:  main_function = calculate_sieve<Bitset<std::uint8_t>>; break;
        case 16: main_function = calculate_sieve<Bitset<std::uint16_t>>; break;
        case 32: main_function = calculate_sieve<Bitset<std::uint32_t>>; break;
        case 64: main_function = calculate_sieve<Bitset<std::uint64_t>>; break;
        };
    }
    
    main_function(n, outfilename);

	return 0;
}
