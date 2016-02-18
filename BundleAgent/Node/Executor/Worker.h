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
 * FILE Worker.h
 * AUTHOR Blackcatn13
 * DATE Feb 18, 2016
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_EXECUTOR_WORKER_H_
#define BUNDLEAGENT_NODE_EXECUTOR_WORKER_H_

#include <dlfcn.h>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <future>
#include <thread>
#include <stdexcept>
#include <fstream>
#include <functional>
#include <memory>

class SigFaultException : public std::runtime_error {
 public:
  explicit SigFaultException(const std::string &what)
      : runtime_error(what) {
  }
};

class WorkerException : public std::runtime_error {
 public:
  explicit WorkerException(const std::string &what)
      : runtime_error(what) {
  }
};

void signalHandler(int signum) {
  throw SigFaultException("Code generated a segmentation fault.");
}

/**
 * This function loads from a handler (dlopen) the symbol with the given name.
 * It returns a std::function of type T, where T has the form
 * <returnType(param1, param2, ...)>
 *
 * If the symbol cannot be load it will throw a WorkerException.
 *
 * @param handler Handler with a shared library opened.
 * @param name The name of the symbol to load.
 * @return A function.
 */
template<class T>
std::function<T> loadFunction(void *handler, std::string name) {
  dlerror();
  void *result = dlsym(handler, name.c_str());
  char* const error = dlerror();
  if (error != NULL) {
    std::stringstream errorMessage;
    errorMessage << "Cannot find symbol in shared library, reason: " << error;
    throw WorkerException(errorMessage.str());
  }
  return reinterpret_cast<T*>(result);
}

/**
 * Snprintf implementation using std::strings instead of char*
 * @param format The string to format.
 * @param args The arguments to add.
 * @return The string with the args formated.
 */
template<typename ... Args>
std::string stringFormat(const std::string& format, Args ... args) {
  size_t size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
  std::unique_ptr<char[]> buffer(new char[size]);
  std::snprintf(buffer.get(), size, format.c_str(), args ...);
  return std::string(buffer.get(), buffer.get() + size - 1);
}

/**
 * CLASS Worker
 * This class is a code executor. It generates a code file, compiles into a
 * shared library, loads that library and calls the function.
 *
 * The two parameters are:
 * T: return type of the function.
 * T1: type of the argument to pass to the function.
 */
template<class T, class T1>
class Worker {
 public:
  /**
   * @brief Constructor.
   *
   * It generates a worker given the following parameters:
   * @param header Header to attach to the code (it must contain an extern "C").
   * @param footer Footer to attach to the code.
   * @param functionName The name of the function that will be loaded.
   * @param commandLine The command line parameters to generate the shared library
   *                    (it will contain to %s, the first for the code file, the
   *                    second for the library output name.)
   */
  Worker(std::string header, std::string footer, std::string functionName,
         std::string commandLine)
      : m_header(header),
        m_footer(footer),
        m_functionName(functionName),
        m_commandLine(commandLine),
        m_handler(0) {
  }
  /**
   * Destructor of the class.
   */
  virtual ~Worker() {
    dlclose(m_handler);
  }
  /**
   * Generates the shared library using the given code.
   *
   * It will attach the header and the footer to the code.
   * @param code Code to compile.
   */
  void generateFunction(std::string code) {
    std::stringstream fullCode;
    fullCode << m_header << code << m_footer;
    std::ofstream codeFile("code.cpp");
    codeFile << fullCode.str();
    codeFile.close();
    std::string command = stringFormat(m_commandLine, "code.cpp", "code.so");
    std::signal(SIGSEGV, signalHandler);
    int val = system(command.c_str());
    if (val != 0) {
      throw WorkerException("Error while compiling the code.");
    } else {
      m_handler = dlopen("./code.so", RTLD_LAZY | RTLD_LOCAL);
      if (!m_handler) {
        std::stringstream errorMessage;
        errorMessage << "Cannot open the shared library, reason: " << dlerror();
        throw WorkerException(errorMessage.str());
      }
    }
  }
  /**
   * This function gets the function and runs it, with the given parameters.
   * @param params The parameters to pass to the function.
   */
  void execute(T1 params) {
    std::signal(SIGSEGV, signalHandler);
    try {
      std::function<T(T1)> function = loadFunction<T(T1)>(
          m_handler, m_functionName.c_str());
      std::packaged_task<T(T1)> task(function);
      m_future = task.get_future();
      std::thread t(std::move(task), params);
      t.detach();
    } catch (const SigFaultException &e) {
      throw WorkerException("Worker could not execute the code correctly.");
    }
  }
  /**
   * This function gets the result of the called function in execute(T1).
   * @return The result that the function returned.
   */
  T getResult() {
    try {
      auto result = m_future.get();
      std::signal(SIGSEGV, SIG_IGN);
      return result;
    } catch (...) {
      throw WorkerException("Worker could not retrieve function result.");
    }
  }

 private:
  /**
   * The future variable where the return value will be saved.
   */
  std::future<T> m_future;
  /**
   * The header to attach to the code.
   */
  std::string m_header;
  /**
   * The footer to attach to the code.
   */
  std::string m_footer;
  /**
   * The function name to call.
   */
  std::string m_functionName;
  /**
   * The command line to generate the compilation files.
   */
  std::string m_commandLine;
  /**
   * The shared library handler.
   */
  void *m_handler;
}
;

#endif  // BUNDLEAGENT_NODE_EXECUTOR_WORKER_H_
