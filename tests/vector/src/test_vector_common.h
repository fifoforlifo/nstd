#pragma once

struct ConOnly
{
    int& value;

    ConOnly(int& value_)
        : value(value_)
    {}
    ConOnly(const ConOnly& rhs)
        : value(rhs.value)
    {}
};

class Swappable
{
    Swappable(const Swappable& rhs);
public:
    int value;

    Swappable(int value_)
        : value(value_)
    {}

    Swappable& swap(Swappable& rhs)
    {
        int tmp = value;
        value = rhs.value;
        rhs.value = tmp;
        return *this;
    }
};
