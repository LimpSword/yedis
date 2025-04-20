#include "buffer.h"
#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>

class BufferTest : public ::testing::Test {
protected:
    Buffer buffer;

    void SetUp() override {
        buffer = Buffer();
    }
};

TEST_F(BufferTest, InitialState) {
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.data().size(), 0);
    EXPECT_EQ(buffer.start, 0);
    EXPECT_EQ(buffer.end, 0);
}

TEST_F(BufferTest, AppendAndVerifyData) {
    const uint8_t data[] = {1, 2, 3, 4, 5};
    buffer.append(data, sizeof(data));

    auto span = buffer.data();
    EXPECT_EQ(span.size(), sizeof(data));
    EXPECT_TRUE(std::equal(span.begin(), span.end(), data));
}

TEST_F(BufferTest, PartialConsume) {
    const uint8_t data[] = {1, 2, 3, 4, 5};
    buffer.append(data, sizeof(data));

    buffer.consume(2);
    EXPECT_EQ(buffer.size(), 3);
    auto span = buffer.data();
    EXPECT_EQ(span.size(), 3);
    EXPECT_EQ(span[0], 3);
    EXPECT_EQ(span[1], 4);
    EXPECT_EQ(span[2], 5);
}

TEST_F(BufferTest, MultipleAppends) {
    const uint8_t data1[] = {1, 2, 3};
    const uint8_t data2[] = {4, 5, 6};

    buffer.append(data1, sizeof(data1));
    buffer.append(data2, sizeof(data2));

    EXPECT_EQ(buffer.size(), 6);
    auto span = buffer.data();
    EXPECT_EQ(span[0], 1);
    EXPECT_EQ(span[5], 6);
}

TEST_F(BufferTest, CompactionTrigger) {
    // Fill buffer past the compaction threshold
    std::vector<uint8_t> large_data(5000, 0);
    std::iota(large_data.begin(), large_data.end(), 0); // Fill with sequential numbers

    buffer.append(large_data.data(), large_data.size());
    buffer.consume(4500); // Consume most of it to trigger compaction on next consume

    buffer.consume(100); // Should trigger compaction

    EXPECT_EQ(buffer.start, 100); // After compaction, start should be set to the previously consumed amount
    EXPECT_LT(buffer.storage.size(), large_data.size()); // Storage should be smaller after compaction

    auto span = buffer.data();
    EXPECT_EQ(span.size(), 400); // Should still have correct amount of data
    // Verify data integrity after compaction
    for (size_t i = 0; i < span.size(); ++i) {
        EXPECT_EQ(span[i], static_cast<uint8_t>(i + 4600));
    }
}

TEST_F(BufferTest, EmptyAfterConsume) {
    const uint8_t data[] = {1, 2, 3};
    buffer.append(data, sizeof(data));
    buffer.consume(buffer.size());

    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.start, 0);
    EXPECT_EQ(buffer.end, 0);
    EXPECT_TRUE(buffer.storage.empty()); // Storage should be cleared
}

TEST_F(BufferTest, DataIntegrityAfterOperations) {
    const uint8_t data1[] = {1, 2, 3, 4, 5};
    const uint8_t data2[] = {6, 7, 8};

    buffer.append(data1, sizeof(data1));
    buffer.consume(2);
    buffer.append(data2, sizeof(data2));

    auto span = buffer.data();
    EXPECT_EQ(span.size(), 6);

    std::vector<uint8_t> expected = {3, 4, 5, 6, 7, 8};
    EXPECT_TRUE(std::equal(span.begin(), span.end(), expected.begin()));
}

TEST_F(BufferTest, ZeroLengthOperations) {
    EXPECT_NO_THROW(buffer.append(nullptr, 0));
    EXPECT_NO_THROW(buffer.consume(0));
    EXPECT_TRUE(buffer.empty());
}

TEST_F(BufferTest, LargeDataHandling) {
    constexpr size_t large_size = 1024 * 1024; // 1MB
    std::vector<uint8_t> large_data(large_size);
    std::iota(large_data.begin(), large_data.end(), 0);

    EXPECT_NO_THROW(buffer.append(large_data.data(), large_data.size()));
    EXPECT_EQ(buffer.size(), large_size);

    auto span = buffer.data();
    EXPECT_TRUE(std::equal(span.begin(), span.end(), large_data.begin()));
}