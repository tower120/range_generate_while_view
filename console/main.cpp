#include <iostream>
#include <vector>
#include <string>

#include <experimental/ranges/view/generate_while.hpp>

#include <chrono>

template<class TimeUnit = std::chrono::milliseconds, class Closure>
static auto measure(Closure&& closure){
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    closure();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    return duration_cast<TimeUnit>(t2-t1).count();
}

template<class TimeUnit = std::chrono::milliseconds, class Closure>
static auto benchmark(unsigned int times, Closure&& closure){
    return measure<TimeUnit>([&]() {
        for (unsigned int i = 0; i < times; ++i) closure();
    });
}

namespace ranges::view{
    using experimental::ranges::view::generate_while;
}

void benchmark(){
    int size  = 100'000;
    int times = 10'000;

    using namespace ranges;

    {
        int sum = 0;
        auto t = benchmark(times, [&](){
            auto list = view::generate([i = 0, size]() mutable -> std::optional<int>{
                            //if (i > size) return {};
                            return {++i};
                        })
                    //| view::take_exactly(size);
                        | view::take_while([size](auto i){ return *i<=size;});

            //auto list = view::generate_n([i = 0]() mutable -> int{ return ++i;}, size);
            for(auto i : list){
                sum += *i;
            }
        });

        std::cout << sum << " " << t << std::endl;
    }

    {
        int sum = 0;
        auto t = benchmark(times, [&](){
            auto list = view::generate_while(
                [i = int(), size](auto rewind) mutable -> optional<int>{
                    if constexpr (rewind) i = 0;
                    ++i;
                    if (i == size) return {};
                    return i;
                });

            for(int i : list){
                sum += i;
            }
        });

        std::cout << sum << " " << t << std::endl;
    }

    {
        int sum = 0;
        auto t = benchmark(times, [&](){
            int i = 0;
            do{
                sum += i;
                ++i;
            } while(i != size);
        });

        std::cout << sum << " " << t << std::endl;
    }
}

int main()
{
    using namespace ranges;

    benchmark();

    return 0;

    auto list = view::generate_while(
        [i = int()](auto rewind) mutable -> optional<int>{
            if constexpr (rewind) i = 0;
            if (i == 10) return {};
            return ++i;
        });

    /*std::cout << *(++list.begin());
    std::cout << std::endl;*/

    for(auto i : list){
        std::cout << i << " ";
    }

    return 0;
}
