/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.x                          */
/*                    Copyright (C)2020, WWIV Software Services           */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/
#include "gtest/gtest.h"

#include "core/log.h"
#include "core/uuid.h"
#include <string>

using std::string;
using namespace wwiv::core;

class UuidTest : public ::testing::Test {
public:
  UuidTest() : g(rd) {}

  std::random_device rd{};
  uuid_generator g;
};

TEST_F(UuidTest, Smoke) {
  const auto u = g.generate();
  LOG(INFO) << u.to_string();
  EXPECT_FALSE(u.is_empty());
  EXPECT_TRUE(u.is_valid());
}

TEST_F(UuidTest, ToString) {
  const auto u = g.generate();
  const auto s = u.to_string();
  LOG(INFO) << s;
  EXPECT_EQ(36u, s.size());
  EXPECT_EQ(u.to_string(), s);
}

TEST_F(UuidTest, Version) {
  const auto u = g.generate();
  LOG(INFO) << u.to_string();
  EXPECT_EQ(4, u.version());
}

TEST_F(UuidTest, Varint) {
  const auto u = g.generate();
  LOG(INFO) << u.to_string();
  EXPECT_EQ(2, u.variant());
}

TEST_F(UuidTest, RoundTrip) {
  const auto u = g.generate();
  const auto orig = u.to_string();
  LOG(INFO) << orig;
  const auto u2 = uuid::from_string(orig);
  ASSERT_TRUE(u2.has_value());
  EXPECT_EQ(u.to_string(), u2.value().to_string());
}