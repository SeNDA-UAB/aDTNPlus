/*
 * Copyright (c) 2018 SeNDA
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
 * FILE RemoveBundleFromDiskException.h
 * AUTHOR MCarmen
 * DATE Jan 16, 2018
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_EXCEPTIONS_REMOVEBUNDLEFROMDISKEXCEPTION_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_EXCEPTIONS_REMOVEBUNDLEFROMDISKEXCEPTION_H_

#include <exception>
#include <string>

class RemoveBundleFromDiskException : public std::exception {
 public:
  RemoveBundleFromDiskException(std::string bundleId);
  virtual ~RemoveBundleFromDiskException();
  virtual const char* what() const throw ();

 private:
  std::string m_bundleId;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_EXCEPTIONS_REMOVEBUNDLEFROMDISKEXCEPTION_H_
