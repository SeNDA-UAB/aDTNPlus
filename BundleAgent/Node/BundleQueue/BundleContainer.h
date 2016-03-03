/*
 * Copyright (c) 2015 SeNDA
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
 * FILE BundleContainer.h
 * AUTHOR Blackcatn13
 * DATE Dec 15, 2015
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEQUEUE_BUNDLECONTAINER_H_
#define BUNDLEAGENT_NODE_BUNDLEQUEUE_BUNDLECONTAINER_H_

#include <memory>
#include <string>

class Bundle;

class BundleContainerCreationException : public std::runtime_error {
 public:
  explicit BundleContainerCreationException(const std::string &what)
      : runtime_error(what) {
  }
};

/**
 * CLASS BundleContainer
 * This class holds all the information that is saved for every bundle.
 * It's also responsible for loading and saving the bundles to disk.
 */
class BundleContainer {
 public:
  /**
   * Default constructor.
   *
   * This will create a BundleContainer, that holds a bundle and the id of
   * the neighbour that send this bundle.
   *
   * @param from the neighbour id.
   * @param bundle the bundle to hold.
   */
  BundleContainer(std::string from, std::unique_ptr<Bundle> bundle);
  /**
   * Generates a BundleContainer from serialized data.
   *
   * @param data The serialized bundleCotainer.
   */
  explicit BundleContainer(const std::string &data);
  /**
   * Destructor of the class.
   */
  virtual ~BundleContainer();
  /**
   * Creates an empty BundleContainer.
   */
  BundleContainer();

  BundleContainer(BundleContainer&& bc);
  /**
   * Get the held bundle.
   *
   * @return The held bundle.
   */
  Bundle& getBundle();
  /**
   * Get the source neighbour name.
   *
   * @return the neighbour source.
   */
  std::string getFrom() const;
  /**
   * Convert the BundleContainer into a string to save it to disk.
   *
   * @return The serialized form of the BundleContainer.
   */
  virtual std::string serialize();
  /**
   * Generates a BunldeContainer from a serialized one.
   *
   * @param data a serialized bundleContainer.
   * @return the BundleContainer.
   */
  virtual void deserialize(const std::string &data);
  /**
   * @brief Returns an string with a nice view of the Bundle container information.
   *
   * @return The string with the bundle container information.
   */
  virtual std::string toString();

 protected:
  /**
   * The bundle that the container holds.
   */
  std::unique_ptr<Bundle> m_bundle;
  /**
   * The source neighbour id.
   */
  std::string m_from;
  /**
   * Header to check serialization integrity and version.
   * 0x1vff, where v is the version.
   */
  static const uint16_t m_header = 0x11ff;
  /**
   * Footer to check serialization integrity and version.
   * 0xff1v, where v is the version.
   */
  static const uint16_t m_footer = 0xff11;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEQUEUE_BUNDLECONTAINER_H_
