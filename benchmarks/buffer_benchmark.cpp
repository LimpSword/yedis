#include <benchmark/benchmark.h>
#include "buffer.h"
#include <random>
#include <algorithm>

// Optimized helper function
std::vector<uint8_t> generate_random_data(size_t size, std::mt19937& gen, std::uniform_int_distribution<>& dis) {
    std::vector<uint8_t> data(size);
    std::generate(data.begin(), data.end(), [&]() { return static_cast<uint8_t>(dis(gen)); });
    return data;
}

// Benchmark small appends
static void BM_SmallAppends(benchmark::State& state) {
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<> dis(0, 255);
    auto data = generate_random_data(8, gen, dis);

    for (auto _ : state) {
        Buffer buf;
        for (int i = 0; i < state.range(0); ++i) {
            buf.append(data.data(), data.size());
        }
    }
}
BENCHMARK(BM_SmallAppends)->Range(8, 8<<10);

// Benchmark large appends
static void BM_LargeAppends(benchmark::State& state) {
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, 255);
    for (auto _ : state) {
        state.PauseTiming();
        Buffer buf;
        auto data = generate_random_data(state.range(0), gen, dis);
        state.ResumeTiming();
        buf.append(data.data(), data.size());
    }
}
BENCHMARK(BM_LargeAppends)->Range(1<<10, 1<<20);

// Benchmark append-consume cycles
static void BM_AppendConsumeCycle(benchmark::State& state) {
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, 255);
    auto data = generate_random_data(1024, gen, dis);

    for (auto _ : state) {
        Buffer buf;
        for (int i = 0; i < state.range(0); ++i) {
            buf.append(data.data(), data.size());
            buf.consume(512);
        }
    }
}
BENCHMARK(BM_AppendConsumeCycle)->Range(8, 8<<10);

// Benchmark compaction behavior
static void BM_CompactionTrigger(benchmark::State& state) {
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, 255);
    auto data = generate_random_data(64, gen, dis);

    for (auto _ : state) {
        Buffer buf;
        for (int i = 0; i < state.range(0); ++i) {
            buf.append(data.data(), data.size());
            buf.consume(32);
        }
    }
}
BENCHMARK(BM_CompactionTrigger)->Range(64, 8<<10);

// Benchmark mixed operations
static void BM_MixedOperations(benchmark::State& state) {
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, 255);
    auto small_data = generate_random_data(16, gen, dis);
    auto large_data = generate_random_data(4096, gen, dis);

    for (auto _ : state) {
        Buffer buf;
        for (int i = 0; i < state.range(0); ++i) {
            if (i % 4 == 0) {
                buf.append(large_data.data(), large_data.size());
            } else {
                buf.append(small_data.data(), small_data.size());
            }

            if (i % 3 == 0) {
                buf.consume(buf.size() / 2);
            }

            if (!buf.empty()) {
                benchmark::DoNotOptimize(buf.data());
            }
        }
    }
}
BENCHMARK(BM_MixedOperations)->Range(8, 8<<10);

// Benchmark data access
static void BM_DataAccess(benchmark::State& state) {
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, 255);
    for (auto _ : state) {
        state.PauseTiming();
        Buffer buf;
        auto data = generate_random_data(state.range(0), gen, dis);
        buf.append(data.data(), data.size());
        state.ResumeTiming();

        for (int i = 0; i < 100; ++i) {
            auto span = buf.data();
            benchmark::DoNotOptimize(span.data());
            benchmark::DoNotOptimize(span.size());
        }
    }
}
BENCHMARK(BM_DataAccess)->Range(8, 8<<10);

BENCHMARK_MAIN();
