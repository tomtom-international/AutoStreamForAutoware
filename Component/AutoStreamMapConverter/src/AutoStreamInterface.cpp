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

#include "AutoStreamMapConverter/AutoStreamInterface.hpp"

#include "TomTom/AutoStream/CallParameters.h"

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

constexpr AutoStream::TLayer kMapLayer = AutoStream::CLayerBitset::kHdRoad
                                         | AutoStream::CLayerBitset::kHdSpeedRestrictions
                                         | AutoStream::CLayerBitset::kHdTrafficSigns;

/**
 * Function for turning a given std string into an AutoStream immutable string reference.
 *
 * @param[in] aString String that must be converted.
 * @return AutoStream::CImmutableStringReference String as an immutable string reference.
 */
AutoStream::CImmutableStringReference getImmutableStringRef(const std::string& aString)
{
  return { aString.c_str(), aString.length() };
}

/**
 * Print the certificate status in a human understandable way to the screen.
 *
 * @param[in] aStatus Status that must be printed.
 */
void printCertificateStatus(const AutoStream::CAutoStream::TCertificatesStatus& aStatus)
{
  switch (aStatus)
  {
    case AutoStream::CAutoStream::kCertificatesStatusSuccess:
      std::cout << "updateCertificates : success" << std::endl;
      break;
    case AutoStream::CAutoStream::kCertificatesStatusStorageFailed:
      std::cout << "updateCertificates : storage failed" << std::endl;
      break;
    case AutoStream::CAutoStream::kCertificatesStatusRootVerificationFailed:
      std::cout << "updateCertificates : root verification failed" << std::endl;
      break;
    case AutoStream::CAutoStream::kCertificatesStatusValidityPeriodExpired:
      std::cout << "updateCertificates : expired" << std::endl;
      break;
    case AutoStream::CAutoStream::kCertificatesStatusOther:
      std::cout << "updateCertificates : other error" << std::endl;
    default:
      assert(static_cast<bool>("Invalid"));
      break;
  }
}

/**
 * Validate a given map version.
 *
 * @param[in] aVersion Version that must be validated.
 * @retval True If map version is valid.
 * @retval False If map version is invalid.
 */
bool validateMapVersion(const AutoStream::CMapVersionAndHash& aVersion)
{
  if (!aVersion.mapVersion.isValid())
  {
    std::cerr << "Map version is invalid" << std::endl;
    return false;
  }

  std::cout << "Map version is valid." << std::endl;
  return true;
}

CAutoStreamInterface::CAutoStreamInterface()
  : mHdMap(nullptr)
  , mHdMapAccess(nullptr)
  , mHttpsClient(nullptr)
  , mHttpDataLogger(nullptr)
{
}

CAutoStreamInterface::~CAutoStreamInterface()
{
  if (mHdMap && mHdMap->isValid() && mHdMapAccess && mHdMapAccess->isValid())
  {
    mHdMap->destroyHdMapAccess(mHdMapAccess);
  }
  mAutoStream.stop();
}

bool CAutoStreamInterface::initializeAutoStream(const CAutoStreamParameters& aAutoStreamParams)
{
  if (!initializeTileCache(
        AutoStream::CImmutableStringReference(aAutoStreamParams.mPersistentTileCachePath.data(),
                                              aAutoStreamParams.mPersistentTileCachePath.length())))
  {
    return false;
  }

  if (!initializeHttpDataUsageLogger(
        AutoStream::CImmutableStringReference(aAutoStreamParams.mHttpDataUsageLogPath.data(),
                                              aAutoStreamParams.mHttpDataUsageLogPath.length())))
  {
    return false;
  }

  initializeHttpsClient(aAutoStreamParams);

  return startAutoStream(aAutoStreamParams);
}

AutoStream::HdMap::CHdMapAccess* CAutoStreamInterface::getHdMapAccess() const
{
  return mHdMapAccess;
}

