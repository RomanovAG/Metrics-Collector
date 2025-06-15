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

    T getResult( bool should_reset = true)
    {
        T res = cnt_;

        if ( should_reset )
        {
            cnt_ = 0;
        }

        return res;
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

    T getResult( bool should_reset = true)
    {
        T res = count_ > 0
                ? sum_ / count_
                : T{0};

        if ( should_reset )
        {
            count_ = 0;
            sum_ = 0;
        }

        return res;
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

    T getResult( bool should_reset = true)
    {
        if ( samples_.empty() )
        {
            return T{0};
        }

        std::vector<T> sorted = samples_;
        std::sort( sorted.begin(), sorted.end());
        
        size_t mid = sorted.size() / 2;

        T res = sorted[mid];
        if ( should_reset )
        {
            samples_.clear();
        }
        return res;
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

    T getResult( bool should_reset = true)
    {
        T res = max_value_;

        if ( should_reset )
        {
            max_value_ = std::numeric_limits<T>::lowest();
        }

        return res;
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

    T getResult( bool should_reset = true)
    {
        T res = min_value_;

        if ( should_reset )
        {
            min_value_ = std::numeric_limits<T>::max();
        }

        return res;
    }
};
} // namespace Metrics::Aggregators