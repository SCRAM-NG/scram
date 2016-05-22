/*
 * Copyright (C) 2014-2016 Olzhas Rakhimov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include "risk_analysis_tests.h"

namespace scram {
namespace core {
namespace test {

// Benchmark Tests for auto-generated 200 event fault tree.
TEST_P(RiskAnalysisTest, 200Event) {
  std::string tree_input = "./share/scram/input/Autogenerated/200_event.xml";
  settings.probability_analysis(true).limit_order(15);
  ASSERT_NO_THROW(ProcessInputFile(tree_input));
  ASSERT_NO_THROW(ran->Analyze());
  if (settings.approximation() == "rare-event") {
    EXPECT_NEAR(0.794828, p_total(), 1e-5);
  } else {
    EXPECT_NEAR(0.55985, p_total(), 1e-5);
  }
  EXPECT_EQ(287, products().size());
}

}  // namespace test
}  // namespace core
}  // namespace scram
