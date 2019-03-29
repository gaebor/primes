# Runtime tests
Tests are run on one core of an `Intel Xeon E5520 @ 2.27GHz`.
The `naive` implementation took **47 sec** to calculate *5M* primes.
So this is the slowest one.

There are 54,400,028 primes between 0 and *`2^30` = 1,073,741,824*.
The rest of the tests are run on this interval.

## [sieve](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) 
Memory usage is about 130MByte.

| word size (bit) | vanilla bitset | blocked bitset |
| --- | --- | --- |
| 8 | 21.98sec | 19.14sec |
| 16| 22.23sec | 18.39sec |
|32 | 21.76sec | 19.05sec |
|64 | 22.99sec | 17.36sec |

## [segmented](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes#Segmented_sieve) sieve
Memory usage is about 3.3MByte.

| word size (bit) | vanilla bitset | blocked bitset |
| --- | --- | --- |
| 8 | 19.50sec | 15.62sec |
| 16| 18.09sec | 14.91sec |
|32 | 18.41sec | 14.65sec |
|64 | 18.96sec | 16.08sec |
