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
 * FILE Table.h
 * AUTHOR Blackcatn13
 * DATE Feb 1, 2016
 * VERSION 1
 * This is a table template class.
 */
#ifndef BUNDLEAGENT_UTILS_TABLE_H_
#define BUNDLEAGENT_UTILS_TABLE_H_

#include <memory>
#include <string>
#include <map>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include "Node/EndpointListener/Endpoint.h"

class TableException : public std::runtime_error {
 public:
  explicit TableException(const std::string &what)
      : runtime_error(what) {
  }
};

template<class T>
class Table {
 public:
  Table() {
  }

  virtual ~Table() {
  }

  /**
   * @brief Updates the value in the table.
   *
   * This function updates the given value.
   * If the value is not present in the table, this function adds the new value.
   *
   * @param value The value to check.
   */
  void update(std::shared_ptr<T> value) {
    mutex.lock();
    typename std::map<std::string, std::shared_ptr<T>>::iterator it = m_values
        .find(value->getId());
    if (it != m_values.end()) {
      m_values[value->getId()]->update(value);
    } else {
      m_values[value->getId()] = value;
    }
    mutex.unlock();
  }

  /**
   * Function to get a list of all the id's in the Table.
   *
   * @return a vector with the current values id's.
   */
  std::vector<std::string> getValues() {
    std::vector<std::string> keys;
    mutex.lock();
    keys.reserve(m_values.size());
    std::transform(
        m_values.begin(), m_values.end(), std::back_inserter(keys),
        [](const typename std::map<std::string,
            std::shared_ptr<T>>::value_type &pair) {
          return pair.first;
        });
    mutex.unlock();
    return keys;
  }

  /**
   * Function to get the information of the given name.
   *
   * @param name the id of the value.
   * @return a T pointer if exists, else throws a TableException.
   */
  std::shared_ptr<T> getValue(const std::string &name) {
    auto it = m_values.find(name);
    if (it != m_values.end())
      return it->second;
    else
      throw TableException("Value not found.");
  }

 protected:
  /**
   * Map with the neighbours.
   */
  std::map<std::string, std::shared_ptr<T>> m_values;
  /**
   * Mutex for the map.
   */
  std::mutex mutex;
};

#endif  // BUNDLEAGENT_UTILS_TABLE_H_
