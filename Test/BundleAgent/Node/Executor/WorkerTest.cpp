/*
 * Copyright (c) 2016 SeNDA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
/**
 * FILE WorkerTest.cpp
 * AUTHOR Blackcatn13
 * DATE Feb 18, 2016
 * VERSION 1
 *
 */

#include <string>
#include <vector>
#include "Node/Executor/Worker.h"
#include "gtest/gtest.h"

TEST(WorkerTest, SimpleCode) {
  std::string header = "extern \"C\" {int r(int value) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "return value * 10;";

  Worker<int, int> w(header, footer, "r", commandLine);
  w.generateFunction(code);
  w.execute(10);
  ASSERT_EQ(100, w.getResult());
}

TEST(WorkerTest, ComplexCode) {
  std::string header = "#include <string>\n"
      "#include <vector>\n"
      "#include <sstream>\n"
      "extern \"C\" {std::string r(std::vector<std::string> values) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "std::stringstream ss;"
      "for (int i = 0; i < values.size(); ++i) {"
      "  ss << values[i].at(0);}"
      "return ss.str();";

  Worker<std::string, std::vector<std::string>> w(header, footer, "r",
                                                  commandLine);
  w.generateFunction(code);
  std::vector<std::string> params;
  params.push_back("This");
  params.push_back("Is");
  params.push_back("A");
  params.push_back("Test");
  w.execute(params);
  ASSERT_EQ("TIAT", w.getResult());
}

TEST(WorkerTest, BadCode) {
  std::string header = "#include <string>\n"
      "extern \"C\" {std::string r(std::vector<std::string> values) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -Wall -fPIC -shared %s -o %s 2>&1";
  std::string code = "std::stringstream ss;"
      "for (int i = 0; i < values.size(); ++i) {"
      "  ss << values[i].at(0);}"
      "return ss.str();";

  Worker<std::string, std::vector<std::string>> w(header, footer, "r",
                                                  commandLine);
  ASSERT_THROW(w.generateFunction(code), WorkerException);
}

TEST(WorkerTest, GoodCodeBadLibrary) {
  std::string header = "extern \"C\" {int r(int value) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "return value * 14;";

  Worker<int, int> w(header, footer, "g", commandLine);
  w.generateFunction(code);
  ASSERT_THROW(w.execute(10), WorkerException);
}

TEST(WorkerTest, TryingToExecuteValueAfterError) {
  std::string header = "extern \"C\" {int r(int value) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "return \"Hi\";";

  Worker<int, int> w(header, footer, "r", commandLine);
  try {
    w.generateFunction(code);
  } catch (const WorkerException &e) {
    ASSERT_THROW(w.execute(10), WorkerException);
  }
}

TEST(WorkerTest, TryingToGetValueAfterError) {
  std::string header = "extern \"C\" {int r(int value) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "return value * 25;";

  Worker<int, int> w(header, footer, "g", commandLine);
  w.generateFunction(code);
  try {
    w.execute(10);
  } catch (const WorkerException &e) {
    ASSERT_THROW(w.getResult(), WorkerException);
  }
}

