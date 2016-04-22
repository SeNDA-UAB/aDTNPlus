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
 * FILE json.h
 * AUTHOR Blackcatn13
 * DATE Mar 10, 2016
 * VERSION 1
 * This file contains the class json.
 */
#ifndef BUNDLEAGENT_UTILS_JSON_H_
#define BUNDLEAGENT_UTILS_JSON_H_

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "ExternTools/json/json.hpp"

template<class ContainerT>
inline void tokenize(const std::string& str, ContainerT& tokens,
                     const std::string& delimiters = " ",
                     bool trimEmpty = false) {
  std::string::size_type pos, lastPos = 0;

  using value_type = typename ContainerT::value_type;
  using size_type = typename ContainerT::size_type;

  while (true) {
    pos = str.find_first_of(delimiters, lastPos);
    if (pos == std::string::npos) {
      pos = str.length();
      if (pos != lastPos || !trimEmpty)
        tokens.push_back(
            value_type(str.data() + lastPos, (size_type) pos - lastPos));
      break;
    } else {
      if (pos != lastPos || !trimEmpty)
        tokens.push_back(
            value_type(str.data() + lastPos, (size_type) pos - lastPos));
    }
    lastPos = pos + 1;
  }
}

/**
 * CLASS Json
 * This class works as middleware between a json library and our processor.
 * This middleware adapts the user queries to return updated information every
 * time.
 */
class Json : public nlohmann::json {
 public:
  /**
   * Empty constructor.
   *
   * Creates a json object.
   */
  Json();
  /**
   * Destructor of the class.
   */
  virtual ~Json();
  /**
   * Returns the element asked by the key. The key will go in the format "first.second"
   * This will be the same as calling ["first"]["second"].
   *
   * This function acts as middleware between the library and the custom information.
   * By default this function returns a read/write reference to the value.
   *
   * Each derived class may define different paths and different behaviours of
   * each path.
   *
   * @param key The key to get the element.
   * @return A reference to the key element.
   */
  virtual reference operator()(const std::string &key);
  /**
   * This function returns a read only reference given a vector of tokens, from the
   * underlying json.
   *
   * @param tokens The tokens used as json names of an object.
   * @return The value of the object.
   */
  reference getReadOnly(const std::vector<std::string> tokens);
  /**
   * This function returns a read/write reference given a vector of tokens, from the
   * underlying json.
   *
   * This function is recursive, the first reference must be the base one to start
   * checking objects.
   *
   * @param tokens The tokens used as json names of an object.
   * @param ref The upmost json reference to check the tokens.
   * @return The value of the object.
   */
  reference getReadAndWrite(std::vector<std::string> tokens, reference ref);
  /**
   * This function returns the reference to this class contained json.
   *
   * @return The reference to the json.
   */
  reference getBaseReference();

 protected:
  /**
   * Auxiliary function to check if two vectors are equals.
   *
   * The call to equals, for two vectors is large, this way the code keeps more
   * simple and easy to read.
   *
   * @param tokensA The first vector of tokens.
   * @param tokensB The second vector of tokens.
   * @return True if the vectors are equals, false otherwise.
   */
  bool tokensEquals(const std::vector<std::string>& tokensA,
                    const std::vector<std::string>& tokensB);
  /**
   * A json used to the read only and for new values.
   */
  nlohmann::json m_newJson;
  /**
   * The reference to the underlying json class.
   */
  nlohmann::json& m_baseReference;
};

#endif  // BUNDLEAGENT_UTILS_JSON_H_
