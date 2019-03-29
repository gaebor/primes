#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>

#include "Sieve.h"

template<size_t r, class Ty>
constexpr Ty Round(Ty x)
{
    return ((x + r - 1) / r) * r;
}

template<class bitset>
void calculate_sieve(size_t n, size_t delta = 0, const std::string& savefilename = "")
{
    delta = std::max(size_t(std::ceil(std::sqrt(n))), delta);
    delta = std::min(delta, n);
    delta = Round<bitset::word_size>(delta);

    std::vector<size_t> found_primes;
    size_t m = 1;

    std::function<void(const bitset& t)> printer;
    if (savefilename.empty())
    {   // output to stdout
        printer = [&m](const bitset& t)
        {
            for (size_t i = 0; i < t.size(); ++i)
                if (t[i])
                    printf("%zu\n", m + i);
        };
    }
    else
    {   // output to a binary file
        auto file = fopen(savefilename.c_str(), "wb");
        if (file)
        {
            printer = [&savefilename](const bitset& t)
            {
                if (!t.WriteToFile(savefilename.c_str(), true))
                    std::cerr << "error writing \"" << savefilename << "\"" << std::endl;
            };
            fclose(file);
        }
    }

    bitset table(delta);
    CalculateSieve(delta, table, [&found_primes](size_t x) {found_primes.push_back(x); });
    printer(table);

    for (m = delta + 1; m < n; m += delta)
    {
        CalculateSegment(m, std::min(m + delta - 1, n), found_primes, table);
        printer(table);
    }
}

int main(int argc, const char* argv[])
{
    bool batched = false;
    size_t delta = 0;
    std::string outfilename = "";
    auto main_function = calculate_sieve<Bitset<std::uint32_t>>;
    size_t wordsize = 32;
    size_t n = 1 << 10;

    for (++argv; *argv; ++argv)
    {
        if (strcmp("-h", *argv) == 0 || strcmp("--help", *argv) == 0)
        {
            printf("Prime counting application using segmented sieve of Jonathan Sorenson\n"
                "Contact author here: borbely@math.bme.hu\n");
            return 0;
        }
        else if (strcmp("-n", *argv) == 0 && *(argv + 1) && atoll(*(argv + 1)) > 0)
        {
            n = atoll(*++argv);
        }
        else if ((strcmp("-d", *argv) == 0 || strcmp("--delta", *argv) == 0) && *(argv + 1))
        {
            delta = atoll(*++argv);
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

    main_function(n, delta, outfilename);

    return 0;
}
