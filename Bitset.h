#include<climits>
#include<limits>
#include<type_traits>
#include<vector>

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
private:
    static const size_t word_size;
public:
    typedef storage WordType;
    Bitset(const size_t n)
    :   _size(n),
        n_words((n + word_size - 1)/word_size),
        _array(new storage[n_words])
    {
        static_assert(std::is_unsigned<storage>::value, "Unsigned integral required!");
    }
    ~Bitset()
    {
        delete[] _array;
    }
    const storage* GetContent()const
    {
        return _array;
    }
    size_t GetnumberOfWords()const
    {
        return n_words;
    }
    size_t GetSize()const
    {
        return _size;
    }
    void set(const size_t i)
    {
        set_one<SetTraits<storage, true>>(i);
    }
    void reset(const size_t i)
    {
        set_one<SetTraits<storage, false>>(i);
    }
    void set_every(const size_t step, const size_t start = 0)
    {
        every<SetTraits<storage, true>>(step, start);
    }
    void reset_every(const size_t step, const size_t start = 0)
    {
        every<SetTraits<storage, false>>(step, start);
    }
    bool operator[](size_t i)const
    {
        return (_array[i / word_size] & (((storage)1) << (i % word_size))) != 0;
    }
private:
    template<typename traits>
    void set_one(size_t i)
    {
        traits::one_bit(_array[i / word_size], i % word_size);
    }
    template<typename traits>
    void every(const size_t step, size_t start = 0)
    {
        if (step <= word_size)
        {   // trick
            storage* place = _array + start / word_size;
            storage* const end_place = _array + n_words;
            if (start < _size && (start % word_size) >= step)
            {   // handle the first word
                for (size_t k = start % word_size; k < word_size; k += step, start += step)
                    traits::one_bit(*place, k);
                place++;
            }
            std::vector<storage> masks(step / gcd(word_size, step), traits::default_mask);
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
            for (size_t i = start; i < _size; i += step)
                traits::one_bit(_array[i / word_size], i % word_size);
        }
    }
private:
    const size_t _size;
    const size_t n_words;
    storage* _array;
};

template <class storage>
const size_t Bitset<storage>::word_size = sizeof(storage) * CHAR_BIT;
