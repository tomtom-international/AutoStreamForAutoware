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

#ifndef TOMTOM_APPLICATION_HELPERS_H
#define TOMTOM_APPLICATION_HELPERS_H

#include "AutoStreamMapConverter/AutoStreamInterface.hpp"

#include "TomTom/AutoStream/MapBaseTypes.h"

#include <string>

namespace TomTom {
namespace AutoStreamForAutoware {

/**
 * Structure that is used to store configuration parameters.
 */
struct CConfigurationParameters
{
  AutoStreamMapConverter::CAutoStreamParameters mParams;
  AutoStream::TBoundingBox                      mBoundingBox;
  std::string                                   mOutputFileName;
};

/**
 * Get parameters from a given configuration file.
 *
 * @param[in] aFilePath Configuration file path.
 * @param[out] aConfig Parameter structure in which parameters will be stored.
 * @retval True If opening configuration file succeeded.
 * @retval False If opening configuration file failed.
 */
bool getParametersFromConfigurationFile(const std::string&        aFilePath,
                                        CConfigurationParameters& aConfig);
}
}
#endif