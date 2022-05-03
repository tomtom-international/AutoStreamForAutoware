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

#include "AutoStreamMapConverter/MapConverter.hpp"

#include "TomTom/AutoStream/HdMap/HdMapArc.h"
#include "TomTom/AutoStream/HdMap/HdMapTrafficSigns.h"

#include <lanelet2_io/Io.h>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

CAutoStreamMapConverter::CAutoStreamMapConverter() {}

bool CAutoStreamMapConverter::initializeAutoStream(const CAutoStreamParameters& aAutoStreamParams)
{
  return mAutoStreamInterface.initializeAutoStream(aAutoStreamParams);
}

bool CAutoStreamMapConverter::storeMap(const AutoStream::TBoundingBox& aBoundingBox)
{
  if (!aBoundingBox.isValid())
  {
    std::cerr << "Bounding box is invalid, storing map failed." << std::endl;
    return false;
  }

  if (!mAutoStreamInterface.isInitialized())
  {
    std::cerr << "AutoStream was not initialized, storing map failed." << std::endl;
    return false;
  }

  if (mOutputFilename.empty())
  {
    std::cerr << "No output file name set, storing map failed." << std::endl;
    return false;
  }

  if (!updateMapAccess())
  {
    std::cerr << "Getting valid map access failed." << std::endl;
    return false;
  }

  // Initialize converters for given bounding box
  auto utmProjector     = getUtmProjector(aBoundingBox);
  mArcConverter         = std::make_unique<CAutoStreamArcConverter>(utmProjector);
  mTrafficSignConverter = std::make_unique<CAutoStreamTrafficSignConverter>(utmProjector);

  mLanelets.clear();
  mAreas.clear();
  mTrafficSignPolygons.clear();

  if (!convertArcsInBoundingBox(aBoundingBox))
  {
    std::cerr << "Converting AutoStream arcs failed, storing map failed." << std::endl;
    return false;
  }

  if (!convertTrafficSignsInBoundingBox(aBoundingBox))
  {
    std::cerr << "Converting AutoStream traffic signs failed, storing map failed." << std::endl;
    return false;
  }

  storeMap(utmProjector);

  return true;
}

bool CAutoStreamMapConverter::convertArcsInBoundingBox(const AutoStream::TBoundingBox& aBoundingBox)
{
  if (!mArcConverter)
  {
    std::cerr << "Arc converter has not been initialized." << std::endl;
    return false;
  }

  try
  {
    // Retrieve arc keys within bounding box
    const AutoStream::CCallParameters callParams;
    const AutoStream::HdMap::TArcKeys keys = mMapAccess->arcKeysInArea(aBoundingBox, callParams);

    // Convert arcs without considering connections
    std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>        laneletMap;
    std::map<AutoStream::HdMap::TArcKey, std::vector<CAutoStreamLaneMetaData>> connectionMap;
    std::set<lanelet::Id> invalidConnectionsOut;

    arcSetToLanelet(keys, laneletMap, connectionMap, invalidConnectionsOut);
    std::map<lanelet::Id, lanelet::Point3d> idPointMap =
      storeLaneletConnectivity(laneletMap, connectionMap, invalidConnectionsOut);
    addConnections(laneletMap, idPointMap);

    storeValidLanelets(laneletMap);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception thrown when converting AutoStream arcs: " << e.what() << std::endl;
    return false;
  }

  return true;
}

void CAutoStreamMapConverter::addConnections(
  std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>& aLaneletMap,
  std::map<lanelet::Id, lanelet::Point3d>&                             aIdPointMap) const
{
  for (auto& laneletPair : aLaneletMap)
  {
    auto arcKey = laneletPair.first;
    for (size_t laneIdx = 0; laneIdx < laneletPair.second.size(); ++laneIdx)
    {
      lanelet::LineString3d left  = aLaneletMap[arcKey][laneIdx].leftBound();
      lanelet::LineString3d right = aLaneletMap[arcKey][laneIdx].rightBound();

      const bool leftChanged  = addConnectionsToLaneBorder(left, aIdPointMap);
      const bool rightChanged = addConnectionsToLaneBorder(right, aIdPointMap);

      if (leftChanged || rightChanged)
      {
        lanelet::Lanelet& oldLanelet = aLaneletMap[arcKey][laneIdx];
        lanelet::Lanelet  newLanelet(oldLanelet.id(), left, right);
        newLanelet.attributes()      = oldLanelet.attributes();
        aLaneletMap[arcKey][laneIdx] = newLanelet;
      }
    }
  }
}

