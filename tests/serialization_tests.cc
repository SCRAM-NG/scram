/*
 * Copyright (C) 2017-2018 Olzhas Rakhimov
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

#include "serialization.h"

#include <boost/filesystem.hpp>

#include <catch2/catch.hpp>

#include "env.h"
#include "initializer.h"
#include "settings.h"
#include "xml.h"

namespace fs = boost::filesystem;

namespace scram::mef::test {

TEST_CASE("SerializationTest.InputOutput", "[mef::serialization]") {
  xml::Validator validator(env::install_dir() + "/share/scram/gui.rng");

  auto input = GENERATE(values<std::vector<std::string>>(
      {{"tests/input/xml_special_chars.xml"},
       {"tests/input/fta/correct_tree_input.xml"},
       {"tests/input/fta/correct_tree_input_with_probs.xml"},
       {"tests/input/fta/missing_bool_constant.xml"},
       {"tests/input/fta/null_gate_with_label.xml"},
       {"tests/input/fta/flavored_types.xml"},
       {"input/TwoTrain/two_train.xml"},
       {"tests/input/fta/correct_formulas.xml"},
       {"input/Theatre/theatre.xml"},
       {"input/Baobab/baobab2.xml", "input/Baobab/baobab2-basic-events.xml"}}));

  std::unique_ptr<Model> model;
  INFO("inputs: " +
       Catch::StringMaker<std::vector<std::string>>::convert(input))
  REQUIRE_NOTHROW(model = mef::Initializer(input, core::Settings{}).model());
  fs::path unique_name = "scram_test-" + fs::unique_path().string();
  fs::path temp_file = fs::temp_directory_path() / unique_name;
  INFO("temp file: " + temp_file.string());
  REQUIRE_NOTHROW(Serialize(*model, temp_file.string()));
  REQUIRE_NOTHROW(xml::Document(temp_file.string(), &validator));
  fs::remove(temp_file);
}

}  // namespace scram::mef::test
