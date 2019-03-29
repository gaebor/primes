#include <climits>
#include <limits>
#include <type_traits>
#include <vector>
#include <functional>
#include <cstdio>

#include "little_endian.h"

template<class Ty, bool do_set>
struct SetTraits
{
};

template<class Ty>
struct SetTraits<Ty, true>
{
    static Ty const default_mask;
    static Ty& one_bit(Ty& a, size_t b)
    {
        return a |= (((Ty)1) << b);
    }
    static Ty& mask(Ty& a, Ty b)
    {
        return a |= b;
    }
};

template<class Ty>
struct SetTraits<Ty, false>
{
    static Ty const default_mask;
    static Ty& one_bit(Ty& a, size_t b)
    {
        return a &= ~(((Ty)1) << b);
    }
    static Ty& mask(Ty& a, Ty b)
    {
        return a &= b;
    }
};

template<class Ty> Ty const SetTraits<Ty, true>::default_mask = 0;

template<class Ty> Ty const SetTraits<Ty, false>::default_mask = std::numeric_limits<Ty>::max();

template<class Type>
constexpr Type gcd(Type a, Type b)
{
    return a < b ? gcd(b, a) : (b == 0 ? a : gcd(b, a%b));
}

template<class Type>
constexpr Type lcm(Type a, Type b)
{
    return (a*b) / gcd<Type>(a, b);
}

template <class storage = size_t>
class Bitset
{
    static_assert(std::is_unsigned<storage>::value, "Unsigned integral required!");
public:
    static const size_t word_size;
    typedef storage WordType;
    explicit Bitset(size_t n) : _array(nullptr)
    {
        resize(n);
    }
    ~Bitset()
    {
        if (_array)
            delete[] _array;
    }
    size_t size()const
    {
        return _size;
    }
    void resize(size_t n)
    {
        _size = n;
        n_words = (n + word_size - 1) / word_size;
        if (_array)
            delete[] _array;
        _array = new WordType[n_words];
    }
    bool WriteToFile(const char* filename, bool append = false)const
    {
        const auto file = fopen(filename, append ? "ab" : "wb");
        if (file)
        {
            WordType x;
            auto ptr = _array;
            const auto end = ptr + n_words;
            for (; ptr != end; ++ptr)
            {
                x = ConvertToLittleEndian(*ptr);
                if (1 != fwrite(&x, sizeof(WordType), 1, file))
                {
                    fclose(file);
                    return false;
                }
            }
            fclose(file);
            return true;
        }
        else
            return false;
    }
    void set(const size_t i)
    {
        set_one<SetTraits<WordType, true>>(i);
    }
    void reset(const size_t i)
    {
        set_one<SetTraits<WordType, false>>(i);
    }
    void set_every(const size_t step, const size_t start = 0)
    {
        every<SetTraits<WordType, true>>(step, start);
    }
    void reset_every(const size_t step, const size_t start = 0)
    {
        every<SetTraits<WordType, false>>(step, start);
    }
    bool operator[](size_t i)const
    {
        return (_array[i / word_size] & (((WordType)1) << (i % word_size))) != 0;
    }
private:
    template<typename traits>
    void set_one(size_t i)
    {
        traits::one_bit(_array[i / word_size], i % word_size);
    }
protected:
    template<typename traits>
    void every(const size_t step, size_t start = 0)
    {
        for (size_t i = start; i < _size; i += step)
            traits::one_bit(_array[i / word_size], i % word_size);
    }
    size_t _size;
    size_t n_words;
    WordType* _array;
};

template <class storage>
const size_t Bitset<storage>::word_size = sizeof(storage) * CHAR_BIT;

template<class Ty>
class BitsetBlocked : public Bitset<Ty>
{
    using Bitset<Ty>::word_size;
    using typename Bitset<Ty>::WordType;
public:
    BitsetBlocked(){}
    explicit BitsetBlocked(size_t n) : Bitset<Ty>(n) {}
    void set_every(const size_t step, const size_t start = 0)
    {
        every<SetTraits<WordType, true>>(step, start);
    }
    void reset_every(const size_t step, const size_t start = 0)
    {
        every<SetTraits<WordType, false>>(step, start);
    }
protected:
    template<typename traits>
    void every(const size_t step, size_t start = 0)
    {
        if (step <= word_size)
        {   // trick
            WordType* place = this->_array + start / word_size;
            WordType* const end_place = this->_array + this->n_words;
            if (start < this->_size && (start % word_size) >= step)
            {   // handle the first word
                for (size_t k = start % word_size; k < word_size; k += step, start += step)
                    traits::one_bit(*place, k);
                place++;
            }
            std::vector<WordType> masks(step / gcd(word_size, step), traits::default_mask);
            for (size_t i = start % word_size; i < lcm(word_size, step); i += step)
                traits::one_bit(masks[i / word_size], i % word_size);

            for (size_t mask_index = 0; place < end_place; ++place)
            {
                traits::mask(*place, masks[mask_index]);
                ++mask_index;
                if (mask_index == masks.size())
                    mask_index = 0;
            }
        }
        else
        {   // vanilla
            for (size_t i = start; i < this->_size; i += step)
                traits::one_bit(this->_array[i / word_size], i % word_size);
        }
    }
};
