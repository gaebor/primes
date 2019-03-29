#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>

#include "Bitset.h"

//! calculates a list of primes in the interval p \in [m, mm]
template<class bitset>
bool segment(const size_t m, const size_t mm, const std::vector<size_t>& found_primes,
    std::function<void(const bitset&)> printer = [](const bitset&) {})
{
    if (found_primes.empty() || found_primes.back()*found_primes.back() < mm)
        return false;
    if (found_primes.front() != (size_t)2)
        return false;

    bitset table(mm - m + 1);
    table.set_every(1);

    for (auto p : found_primes)
    {
        if (p*p > mm)
            break;
        size_t start = m % p;
        if (start != 0)
            start = p - start;
        table.reset_every(p, start);
    }
    printer(table);
    return true;
}

template<class Storage>
void sieve(const size_t n, size_t delta = 0, const std::string& savefilename = "")
{
    delta = std::max(size_t(std::ceil(std::sqrt(n))), delta);
    //TODO what if delta is not a multiple of word_size!
    std::vector<size_t> found_primes;
    {
        Bitset<Storage> table(delta);
        table.set_every(1);

        size_t pm1 = 0; // the zeroth index is '1'
        table.reset(pm1); // one is not a prime
        while (++pm1 < delta)
        {
            while (!table[pm1])
            {
                ++pm1;
                if (pm1 == delta)
                    goto next1;
            }
            const size_t p = pm1 + 1;
            found_primes.push_back(p);
            printf("%zu\n", p);
            table.reset_every(p, p*p - 1);
        }
    next1:
        if (!savefilename.empty())
        {
            if (!table.WriteToFile(savefilename.c_str(), false))
                std::cerr << "error writing \"" << savefilename << "\"" << std::endl;
        }
    }

    for (size_t m = delta; m < n; m += delta)
    {
        const auto mm = std::min(n, m + delta);
        Bitset<Storage> table(mm - m);
        table.set_every(1);
        for (auto p : found_primes)
        {
            if (p*p >= mm)
                break;
            size_t start = (m + 1) % p;
            table.reset_every(p, start == 0 ? 0 : p - start);
        }
        if (!savefilename.empty())
        {
            if (!table.WriteToFile(savefilename.c_str(), true))
                std::cerr << "error writing \"" << savefilename << "\"" << std::endl;
        }
        for (size_t i = 0; i < table.size(); ++i)
            if (table[i])
                printf("%zu\n", m + i + 1);
    }
}

int main(int argc, const char* argv[])
{
    auto sieve_function = sieve<std::uint32_t>;
    std::string outfilename = "";

    size_t n = 1 << 10;
    size_t delta = 0;
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
        else if ((strcmp("-d", *argv) == 0 || strcmp("--delta", *argv) == 0) && *(argv + 1) && atoll(*(argv + 1)) > 0)
        {
            delta = atoll(*++argv);
        }
        else if ((strcmp("-s", *argv) == 0 || strcmp("--storage", *argv) == 0 || 
            strcmp("-w", *argv) == 0 || strcmp("--word", *argv) == 0) && *(argv + 1))
        {
            auto w = atoi(*++argv);
            switch (w)
            {
            case 8: sieve_function = sieve<std::uint8_t>; break;
            case 16: sieve_function = sieve<std::uint16_t>; break;
            case 32: sieve_function = sieve<std::uint32_t>; break;
            case 64: sieve_function = sieve<std::uint64_t>; break;
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

    sieve_function(n, delta, outfilename);

	return 0;
}
