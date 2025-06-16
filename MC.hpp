#ifndef _MC_HPP_
#define _MC_HPP_

#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

namespace Metrics
{
class MetricBase
{
private:
    std::string name_;

public:
    MetricBase( const std::string &name)
        : name_( name) {}
    virtual ~MetricBase() = default;
    virtual std::string getValueAsString() = 0;
    std::string getName() const { return name_; }
};

template <typename Agg, typename T>
concept AggConcept = requires( Agg agg, T value)
{
    // Must have void addSample(T value);
    { agg.addSample(value) } -> std::same_as<void>;
    
    // Must have T getResult();
    { agg.getResult() } -> std::same_as<T>;

    // Must have void reset();
    { agg.reset() } -> std::same_as<void>;
};

template <template <typename> class Aggregator, typename T>
requires AggConcept<Aggregator<T>, T>
class Metric : public MetricBase
{
    static_assert( std::is_arithmetic_v<T>, "Type must be arithmetic" );
private:
    std::string convertToString( T value) const
    {
        if constexpr ( std::is_arithmetic_v<T> )
        {
            return std::to_string( value);
        } else
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
    }

    Aggregator<T> aggregator_;
    //mutable
    std::mutex mutex_;

public:
    Metric( const std::string &name)
        : MetricBase( name) {}

    void addSample( T value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        aggregator_.addSample( value);
    }

    std::string getValueAsString() override
    {
        T val;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            val = aggregator_.getResult();
            aggregator_.reset();
        }
        return this->convertToString( val);
    }
};

class MetricsCollector
{
private:
    std::string getCurrentTimestamp() const
    {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t( now);
        std::tm now_tm = *std::localtime( &now_c);
        
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::put_time( &now_tm, "%Y-%m-%d %H:%M:%S")
            << '.' << std::setfill( '0') << std::setw( 3) << milliseconds.count();
        return oss.str();
    }

    std::vector<std::shared_ptr<MetricBase>> metrics_;
    mutable std::mutex mutex_;

public:
    template <template <typename> class Aggregator, typename T>
    std::shared_ptr<Metric<Aggregator, T>>
    registerMetric( const std::string &name)
    {
        auto metric = std::make_shared<Metric<Aggregator, T>>( name);
        std::lock_guard<std::mutex> lock( mutex_);
        metrics_.push_back( metric);
        return metric;
    }

    void writeToFile( const std::string &filename) const
    {
        std::ostringstream line;
        line << this->getCurrentTimestamp();
        {
            std::lock_guard<std::mutex> lock( mutex_);
        
            for ( const auto &metric : metrics_ )
            {
                line << " \"" << metric->getName() << "\" " << metric->getValueAsString();
            }
        }

        std::ofstream file( filename, std::ios::app);
        if ( !file.is_open() )
        {
            throw std::runtime_error( "Cannot open file: " + filename);
        }
        file << line.str() << std::endl;
    }
};
} // namespace Metrics

#endif // _MC_HPP_