bool CAutoStreamMapConverter::addConnectionsToLaneBorder(
  lanelet::LineString3d& aLaneBorder, std::map<lanelet::Id, lanelet::Point3d>& aIdPointMap) const
{
  bool changed = false;

  if (!aLaneBorder.empty())
  {
    if (aIdPointMap.find(aLaneBorder.front().id()) != aIdPointMap.end())
    {
      aLaneBorder.front() = aIdPointMap[aLaneBorder.front().id()];
      changed             = true;
    }
    if (aIdPointMap.find(aLaneBorder.back().id()) != aIdPointMap.end())
    {
      aLaneBorder.back() = aIdPointMap[aLaneBorder.back().id()];
      changed            = true;
    }
  }
  return changed;
}

void CAutoStreamMapConverter::arcSetToLanelet(
  const AutoStream::HdMap::TArcKeys&                                          aAutoStreamArcKeys,
  std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>&        aLaneletMap,
  std::map<AutoStream::HdMap::TArcKey, std::vector<CAutoStreamLaneMetaData>>& aConnectionMap,
  std::set<lanelet::Id>& aInvalidConnectionsOut)
{
  const AutoStream::CCallParameters callParams;
  for (const auto& key : aAutoStreamArcKeys.getSet())
  {
    std::vector<lanelet::Lanelet>        lanelets;
    std::vector<CAutoStreamLaneMetaData> connections;
    const AutoStream::HdMap::TArc&       arc = mMapAccess->key2Arc(key, callParams);
    if (!mArcConverter->convertArc(
          arc, mMapAccess, mAreas, lanelets, connections, aInvalidConnectionsOut))
    {
      std::cerr << "Converting arc failed" << std::endl;
      continue;
    }

    // Store such that connections can later be handled properly
    aLaneletMap[key]    = lanelets;
    aConnectionMap[key] = connections;
  }
}

std::map<lanelet::Id, lanelet::Point3d> CAutoStreamMapConverter::storeLaneletConnectivity(
  std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>&              aLaneletMap,
  const std::map<AutoStream::HdMap::TArcKey, std::vector<CAutoStreamLaneMetaData>>& aConnectionMap,
  const std::set<lanelet::Id>& aInvalidConnectionsOut)
{
  std::map<lanelet::Id, lanelet::Point3d> idPointMap;

  for (const auto& p : aConnectionMap)
  {
    const auto& currentArcKey = p.first;
    if (aLaneletMap.find(currentArcKey) == aLaneletMap.end())
    {
      std::cerr
        << "Cannot check connectivity for unconverted arc! (how did it end up in connection map?)"
        << std::endl;
      continue;
    }

    const auto& currentArcLaneMetaData  = p.second;
    auto&       currentArcLaneletVector = aLaneletMap[currentArcKey];
    if (currentArcLaneMetaData.size() != currentArcLaneletVector.size())
    {
      std::cerr << "Error in conversion: number of AutoStream lanes differs from number of lanelets"
                << std::endl;
      continue;
    }

    addConnectionsToArc(currentArcLaneMetaData,
                        currentArcLaneletVector,
                        aLaneletMap,
                        idPointMap,
                        aInvalidConnectionsOut);
  }

  return idPointMap;
}

