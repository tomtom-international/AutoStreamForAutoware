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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace TomTom {
namespace AutoStreamForAutoware {

const char kCommentSymbol   = '#';
const char kDelimiterSymbol = ':';

/**
 * Function for reading a named parameter as a string from a given file. The file is assumed
 * to follow a yaml-like format where a parameter name is followed by ':' and a value.
 *
 * @param[in] aFilename File from which the parameter must be read.
 * @param[in] aName Name of the parameter.
 * @param[out] aValue Value of the parameter.
 *
 * @retval true If getting parameter succeeded.
 * @retval false If getting parameter failed.
 */
bool getNamedParameter(const std::string& aFilename, const std::string& aName, std::string& aValue)
{
  std::ifstream file(aFilename);
  if (file.is_open())
  {
    std::string line;
    while (getline(file, line))
    {
      // Remove spaces
      line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

      // Skip comments and empty lines
      if (line[0] == kCommentSymbol || line.empty())
      {
        continue;
      }

      // Get parameter name
      auto delimiterPosition = line.find(kDelimiterSymbol);
      auto parameterName     = line.substr(0, delimiterPosition);
      if (aName == parameterName)
      {
        aValue = line.substr(delimiterPosition + 1);
        return true;
      }
    }
  }
  else
  {
    std::cerr << "Couldn't open config file for reading.\n";
  }

  std::cerr << "Parameter " << aName << " not defined!" << std::endl;

  return false;
}

/**
 * Get the content of a file and store it into a single string.
 *
 * @param[in] aFilename Name of the file.
 * @param[out] aContent Content of the file.
 *
 * @retval true If getting parameter succeeded.
 * @retval false If getting parameter failed.
 */
bool getFileContent(const std::string& aFilename, std::string& aContent)
{
  auto          ss = std::ostringstream();
  std::ifstream file(aFilename);
  if (!file.is_open())
  {
    std::cerr << "Could not open file " << aFilename << std::endl;
    return false;
  }

  aContent = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  return true;
}

bool getParametersFromConfigurationFile(const std::string&        aFilePath,
                                        CConfigurationParameters& aConfig)
{
  // Get string parameters from the file
  std::string autoStreamHostNamePort;
  bool        allParams = getNamedParameter(aFilePath, "urlHostNamePort", autoStreamHostNamePort);

  std::string autoStreamUriBasePath;
  allParams = getNamedParameter(aFilePath, "urlUriBasePath", autoStreamUriBasePath) && allParams;

  std::string autoStreamApiKey;
  allParams = getNamedParameter(aFilePath, "apiKey", autoStreamApiKey) && allParams;

  std::string caCertificatesFilePath;
  allParams = getNamedParameter(aFilePath, "certificatesFile", caCertificatesFilePath) && allParams;

  std::string persistentTileCachePath;
  allParams =
    getNamedParameter(aFilePath, "persistentTileCache", persistentTileCachePath) && allParams;

  std::string httpDataUsageLogPath;
  allParams = getNamedParameter(aFilePath, "httpDataUsageLog", httpDataUsageLogPath) && allParams;

  std::string numConnections;
  allParams = getNamedParameter(aFilePath, "numberOfConnections", numConnections) && allParams;

  std::string swLatString, swLonString, neLatString, neLonString;
  allParams = getNamedParameter(aFilePath, "southWestLat", swLatString) && allParams;
  allParams = getNamedParameter(aFilePath, "southWestLon", swLonString) && allParams;
  allParams = getNamedParameter(aFilePath, "northEastLat", neLatString) && allParams;
  allParams = getNamedParameter(aFilePath, "northEastLon", neLonString) && allParams;

  std::string trustedRootCertificateFile;
  allParams = getNamedParameter(aFilePath, "trustedRootCertificateFile", trustedRootCertificateFile)
              && allParams;

  std::string outputFile;
  allParams = getNamedParameter(aFilePath, "outputFile", outputFile) && allParams;

  // Check if all parameters were found
  if (!allParams)
  {
    std::cerr << "Missing parameters in configuration file" << std::endl;
    return false;
  }

  // Store AutoStream parameters
  aConfig.mParams.mApiKey                  = autoStreamApiKey;
  aConfig.mParams.mHostNamePort            = autoStreamHostNamePort;
  aConfig.mParams.mCaCertificatesFilePath  = caCertificatesFilePath;
  aConfig.mParams.mHttpDataUsageLogPath    = httpDataUsageLogPath;
  aConfig.mParams.mPersistentTileCachePath = persistentTileCachePath;
  aConfig.mParams.mUriBasePath             = autoStreamUriBasePath;

  // Set bounding box
  aConfig.mBoundingBox = AutoStream::TBoundingBox(
    AutoStream::TCoordinate::createFromDegrees(std::stod(swLatString), std::stod(swLonString)),
    AutoStream::TCoordinate::createFromDegrees(std::stod(neLatString), std::stod(neLonString)));

  // Name of output file
  aConfig.mOutputFileName = outputFile;

  // Set number of connections
  aConfig.mParams.mNumConnections = std::stoul(numConnections);

  // Set certificate by reading it from file
  std::string certificate;
  if (!getFileContent(trustedRootCertificateFile, certificate))
  {
    std::cerr << "Failed to read certificate file" << std::endl;
    return false;
  }

  aConfig.mParams.mTrustedRootCertificateFile = certificate;

  return true;
}
}
}