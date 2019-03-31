#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>
#include <thread>
#include <list>

#include "Sieve.h"
#include "ArgParser.h"

template<size_t r, class Ty>
constexpr Ty Round(Ty x)
{
    return ((x + r - 1) / r) * r;
}

template<class bitset>
void calculate_sieve(size_t n,
    size_t delta = 0, size_t delta2 = 0, size_t thread = 1,
    const std::string& savefilename = "")
{
    delta = std::max(size_t(std::ceil(std::sqrt(n))), delta);
    delta = std::min(delta, n);
    delta = Round<bitset::word_size>(delta);

    if (delta2 == 0)
        delta2 = delta;

    std::vector<size_t> found_primes;

    FILE* output = nullptr;
    std::function<void(const bitset& t, size_t)> printer;

    if (savefilename.empty())
    {   // output to stdout
        printer = [](const bitset& t, size_t offset)
        {
            for (size_t i = 0; i < t.size(); ++i)
                if (t[i])
                    printf("%zu\n", offset + i);
        };
    }
    else
    {   // output to a binary file
        output = fopen(savefilename.c_str(), "wb");
        if (output)
        {
            printer = [&output](const bitset& t, size_t)
            {
                t.WriteToFile(output);
            };
        }
        else
        {
            std::cerr << "error writing \"" << savefilename << "\"" << std::endl;
        }
    }

    std::list<bitset> tables;
    tables.emplace_back(delta);
    found_primes.reserve(size_t(delta / std::log(delta)));

    CalculateSieve(delta, tables.front() , [&found_primes](size_t x) {found_primes.push_back(x); });
    printer(tables.front(), 1);

    for (size_t i = 1; i < thread; ++i)
        tables.emplace_back(delta2);
    
    std::vector<std::thread> threads(thread);

    //for (m = delta + 1; m < n; m += delta2)
    //{
    //    CalculateSegment(m, std::min(m + delta2 - 1, n), found_primes, tables[0]);
    //    printer(tables[0]);
    //}

    size_t m = delta + 1;
    while (m < n)
    {
        auto table_it = tables.begin();
        for (size_t t = 0; t < thread; ++t, ++table_it)
        {
            if (m < n)
                threads[t] = std::thread([&found_primes](size_t a, size_t b, bitset* table)
            {
                CalculateSegment<bitset>(a, b, found_primes, *table);
            }, m + t * delta2, std::min(m + (t + 1) * delta2 - 1, n), &*table_it);
            else
                break;
        }
        table_it = tables.begin();
        for (size_t t = 0; t < thread; ++t, m += delta2)
        {
            if (threads[t].joinable())
            {
                threads[t].join();
                printer(*table_it++, m);
            }
        }
    }
    if (output)
        fclose(output);
}

int main(int argc, const char* argv[])
{
    bool batched = true;
    std::string outfilename = "";
    size_t wordsize = CHAR_BIT * sizeof(void*);
    size_t n = 1 << 10;
    auto main_function = calculate_sieve<Bitset<std::uint32_t>>;
    size_t delta = 0;
    size_t delta2 = 0;
    size_t thread = 1;

    arg::Parser parser("Prime counting application using segmented sieve of Jonathan Sorenson\n"
        "Author: Gabor Borbely, Contact: borbely@math.bme.hu");

    parser.AddArg(n, { "-n" }, "checks primes in the interval [1, n]");
    parser.AddArg(delta, { "-d", "--delta" }, 
        "length of one segment in the segmented sieve"
        "\n\t\tif not set, then sqrt(n) is used."
        "\n\t\tIt is advised to set it to 4 times the L2 cache size in bytes."
        "\n\t\tFor example: 1MiByte -> delta=2^22");
    parser.AddArg(outfilename, { "-o", "--output" },
        "binary output filename, if empty then print text to stdout", "string");
    parser.AddFlag(batched, { "-b", "--blocked", "--batched", "-m", "--masked" }, 
        "a bit more clever way of computing the sieve");
    parser.AddFlag(batched, { "-B", "--no-blocked", "--no-batched", "-M", "--no-masked" },
        "resets blocked flag to vanilla computing", false);
    parser.AddArg<size_t>(wordsize, { "-w", "--word", "-s", "--storage" },
        "internal representation size in bits", "", {8,16,32,64});
    parser.AddArg(delta2, { "-d2", "--delta2" });
    parser.AddArg(thread, { "-t", "--thread", "--threads" });
    
    parser.Do(argc, argv);

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

    main_function(n, delta, delta2, thread, outfilename);

    return 0;
}
