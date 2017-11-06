#include "gtest/gtest.h"

TEST (ExamplePassingTest, Equality) {
  EXPECT_EQ (18.0, 18);
}

TEST (ExampleFailingTest, Equality) {
  EXPECT_EQ (18.0, 17);
}
