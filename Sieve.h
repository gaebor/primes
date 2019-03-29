
#include "Bitset.h"

template<class bitset>
class Sieve
{
private:
    bitset table;
public:
    Sieve() : table((size_t)1){}
    ~Sieve() {}

    void calculate(size_t n, std::function<void(size_t)> printer = [](size_t) {})
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
    const bitset& GetTable()const
    {
        return table;
    }
};
