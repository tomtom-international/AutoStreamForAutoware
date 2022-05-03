/*
 * Copyright © 2021 TomTom NV. All rights reserved.
 *
 * This software is the proprietary copyright of TomTom NV and its subsidiaries and may be
 * used for internal evaluation purposes or commercial use strictly subject to separate
 * license agreement between you and TomTom NV. If you are the licensee, you are only permitted
 * to use this software in accordance with the terms of your license agreement. If you are
 * not the licensee, you are not authorized to use this software in any manner and should
 * immediately return or destroy it.
 */

#include "Application/Helpers.hpp"

#include "AutoStreamMapConverter/MapConverter.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
  using namespace TomTom::AutoStreamForAutoware;

  // Get configuration file
  if (argc != 2)
  {
    std::cerr << "Mandatory file not provided:" << argv[0] << " <config-file.txt>" << std::endl;
    return 1;
  }

  CConfigurationParameters config;
  if (!getParametersFromConfigurationFile(argv[1], config))
  {
    std::cerr << "Loading parameters failed." << std::endl;
    return 1;
  }

  AutoStreamMapConverter::CAutoStreamMapConverter mapConverter;
  if (!mapConverter.initializeAutoStream(config.mParams))
  {
    std::cerr << "Failed to initialize AutoStream." << std::endl;
  }

  // Create map
  mapConverter.setOutputFileName(config.mOutputFileName);
  if (!mapConverter.storeMap(config.mBoundingBox))
  {
    std::cerr << "Converting map for given bounding box failed." << std::endl;
    return 1;
  }

  return 0;
}
