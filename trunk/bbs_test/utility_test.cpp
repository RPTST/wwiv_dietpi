/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.0x                         */
/*             Copyright (C)2014, WWIV Software Services                  */
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
// Needed by time_t in WWIV. Must be specified before gtest.h since that will
// define time_t for us.
#define _USE_32BIT_TIME_T

#include <ctime>
#include "gtest/gtest.h"
#include "bbs_helper.h"

char* W_DateString(time_t tDateTime, const char* pszOrigMode , const char* delim);

static const time_t MAY_18_2014 = 1400461200;

using std::cout;
using std::endl;
using std::string;

class UtilityTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        helper.SetUp();
    }

    BbsHelper helper;
};

TEST_F(UtilityTest, DateString) {
    char *res = W_DateString(MAY_18_2014, "Y", "");
    ASSERT_STREQ("2014", res);

    char *res2 = W_DateString(MAY_18_2014, "WDT", "");
    ASSERT_STREQ("Sunday, May 18, 2014 06:00 PM", res2);
}
