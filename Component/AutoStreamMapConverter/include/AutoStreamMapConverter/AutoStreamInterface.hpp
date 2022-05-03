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

#ifndef TOMTOM_AUTOSTREAM_MAP_CONVERTER_AUTOSTREAM_INTERFACE_H
#define TOMTOM_AUTOSTREAM_MAP_CONVERTER_AUTOSTREAM_INTERFACE_H

#include "TomTom/AutoStream/AllocatorFactory.h"
#include "TomTom/AutoStream/AllocatorSettings.h"
#include "TomTom/AutoStream/AutoStream.h"
#include "TomTom/AutoStream/HdMap/HdMap.h"
#include "TomTom/AutoStream/HdMap/HdMapAccess.h"
#include "TomTom/AutoStream/ImmutableStringReference.h"
#include "TomTom/AutoStream/MapBaseTypes.h"
#include "TomTom/AutoStream/Reference/BoostHttpsClientV2/BoostHttpsClientV2.h"
#include "TomTom/AutoStream/Reference/HttpDataUsageLogger/HttpDataUsageLogger.h"
#include "TomTom/AutoStream/Reference/SqlitePersistentTileCacheV2/SqlitePersistentTileCacheV2.h"

#include <memory>
#include <string>
#include <vector>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

/**
 * Structure that is used to store AutoStream configuration parameters.
 */
struct CAutoStreamParameters
{
  std::string   mApiKey;
  std::string   mCaCertificatesFilePath;
  std::string   mHostNamePort;
  std::string   mHttpDataUsageLogPath;
  unsigned long mNumConnections;
  std::string   mPersistentTileCachePath;
  std::string   mTrustedRootCertificateFile;
  std::string   mUriBasePath;
};

/**
 * Class that performs the initialization of and interaction with AutoStream.
 */
class CAutoStreamInterface
{
public:
  /**
   * Construct a new CAutoStreamInterface object
   */
  CAutoStreamInterface();

  /**
   * Deconstructor.
   */
  ~CAutoStreamInterface();

  /**
   * Initialize AutoStream with the given set of parameters.
   *
   * @param[in] aAutoStreamParams Set of parameters needed for initializing AutoStream.
   * @retval True If initialization succeeded.
   * @retval False If initialization failed.
   */
  bool initializeAutoStream(const CAutoStreamParameters& aAutoStreamParams);

  /**
   * Get map access pointer such that data can be requested.
   *
   * @return AutoStream::HdMap::CHdMapAccess* Pointer to AutoStream HD map access object.
   */
  AutoStream::HdMap::CHdMapAccess* getHdMapAccess() const;

  /**
   * Check if AutoStream has been initialized successfully.
   *
   * @retval True If AutoStream has been initialized.
   * @retval False If AutoStream has not been initialized successfully.
   */
  bool isInitialized() const noexcept;

private:
  /**
   * Start AutoStream with given set of parameters.
   *
   * @param[in] aAutoStreamParams Set of AutoStream parameters
   * @retval True If starting AutoStream succeeded.
   * @retval False If starting AutoStream failed.
   */
  bool startAutoStream(const CAutoStreamParameters& aAutoStreamParams);

  /**
   * Initialize the persistent tile cache object.
   *
   * @param[in] aTileCachePath Path that should be used by tile cache.
   * @retval True If initializing persistent tile cache succeeded.
   * @retval False If initializing persistent tile cache failed.
   */
  bool initializeTileCache(const AutoStream::CImmutableStringReference& aTileCachePath);

  /**
   * Initialize the HTTP data usage logger object.
   *
   * @param[in] aLogFilePath Path that should be used by logger.
   * @retval True If initializing HTTP data usage logger succeeded.
   * @retval False If initializing HTTP data usage logger failed.
   */
  bool initializeHttpDataUsageLogger(const AutoStream::CImmutableStringReference& aLogFilePath);

  /**
   * Initialize the HTTPS client.
   *
   * @param[in] aAutoStreamParams Parameters needed to initialize the client.
   */
  void initializeHttpsClient(const CAutoStreamParameters& aAutoStreamParams);

  /**
   * Store HD map handle as a member variable.
   *
   * @retval True If storing map handle succeeded.
   * @retval False If storing map handle failed.
   */
  bool storeHdMapHandle();

  /**
   * Get certificate status of AutoStream member variable.
   *
   * @return AutoStream::CAutoStream::TCertificatesStatus Current status.
   */
  AutoStream::CAutoStream::TCertificatesStatus getCertificateStatus();

  /**
   * Get map version of AutoStream member variable.
   *
   * @return AutoStream::CMapVersionAndHash Map version.
   */
  AutoStream::CMapVersionAndHash getMapVersion();

  std::unique_ptr<AutoStream::CAllocatorFactory>               mAllocatorFactory;
  AutoStream::CAllocatorSettings                               mAllocatorSettings;
  AutoStream::CAutoStream                                      mAutoStream;
  const AutoStream::HdMap::CHdMap*                             mHdMap;
  AutoStream::HdMap::CHdMapAccess*                             mHdMapAccess;
  std::unique_ptr<AutoStream::Reference::CBoostHttpsClientV2>  mHttpsClient;
  std::unique_ptr<AutoStream::Reference::CHttpDataUsageLogger> mHttpDataLogger;
  AutoStream::Reference::CSqlitePersistentTileCacheV2          mTileCache;
  AutoStream::CMapVersionAndHash                               mMapVersionAndHash;
};
}
}
}
#endif