void CAutoStreamMapConverter::addConnectionsToArc(
  const std::vector<CAutoStreamLaneMetaData>&                          aCurrentArcLaneMetaData,
  std::vector<lanelet::Lanelet>&                                       aCurrentArcLaneletVector,
  std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>& aLaneletMap,
  std::map<lanelet::Id, lanelet::Point3d>&                             aIdPointMap,
  const std::set<lanelet::Id>&                                         aInvalidConnectionsOut)
{
  for (size_t laneIdx = 0; laneIdx < aCurrentArcLaneMetaData.size(); ++laneIdx)
  {
    auto& currentLanelet = aCurrentArcLaneletVector[laneIdx];
    if (currentLanelet.id() == lanelet::InvalId)
    {
      // Current AutoStream lane was converted to area, connections of areas will be skipped
      continue;
    }

    for (const auto& c : aCurrentArcLaneMetaData[laneIdx].mConnectionsOut)
    {
      auto connectedArcKey  = c.first;
      auto connectedLaneIdx = c.second;
      if (aLaneletMap.find(connectedArcKey) != aLaneletMap.end())
      {
        storeConnection(connectedArcKey,
                        connectedLaneIdx,
                        currentLanelet,
                        aLaneletMap,
                        aIdPointMap,
                        aInvalidConnectionsOut);
      }
    }
  }
}

void CAutoStreamMapConverter::storeValidLanelets(
  const std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>& aLaneletMap)
{
  for (const auto& p : aLaneletMap)
  {
    for (const auto& l : p.second)
    {
      if (l.id() != lanelet::InvalId)
      {
        mLanelets.emplace_back(l);
      }
    }
  }
}

void CAutoStreamMapConverter::storeConnection(
  const AutoStream::HdMap::TArcKey&                                    aConnectedArcKey,
  const uint32_t                                                       aConnectedLaneIdx,
  lanelet::Lanelet&                                                    aCurrentLanelet,
  std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>& aLaneletMap,
  std::map<lanelet::Id, lanelet::Point3d>&                             aIdPointMap,
  const std::set<lanelet::Id>&                                         aInvalidConnectionsOut)
{
  auto connectedArcLaneletVector = aLaneletMap.at(aConnectedArcKey);

  if (aConnectedLaneIdx >= connectedArcLaneletVector.size())
  {
    std::cerr << "Connected to lane with ID " << aConnectedLaneIdx << ", but arc has only "
              << connectedArcLaneletVector.size() << " lanelets. Skip connection." << std::endl;
    return;
  }

  auto connectedLanelet = connectedArcLaneletVector[aConnectedLaneIdx];

  if (connectedLanelet.id() == lanelet::InvalId
      || aInvalidConnectionsOut.find(connectedLanelet.id()) != aInvalidConnectionsOut.end())
  {
    return;
  }

  lanelet::LineString3d connectedLeft  = connectedLanelet.leftBound();
  lanelet::LineString3d connectedRight = connectedLanelet.rightBound();

  // Store connection between lanelets
  lanelet::Id      oldIdLeft     = connectedLeft.front().id();
  lanelet::Id      oldIdRight    = connectedRight.front().id();
  lanelet::Point3d newFirstLeft  = aCurrentLanelet.leftBound().back();
  lanelet::Point3d newFirstRight = aCurrentLanelet.rightBound().back();
  addPointToMapping(oldIdLeft, newFirstLeft, aIdPointMap);
  addPointToMapping(oldIdRight, newFirstRight, aIdPointMap);

  // Update areas accordingly
  replacePointInAreas(connectedLeft.id(), oldIdLeft, newFirstLeft);
  replacePointInAreas(connectedRight.id(), oldIdRight, newFirstRight);
}

void CAutoStreamMapConverter::addPointToMapping(
  const lanelet::Id                        aOldPointId,
  const lanelet::Point3d                   aNewPoint,
  std::map<lanelet::Id, lanelet::Point3d>& aIdPointMap) const
{
  /* If key exists, value of this key must be added as a key with new value as well
   *  aIdPointMap[1] = 2
   * If we want to add 1 -> 3, the desired outcome is
   *  aIdPointMap[1] = 3
   *  aIdPointMap[2] = 3
   * to avoid other users of point 2 will still cause duplicate points.
   */
  if (aIdPointMap.find(aOldPointId) != aIdPointMap.end())
  {
    addPointToMapping(aIdPointMap[aOldPointId].id(), aNewPoint, aIdPointMap);
  }

  aIdPointMap[aOldPointId] = aNewPoint;
}

