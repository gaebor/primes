#pragma once

#include <functional>

#include "Bitset.h"

template<class bitset>
void CalculateSieve(size_t n, bitset& table,
    std::function<void(size_t)> printer = [](size_t x) {})
{
    table.resize(n);
    table.set_every(1);
    
    size_t pm1 = 0; // the zeroth index is '1', the 'n-1'-th index is 'n'
    table.reset(pm1); // one is not a prime
    while (++pm1 < n)
    {
        while (!table[pm1])
        {
            ++pm1;
            if (pm1 == n)
                return;
        }
        const size_t p = pm1 + 1;
        printer(p);
        table.reset_every(p, p*p - 1);
    }
}

//! calculates a list of primes in the interval [m, mm]
template<class bitset>
void CalculateSegment(const size_t m, const size_t mm, const std::vector<size_t>& found_primes,
    bitset& table)
{
    table.resize(mm - m + 1);
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
}