bool CAutoStreamInterface::startAutoStream(const CAutoStreamParameters& aAutoStreamParams)
{
  AutoStream::CAutoStreamSettings settings;
  settings.rootCertificate = getImmutableStringRef(aAutoStreamParams.mTrustedRootCertificateFile);
  settings.persistentTileCacheV2 = &mTileCache;
  settings.httpsClientV2         = mHttpsClient.get();
  settings.hostNamePort          = getImmutableStringRef(aAutoStreamParams.mHostNamePort);
  settings.uriBasePath           = getImmutableStringRef(aAutoStreamParams.mUriBasePath);
  settings.apiKey                = getImmutableStringRef(aAutoStreamParams.mApiKey);

  // Start AutoStream
  mAllocatorFactory      = std::make_unique<AutoStream::CAllocatorFactory>(mAllocatorSettings);
  bool autoStreamStarted = mAutoStream.start(settings, *mAllocatorFactory);
  if (!autoStreamStarted)
  {
    std::cerr << "Failed to start AutoStream" << std::endl;
    return false;
  }

  printCertificateStatus(getCertificateStatus());

  mMapVersionAndHash = getMapVersion();
  if (!validateMapVersion(mMapVersionAndHash))
  {
    return false;
  }

  if (!storeHdMapHandle())
  {
    return false;
  }

  return true;
}

bool CAutoStreamInterface::isInitialized() const noexcept
{
  return mHdMapAccess->isValid();
}

bool CAutoStreamInterface::storeHdMapHandle()
{
  mHdMap = &mAutoStream.getHdMap();
  if (!mHdMap->isValid())
  {
    std::cerr << "HD map object is invalid." << std::endl;
    return false;
  }

  mHdMapAccess = mHdMap->createHdMapAccess(kMapLayer, mMapVersionAndHash.mapVersion);

  if (!mHdMapAccess)
  {
    std::cerr << "Failed to acquire map access object." << std::endl;
    return false;
  }

  if (!mHdMapAccess->isValid())
  {
    std::cerr << "Map access object is invalid." << std::endl;
    return false;
  }

  return true;
}

bool CAutoStreamInterface::initializeTileCache(
  const AutoStream::CImmutableStringReference& aTileCachePath)
{
  if (!mTileCache.initialize(aTileCachePath))
  {
    std::cerr << "Failed to initialize persistent tile cache." << std::endl;
    return false;
  }

  return true;
}

bool CAutoStreamInterface::initializeHttpDataUsageLogger(
  const AutoStream::CImmutableStringReference& aLogFilePath)
{
  mHttpDataLogger = std::make_unique<AutoStream::Reference::CHttpDataUsageLogger>();

  const bool clearHttpDataUsageLog = true;
  if (!mHttpDataLogger->initialize(aLogFilePath, clearHttpDataUsageLog))
  {
    std::cerr << "Failed to initialize http data usage logger." << std::endl;
    return false;
  }

  return true;
}

void CAutoStreamInterface::initializeHttpsClient(const CAutoStreamParameters& aAutoStreamParams)
{
  auto caCertificatesFile =
    AutoStream::CImmutableStringReference(aAutoStreamParams.mCaCertificatesFilePath.data(),
                                          aAutoStreamParams.mCaCertificatesFilePath.length());
  mHttpsClient = std::make_unique<AutoStream::Reference::CBoostHttpsClientV2>(
    caCertificatesFile, static_cast<uint8_t>(aAutoStreamParams.mNumConnections));
}

AutoStream::CAutoStream::TCertificatesStatus CAutoStreamInterface::getCertificateStatus()
{
  auto updateCertificateStatus = AutoStream::CAutoStream::kCertificatesStatusOther;
  try
  {
    updateCertificateStatus = mAutoStream.updateCertificates(AutoStream::CCallParameters());
  }
  catch (const std::exception&)
  {
    std::cerr << " Exception thrown in UpdateCertificates method." << std::endl;
  }

  return updateCertificateStatus;
}

AutoStream::CMapVersionAndHash CAutoStreamInterface::getMapVersion()
{
  // Get map version, needed for getting map access later
  AutoStream::CMapVersionAndHash mapVersionAndHash;
  try
  {
    mapVersionAndHash =
      mAutoStream.getLatestMapVersion(kMapLayer, 0U, AutoStream::CCallParameters());
  }
  catch (const std::exception&)
  {
    std::cerr << "Exception thrown" << std::endl;
  }

  return mapVersionAndHash;
}
}
}
}