void CAutoStreamMapConverter::addToMap(const lanelet::Id& aLineStringId,
                                       const lanelet::Id& aPointIdOld,
                                       const lanelet::Id& aPointIdNew,
                                       TLinePointIdMap&   aLaneletPointMap) const
{
  if (aLaneletPointMap.find(aLineStringId) == aLaneletPointMap.end())
  {
    aLaneletPointMap[aLineStringId] = {};
  }
  aLaneletPointMap.emplace(aPointIdOld, aPointIdNew);
}

void CAutoStreamMapConverter::replacePointInAreas(const lanelet::Id       aLineStringId,
                                                  const lanelet::Id       aOldPointId,
                                                  const lanelet::Point3d& aNewPoint)
{
  for (lanelet::Area a : mAreas)
  {
    auto outerBound = a.outerBound();

    bool lineStringUsedInArea = false;
    for (const auto& border : outerBound)
    {
      lineStringUsedInArea |= border.id() == aLineStringId;
    }

    if (lineStringUsedInArea)
    {
      for (lanelet::LineString3d& border : outerBound)
      {
        // Replace old point by new one (if present)
        for (size_t idx = 0; idx < border.size(); ++idx)
        {
          if (border[idx].id() == aOldPointId)
          {
            border[idx] = aNewPoint;
          }
        }
      }
    }
  }
}

bool CAutoStreamMapConverter::convertTrafficSignsInBoundingBox(
  const AutoStream::TBoundingBox& aBoundingBox)
{
  if (!mTrafficSignConverter)
  {
    std::cerr << "Traffic sign converter has not been initialized." << std::endl;
    return false;
  }

  try
  {
    // Retrieve traffic sign keys within bounding box
    const AutoStream::CCallParameters         callParams;
    const AutoStream::HdMap::TTrafficSignKeys keys =
      mMapAccess->getTrafficSigns().trafficSignKeysInArea(aBoundingBox, callParams);

    // Convert traffic signs one by one
    for (const auto& key : keys.getSet())
    {
      const AutoStream::HdMap::TTrafficSign& signAutoStream =
        mMapAccess->getTrafficSigns().key2TrafficSign(key, callParams);

      lanelet::Polygon3d trafficSign;
      if (mTrafficSignConverter->convertTrafficSign(signAutoStream, mMapAccess, trafficSign))
      {
        mTrafficSignPolygons.emplace_back(trafficSign);
      }
      else
      {
        std::cerr << "Converting traffic sign failed" << std::endl;
      }
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception thrown when converting AutoStream traffic signs: " << e.what()
              << std::endl;
    return false;
  }

  return true;
}

bool CAutoStreamMapConverter::updateMapAccess()
{
  mMapAccess = mAutoStreamInterface.getHdMapAccess();
  if (!mMapAccess)
  {
    std::cerr << "Failed retrieving a map handle." << std::endl;
    return false;
  }

  return true;
}

std::string CAutoStreamMapConverter::getOutputFileName() const noexcept
{
  return mOutputFilename;
}

void CAutoStreamMapConverter::setOutputFileName(const std::string& aOutputFileName) noexcept
{
  mOutputFilename = aOutputFileName;
}

lanelet::projection::UtmProjector
CAutoStreamMapConverter::getUtmProjector(const AutoStream::TBoundingBox& aBoundingBox) const
{
  const auto            bbSWCornerLat = aBoundingBox.getCornerSW().getLatDegree();
  const auto            bbSWCornerLon = aBoundingBox.getCornerSW().getLonDegree();
  const lanelet::Origin origin({ bbSWCornerLat, bbSWCornerLon });

  return lanelet::projection::UtmProjector(origin);
}

void CAutoStreamMapConverter::storeMap(const lanelet::projection::UtmProjector& aUtmProjector) const
{
  auto map = std::make_unique<lanelet::LaneletMap>();

  for (const auto& lanelet : mLanelets)
  {
    map->add(lanelet);
  }

  for (const auto& area : mAreas)
  {
    map->add(area);
  }

  for (const auto& sign : mTrafficSignPolygons)
  {
    map->add(sign);
  }

  lanelet::write(mOutputFilename, *map, aUtmProjector);
}
}
}
}