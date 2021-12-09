#ifndef COMPARATOR_H
#define COMPARATOR_H
enum compare_t {
    EQUAL = 0, //равно
    GREAT = 1, //больше
    LESS = -1  //меньше
};

template <typename TKey>
class comparator
{
public:
    compare_t operator () (const TKey &key_1, const TKey &key_2) const;
};

template <typename TKey>
compare_t comparator<TKey>::operator () (const TKey &key_1, const TKey &key_2) const
{
    compare_t result = EQUAL;
    if (key_1 == key_2)
    {
        result = EQUAL;
    }
    else if (key_1 > key_2)
    {
        result = GREAT;
    }
    else if (key_1 < key_2)
    {
        result = LESS;
    }
    return result;
}
#endif // COMPARATOR_H
