#include <random>

#include "Aggregators.hpp"
#include "MC.hpp"

using namespace Metrics;
using namespace Aggregators;

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
    auto cpu_metric = collector.registerMetric<Average, double>("CPU");
    auto http_metric = collector.registerMetric<Counter, long long>("HTTP requests RPS");
    auto temp_metric = collector.registerMetric<Median, float>("Temperature");
    auto loop_iterations = collector.registerMetric<Counter, int>("Loop iters");

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
        temp_metric->addSample( 36.6f);
        std::this_thread::sleep_for( std::chrono::seconds( 1));
        temp_metric->addSample( 0.5f);
    });

    for ( int i = 0; i < work_duration; ++i )
    {
        std::this_thread::sleep_for( std::chrono::seconds(1));
        collector.writeToFile( log_file);
        std::cout << "Written at " << i + 1 << " second(s)\n";
    }

    http_worker.join();
    loop_thread_2.join();
    loop_thread_3.join();
    loop_thread_6.join();
    loop_thread_4.join();
    loop_thread_8.join();
    loop_thread_5.join();
    loop_thread_7.join();
    loop_thread_1.join();
    cpu_monitor.join();
    temp_sensor.join();

    return 0;
}