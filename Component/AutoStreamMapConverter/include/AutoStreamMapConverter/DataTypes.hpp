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

#ifndef TOMTOM_AUTOSTREAM_MAP_CONVERTER_DATA_TYPES_H
#define TOMTOM_AUTOSTREAM_MAP_CONVERTER_DATA_TYPES_H

#include "TomTom/AutoStream/HdMap/HdMapAccess.h"
#include "TomTom/AutoStream/HdMap/HdMapArc.h"
#include "TomTom/AutoStream/HdMap/HdRoadDataTypes.h"

#include <utility>
#include <vector>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

/**
 * Structure that summarizes lane meta data.
 */
struct CAutoStreamLaneMetaData
{
  AutoStream::HdMap::HdRoad::TDrivingSide                      mDrivingSide;
  bool                                                         mOpposingTrafficAllowed;
  uint32_t                                                     mLaneWidthCm;
  uint32_t                                                     mLaneLengthCm;
  AutoStream::HdMap::HdRoad::TLaneType                         mType;
  std::vector<std::pair<AutoStream::HdMap::TArcKey, uint32_t>> mConnectionsOut;
};

/**
 * Structure that summarizes relevant arc information in lanelet2 friendly way.
 */
struct CAutoStreamArcData
{
  /**
   * Check if arc data is empty.
   *
   * @retval True If no data is present.
   * @retval False If data is present.
   */
  bool empty() const noexcept;

  /**
   * Validate dimensions of arc data stored in members.
   *
   * @retval True If dimensions are compatible.
   * @retval False If dimensions are not compatible.
   */
  bool isValid() const noexcept;

  /**
   * Get number of lanes.
   *
   * @return size_t Number of lanes.
   */
  size_t size() const noexcept;

  std::vector<AutoStream::HdMap::HdRoad::CLaneBorder> mLaneBorders;
  std::vector<CAutoStreamLaneMetaData>                mLaneMetaData;
};
}
}
}
#endif