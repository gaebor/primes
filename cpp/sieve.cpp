#include <iostream>

#include <string>
#include <cinttypes>

#include "Sieve.h"
#include "ArgParser.h"

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
        FILE* outfile = fopen(savefilename.c_str(), "wb");       

        // CalculateSieve(n, table); // try this 
        CalculateSieve(n, table, [](size_t x){});
        if (!outfile || !table.WriteToFile(outfile))
            std::cerr << "error writing \"" << savefilename << "\"" << std::endl;
    }
}

int main(int argc, const char* argv[])
{
    bool batched = true;
    std::string outfilename = "";
    size_t wordsize = CHAR_BIT * sizeof(void*);
    size_t n = 1 << 10;
    auto main_function = calculate_sieve<Bitset<std::uint32_t>>;

    arg::Parser parser("Prime counting application using Sieve of Eratosthenes\n"
        "Author: Gabor Borbely, Contact: borbely@math.bme.hu");

    parser.AddArg(n, { "-n" }, "checks primes in the interval [1, n]");
    parser.AddArg(outfilename, { "-o", "--output" },
        "binary output filename, if empty then print text to stdout", "string");
    parser.AddFlag(batched, { "-b", "--blocked", "--batched", "-m", "--masked" },
        "a bit more clever way of computing the sieve");
    parser.AddFlag(batched, { "-B", "--no-blocked", "--no-batched", "-M", "--no-masked" },
        "resets blocked flag to vanilla computing", false);
    parser.AddArg<size_t>(wordsize, { "-w", "--word", "-s", "--storage" },
        "internal representation size in bits", "", { 8,16,32,64 });

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
    
    main_function(n, outfilename);

	return 0;
}
