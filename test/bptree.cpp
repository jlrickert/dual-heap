#include "gtest/gtest.h"

TEST (ExamplePassingTest, Equality) {
  int x = 5;
  int y = 6;
  EXPECT_EQ (18.0, 18);
}

TEST (ExampleFailingTest, Equality) {
  EXPECT_EQ (18.0, 17);
}

TEST(ArithmeticTest, Positive) {
  int x = 5;
  int y = 6;
  int expected = 11;
  EXPECT_EQ(x + y, expected);
}

TEST(ArithmeticTest, Negative) {
  int x = 5;
  int y = 6;
  int expected = -11;
  EXPECT_EQ(x + y, expected);
}
