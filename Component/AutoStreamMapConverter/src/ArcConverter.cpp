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

#include "AutoStreamMapConverter/ArcConverter.hpp"

#include "TomTom/AutoStream/HdMap/HdMapSpeedRestrictions.h"

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

CAutoStreamArcConverter::CAutoStreamArcConverter(
  const lanelet::projection::UtmProjector& aUtmProjector)
{
  mLaneConverter = std::make_unique<CAutoStreamLaneConverter>(aUtmProjector);
}

bool CAutoStreamArcConverter::convertArc(const AutoStream::HdMap::TArc&         aArc,
                                         const AutoStream::HdMap::CHdMapAccess* aMapAccess,
                                         std::vector<lanelet::Area>&            aAreas,
                                         std::vector<lanelet::Lanelet>&         aLanelets,
                                         std::vector<CAutoStreamLaneMetaData>&  aConnections,
                                         std::set<lanelet::Id>& aInvalidConnectionsOut)
{
  if (!validateMapAccess(aMapAccess))
  {
    return false;
  }

  try
  {
    // Get speed restrictions
    const AutoStream::HdMap::CHdMapSpeedRestrictions& speedRestrictions =
      aMapAccess->getSpeedRestrictions();

    // Get and convert lane borders
    CAutoStreamArcData laneData = getLanes(aArc, aMapAccess);
    mLaneConverter->convertLanes(
      laneData, aArc, speedRestrictions, aAreas, aLanelets, aInvalidConnectionsOut);
    aConnections = laneData.mLaneMetaData;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception thrown: " << e.what() << std::endl;
    return false;
  }

  return true;
}

CAutoStreamArcData
CAutoStreamArcConverter::getLanes(const AutoStream::HdMap::TArc&         aArc,
                                  const AutoStream::HdMap::CHdMapAccess* aMapAccess) const
{
  CAutoStreamArcData laneData;

  // Only lane groups can be converted
  const auto arcType = aMapAccess->arcType(aArc);
  if (arcType != AutoStream::HdMap::HdRoad::TArcType::kArcTypeLaneGroup)
  {
    std::cerr << "Only lane groups can be converted, conversion of arc with type " << arcType
              << " failed." << std::endl;
    return laneData;
  }

  // Iterate over lanes, from right to left
  uint32_t    numberOfLanes = aMapAccess->nrOfLanesOrTrajectories(aArc);
  const auto& drivingSide   = aMapAccess->drivingSide(aArc);
  for (uint32_t idx = 0; idx < numberOfLanes; ++idx)
  {
    // Get lane with given index from arc and store data
    auto lane = aMapAccess->getLaneOrTrajectory(aArc, idx);

    // For last lane, store left border as well
    getLaneMetaDataAndBorders(lane, drivingSide, idx == numberOfLanes - 1, laneData);
  }

  return laneData;
}

bool CAutoStreamArcConverter::validateMapAccess(
  const AutoStream::HdMap::CHdMapAccess* aMapAccess) const noexcept
{
  if (!aMapAccess)
  {
    std::cerr << "Invalid map access pointer." << std::endl;
    return false;
  }

  if (!aMapAccess->isValid())
  {
    std::cerr << "Provided map access is invalid." << std::endl;
    return false;
  }

  return true;
}

void CAutoStreamArcConverter::getLaneMetaDataAndBorders(
  const AutoStream::HdMap::HdRoad::CLaneOrTrajectory& aLane,
  const AutoStream::HdMap::HdRoad::TDrivingSide       aDrivingSide,
  bool                                                aStoreLeftBorder,
  CAutoStreamArcData&                                 aLaneData) const
{
  // Store all data
  aLaneData.mLaneMetaData.emplace_back(getLaneMetaData(aLane, aDrivingSide));
  storeLaneBorders(aLane, aStoreLeftBorder, aDrivingSide, aLaneData);
}

CAutoStreamLaneMetaData CAutoStreamArcConverter::getLaneMetaData(
  const AutoStream::HdMap::HdRoad::CLaneOrTrajectory& aLane,
  const AutoStream::HdMap::HdRoad::TDrivingSide       aDrivingSide) const
{
  // Add meta data
  CAutoStreamLaneMetaData metaData;
  metaData.mDrivingSide            = aDrivingSide;
  metaData.mLaneWidthCm            = aLane.laneWidth();
  metaData.mLaneLengthCm           = aLane.laneLength();
  metaData.mType                   = aLane.laneType();
  metaData.mOpposingTrafficAllowed = aLane.isOpposingTrafficPossible();
  for (uint32_t conIdx = 0; conIdx < aLane.nrOfOutgoingLaneConnections(); ++conIdx)
  {
    metaData.mConnectionsOut.emplace_back(aLane.outgoingLaneConnection(conIdx));
  }

  return metaData;
}

void CAutoStreamArcConverter::storeLaneBorders(
  const AutoStream::HdMap::HdRoad::CLaneOrTrajectory& aLane,
  const bool                                          aStoreSecondBorder,
  const AutoStream::HdMap::HdRoad::TDrivingSide       aDrivingSide,
  CAutoStreamArcData&                                 aLaneData) const
{
  auto firstBorder  = AutoStream::HdMap::HdRoad::CLaneOrTrajectory::kBorderSideRight;
  auto secondBorder = AutoStream::HdMap::HdRoad::CLaneOrTrajectory::kBorderSideLeft;

  if (aDrivingSide == AutoStream::HdMap::HdRoad::TDrivingSide::kDrivingSideLeft)
  {
    firstBorder  = AutoStream::HdMap::HdRoad::CLaneOrTrajectory::kBorderSideLeft;
    secondBorder = AutoStream::HdMap::HdRoad::CLaneOrTrajectory::kBorderSideRight;
  }

  // Store first lane border (e.g. left border is same as right border next lane when driving on the
  // right)
  aLaneData.mLaneBorders.emplace_back(aLane.laneBorder(firstBorder));

  // Store second border if needed
  if (aStoreSecondBorder)
  {
    aLaneData.mLaneBorders.emplace_back(aLane.laneBorder(secondBorder));
  }
}
}
}
}