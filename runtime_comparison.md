# Runtime tests
Tests are run on one core of an `Intel Xeon E5520 @ 2.27GHz`.
The `naive` implementation took **47 sec** to calculate 5,000,000 primes.
So this is the slowest one.

There are 54,400,028 primes between *1* and *`2^30` = 1,073,741,824*.
The rest of the tests are run on this interval.

## [sieve](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) 
Memory usage is about 130MiByte.

| word size (bit) | vanilla bitset | blocked bitset |
| --- | --- | --- |
| 8 | 21.98sec | 19.14sec |
| 16| 22.23sec | 18.39sec |
|32 | 21.76sec | 19.05sec |
|64 | 22.99sec | 17.36sec |

## [segmented](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes#Segmented_sieve) sieve
Memory usage is about 3-5MiByte.

| word size (bit) | vanilla bitset | blocked bitset |
| --- | --- | --- |
| 8 | 19.50sec | 15.62sec |
| 16| 18.09sec | 14.91sec |
|32 | 18.41sec | 14.65sec |
|64 | 18.96sec | 16.08sec |

The main advantage of the segmented sieve is that it uses *O(sqrt(n))* memory.
In case of *n=1,073,741,824* this means that it even fits the cache.

The best performance is achieved if the delta segment size is just below the L2 cache size.
In my CPU the L2 is 256KiByte, meaning that *delta* (`-d`) is `2^20` uses the cache efficiently.
With this caching (and blocked bitset):

| word size (bit) | blocked bitset |
| --- | --- |
| 8 | 14.57sec |
| 16| 13.69sec |
|32 | 13.21sec |
|64 | 12.85sec |

As delta increases then the segmented algorithm becomes the standard sieve.
