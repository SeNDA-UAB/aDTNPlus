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
 * FILE PluginAPI.h
 * AUTHOR Blackcatn13
 * DATE Feb 22, 2016
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_PLUGINAPI_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_PLUGINAPI_H_

class BundleProcessor;

#define PLUGIN_API_VERSION 1

typedef BundleProcessor *(*GetPluginFunc)();

struct PluginInfo {
  int apiVersion;                 // Version of the API used in this plugin.
  const char *fileName;           // Name of the file.
  const char *className;          // Name of the class in the plugin.
  const char *pluginName;         // Name of the plugin.
  const char *pluginVersion;      // Version of the plugin.
  const char *pluginDescription;  // Small description of the plugin.
  GetPluginFunc getPlugin;        // Function that initialises the plugin.
};

#define NEW_PLUGIN(classType, pluginName, pluginVersion, pluginDescription)   \
    extern "C" {                                                              \
      BundleProcessor* initProcessor()                                        \
      {                                                                       \
        static classType m_singleton;                                         \
        return &m_singleton;                                                  \
      }                                                                       \
      PluginInfo information =                                                \
      {                                                                       \
        PLUGIN_API_VERSION,                                                   \
        __FILE__,                                                             \
        #classType,                                                           \
        pluginName,                                                           \
        pluginVersion,                                                        \
        pluginDescription,                                                    \
        initProcessor,                                                        \
      };                                                                      \
    }                                                                         \

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_PLUGINAPI_H_
