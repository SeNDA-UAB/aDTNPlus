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
  std::string header = "extern \"C\" {int r(int& value) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "return value * 10;";
  int val = 10;
  Worker<int, int> w(header, footer, "r", commandLine, "./");
  w.generateFunction(code);
  w.execute(val);
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
                                                  commandLine, "./");
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
                                                  commandLine, "./");
  ASSERT_THROW(w.generateFunction(code), WorkerException);
}

TEST(WorkerTest, GoodCodeBadLibrary) {
  std::string header = "extern \"C\" {int r(int value) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "return value * 14;";
  int value = 10;
  Worker<int, int> w(header, footer, "g", commandLine, "./");
  w.generateFunction(code);
  ASSERT_THROW(w.execute(value), WorkerException);
}

TEST(WorkerTest, TryingToExecuteValueAfterError) {
  std::string header = "extern \"C\" {int r(int value) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "return \"Hi\";";
  int value = 10;
  Worker<int, int> w(header, footer, "r", commandLine, "./");
  try {
    w.generateFunction(code);
  } catch (const WorkerException &e) {
    ASSERT_THROW(w.execute(value), WorkerException);
  }
}

TEST(WorkerTest, TryingToGetValueAfterError) {
  std::string header = "extern \"C\" {int r(int value) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "return value * 25;";
  int value = 10;
  Worker<int, int> w(header, footer, "g", commandLine, "./");
  w.generateFunction(code);
  try {
    w.execute(value);
  } catch (const WorkerException &e) {
    ASSERT_THROW(w.getResult(), WorkerException);
  }
}

TEST(WorkerTest, MultipleParameters) {
  std::string header =
      "extern \"C\" {int r(int& value, int& value2, int& value3) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared %s -o %s 2>&1";
  std::string code = "return value + value2 + value3;";
  int val1 = 10;
  int val2 = 20;
  int val3 = 30;
  Worker<int, int, int, int> w(header, footer, "r", commandLine, "./");
  w.generateFunction(code);
  w.execute(val1, val2, val3);
  ASSERT_EQ(60, w.getResult());
}

TEST(WorkerTest, MultipleDifferentParameters) {
  std::string header = "#include <string>\n"
      "#include <vector>\n"
      "extern \"C\" {int r(int& value, std::string val2, "
      "std::vector<std::string> value3) {";
  std::string footer = "}}";
  std::string commandLine = "g++ -w -fPIC -shared -std=c++11 %s -o %s 2>&1";
  std::string code = "int res = value;"
      "for (std::string s : value3) {"
      " res += std::stoi(s);"
      "}"
      "res += std::stoi(val2);"
      "return res;";

  Worker<int, int, std::string, std::vector<std::string>> w(header, footer, "r",
                                                            commandLine, "./");
  int value = 10;
  std::string val2 = "20";
  w.generateFunction(code);
  std::vector<std::string> params;
  params.push_back("10");
  params.push_back("20");
  params.push_back("30");
  params.push_back("40");
  w.execute(value, val2, params);
  ASSERT_EQ(130, w.getResult());
}

TEST(WorkerTest, MultipleParameters2) {
  std::string header = "#include <string>\n"
      "#include <vector>\n"
      "#include <algorithm>\n"
      "extern \"C\" {std::vector<std::string> activeForwardingAlgorithm("
      "std::vector<std::string> val1, std::string val2) {";
  std::string footer = "}}";
  std::string functionName = "activeForwardingAlgorithm";
  std::string commandLine = "g++ -w -fPIC -shared -std=c++11 %s -o %s 2>&1";
  std::string code = "auto it = std::find(val1.begin(), val1.end(), val2);"
      "val1.erase(it);"
      "return val1;";
  std::string source = "THE_SOURCE";
  std::vector<std::string> neighbours;
  neighbours.push_back("THE_INIT");
  neighbours.push_back("THE_MIDDLE");
  neighbours.push_back("THE_SOURCE");
  neighbours.push_back("THE_DESTINATION");
  neighbours.push_back("THE_FINAL");
  ASSERT_EQ(neighbours.size(), 5);

  Worker<std::vector<std::string>, std::vector<std::string>,
  std::string> w(header, footer, functionName, commandLine, "./");
  w.generateFunction(code);
  w.execute(neighbours, source);
  std::vector<std::string> result = w.getResult();
  ASSERT_EQ(result.size(), 4);
}

