#include <iostream>
#include <limits>
#include <algorithm>
#include <vector>

namespace Metrics::Aggregators
{
template <typename T>
class Counter
{
private:
    T cnt_{0};

public:
    void addSample( T value)
    {
        cnt_ += value;
    }

    T getResult() const
    {
        return cnt_;
    }

    void reset()
    {
        cnt_ = T{0};
    }
};

template <typename T>
class Average
{
private:
    T sum_{0};
    size_t count_{0};

public:
    void addSample( T value)
    {
        sum_ += value;
        count_++;
    }

    T getResult() const
    {
        T res = count_ > 0
                ? sum_ / count_
                : T{0};

        return res;
    }

    void reset()
    {
        count_ = T{0};
        sum_ = T{0};
    }
};

template <typename T>
class Median
{
private:
    std::vector<T> samples_;

public:
    void addSample( T value)
    {
        samples_.push_back( value);
    }

    T getResult() const
    {
        if ( samples_.empty() )
        {
            return T{0};
        }

        std::vector<T> sorted = samples_;
        std::sort( sorted.begin(), sorted.end());
        
        size_t mid = sorted.size() / 2;

        return sorted[mid];
    }

    void reset()
    {
        samples_.clear();
    }
};

template <typename T>
class Max
{
private:
    T max_value_{std::numeric_limits<T>::lowest()};
public:
    void addSample( T value)
    {
        if ( value > max_value_ ) max_value_ = value;
    }

    T getResult() const
    {
        return max_value_;
    }

    void reset()
    {
        max_value_ = std::numeric_limits<T>::lowest();
    }
};

template <typename T>
class Min
{
private:
    T min_value_{std::numeric_limits<T>::max()};
public:
    void addSample( T value)
    {
        if ( value < min_value_ ) min_value_ = value;
    }

    T getResult() const
    {
        return min_value_;
    }

    void reset()
    {
        min_value_ = std::numeric_limits<T>::max();
    }
};
} // namespace Metrics::Aggregators