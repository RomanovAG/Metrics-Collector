#include <random>

#include "Aggregators.hpp"
#include "MC.hpp"

using namespace Metrics;
using namespace Aggregators;

// example of custom aggregator
template<typename T>
class CustomAggLast
{
private:
    T last_ = T{0};
public:
    void addSample( T val)
    {
        last_ = val;
    }

    T getResult() const
    {
        return last_;
    }

    void reset()
    {
        last_ = T{0};
    }
};


int main( int argc, char **argv)
{
    // default
    std::string log_file = "metrics.log";
    int work_duration = 10/*iterations*/;

    if (argc > 1)
    {
        log_file = argv[1];
    }
    if (argc > 2)
    {
        try
        {
            work_duration = std::stoi(argv[2]);
        } catch (const std::exception& e)
        {
            std::cerr << "Invalid interval argument: " << argv[2] 
                      << ". Using default value of " << work_duration << " second(s).\n";
        }
    }

    MetricsCollector collector;
    
    // Register some metrics
    auto cpu_metric = collector.registerMetric<Average, double>( "CPU");
    auto http_metric = collector.registerMetric<Counter, long long>( "HTTP requests RPS");
    auto temp_metric = collector.registerMetric<Median, float>( "Temperature");
    auto loop_iterations = collector.registerMetric<Counter, int>( "Loop iters");

    auto last_time = collector.registerMetric<CustomAggLast, std::string>( "Last timestamp");

    // Imitate some multithread system

    std::thread http_worker( [&http_metric]()
    {
        for ( int i = 0; i < 10000; ++i )
        {
            http_metric->addSample( random() % 2);
            //http_metric->addSample( i);
            std::this_thread::sleep_for( std::chrono::microseconds( 10));
        }
    });

    std::thread loop_thread_1( [&loop_iterations]()
    {
        for ( int i = 0; i < 250'000'000; ++i )
        {
            loop_iterations->addSample( 1);
        }
    });
    std::thread loop_thread_2( [&loop_iterations]()
    {
        for ( int i = 0; i < 125'000'000; ++i )
        {
            loop_iterations->addSample( 1);
        }
    });
    std::thread loop_thread_3( [&loop_iterations]()
    {
        for ( int i = 0; i < 62'500'000; ++i )
        {
            loop_iterations->addSample( 1);
        }
    });
    std::thread loop_thread_4( [&loop_iterations]()
    {
        for ( int i = 0; i < 31'250'000; ++i )
        {
            loop_iterations->addSample( 1);
        }
    });
    std::thread loop_thread_5( [&loop_iterations]()
    {
        for ( int i = 0; i < 15'625'000; ++i )
        {
            loop_iterations->addSample( 1);
        }
    });
    std::thread loop_thread_6( [&loop_iterations]()
    {
        for ( int i = 0; i < 7'812'500; ++i )
        {
            loop_iterations->addSample( 1);
        }
    });
    std::thread loop_thread_7( [&loop_iterations]()
    {
        for ( int i = 0; i < 3'906'250; ++i )
        {
            loop_iterations->addSample( 1);
        }
    });
    std::thread loop_thread_8( [&loop_iterations]()
    {
        for ( int i = 0; i < 1'953'125*2; ++i )
        {
            loop_iterations->addSample( 1);
        }
    });

    std::thread cpu_monitor( [&cpu_metric]()
    {
        for ( int i = 0; i < 525; ++i )
        {
            cpu_metric->addSample( random() % 8);
            std::this_thread::sleep_for( std::chrono::milliseconds( 10));
        }
    });

    std::thread temp_sensor( [&temp_metric]()
    {
        for ( int i = 0; i < 1525; ++i )
        {
            temp_metric->addSample( random() % 4 + 25.0f);
            std::this_thread::sleep_for( std::chrono::milliseconds( 10));
        }
    });

    std::thread timestamp_work( [&last_time]()
    {
        for ( int i = 0; i < 19; ++i )
        {
            auto now = std::chrono::system_clock::now();
            auto now_c = std::chrono::system_clock::to_time_t( now);
            std::tm now_tm = *std::localtime( &now_c);
        
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;

            std::ostringstream oss;
            oss << std::put_time( &now_tm, "%H:%M:%S")
                << '.' << std::setfill( '0') << std::setw( 3) << milliseconds.count();
            last_time->addSample( oss.str());
            std::this_thread::sleep_for( std::chrono::milliseconds( 500));
        }
    });

    for ( int i = 0; i < work_duration; ++i )
    {
        // write to file every second
        std::this_thread::sleep_for( std::chrono::seconds(1));
        collector.writeToFile( log_file);
        std::cout << "Written at " << i + 1 << " seconds\n";
    }

    http_worker.join();
    loop_thread_1.join();
    loop_thread_2.join();
    loop_thread_3.join();
    loop_thread_4.join();
    loop_thread_5.join();
    loop_thread_6.join();
    loop_thread_7.join();
    loop_thread_8.join();
    cpu_monitor.join();
    temp_sensor.join();
    timestamp_work.join();

    return 0;
}