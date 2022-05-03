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

#include "AutoStreamMapConverter/LaneConverter.hpp"
#include "AutoStreamMapConverter/ConversionHelpers.hpp"

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/primitives/BasicRegulatoryElements.h>
#include <lanelet2_core/primitives/Point.h>

#include <stdexcept>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

CAutoStreamLaneConverter::CAutoStreamLaneConverter(
  const lanelet::projection::UtmProjector& aUtmProjector)
  : mUtmProjector(aUtmProjector)
{
}

bool CAutoStreamLaneConverter::convertLanes(
  CAutoStreamArcData&                               aArcData,
  const AutoStream::HdMap::TArc&                    aArc,
  const AutoStream::HdMap::CHdMapSpeedRestrictions& aSpeedRestrictions,
  std::vector<lanelet::Area>&                       aAreas,
  std::vector<lanelet::Lanelet>&                    aLanelets,
  std::set<lanelet::Id>&                            aInvalidConnectionsOut)
{
  if (!aArcData.isValid())
  {
    throw std::out_of_range("Invalid dimensions in arc data: cannot convert lanes.");
  }

  if (aArcData.empty())
  {
    return true;
  }

  // Converting all borders to line strings, for double lines, only the first line is converted.
  std::vector<lanelet::LineString3d> lineStrings;
  if (!convertLaneBordersToLineStrings(aArcData.mLaneBorders, lineStrings))
  {
    return false;
  }

  // Convert all lanes (number of line strings/borders equals number of lines plus one)
  for (uint32_t laneIdx = 0; laneIdx < aArcData.mLaneMetaData.size(); ++laneIdx)
  {
    // For ease of writing when referring to lane border (boundary) lines
    auto&                    leftBorder  = lineStrings[laneIdx + 1];
    auto&                    rightBorder = lineStrings[laneIdx];
    CAutoStreamLaneMetaData& metaData    = aArcData.mLaneMetaData[laneIdx];

    // Store converted lane
    if (isLanelet(metaData.mType))
    {
      removeConnectionsIfConvergingTriangularLane(leftBorder, rightBorder, metaData);

      if (metaData.mDrivingSide == AutoStream::HdMap::HdRoad::TDrivingSide::kDrivingSideRight)
      {
        aLanelets.emplace_back(getLanelet(leftBorder, rightBorder, metaData));
      }
      else
      {
        aLanelets.emplace_back(getLanelet(rightBorder, leftBorder, metaData));
      }

      storeIfDivergingTriangularLane(
        leftBorder, rightBorder, aLanelets.back().id(), aInvalidConnectionsOut);
      setSpeedLimit(aSpeedRestrictions, aArc, laneIdx, metaData.mType, aLanelets.back());
    }
    else
    {
      if (isArea(metaData.mType))
      {
        lanelet::Area areaIdx = getArea(leftBorder, rightBorder, metaData);
        aAreas.emplace_back(areaIdx);
      }

      // Add invalid lanelet such that number of lanelets equals number of AutoStream lanes
      aLanelets.emplace_back(lanelet::InvalId);
    }
  }

  return true;
}

void CAutoStreamLaneConverter::removeConnectionsIfConvergingTriangularLane(
  lanelet::LineString3d&   aLeft,
  lanelet::LineString3d&   aRight,
  CAutoStreamLaneMetaData& aLaneletMetaData) const
{
  /*
   * Converging triangular lanes (e.g. A) may have outgoing connections in AutoStream (to C),
   * however, lanelets only have connections if both lane border end points are shared. Connections
   * from A to C will be removed to keep the lanelet map clean.
   * -----\
   *   A   \
   * -------+----
   *   B      C
   * -------+----
   */
  if (isSame(aLeft.back(), aRight.back()))
  {
    aLaneletMetaData.mConnectionsOut.clear();
    aLeft.back() = aRight.back();
  }
}

void CAutoStreamLaneConverter::storeIfDivergingTriangularLane(
  lanelet::LineString3d& aLeft,
  lanelet::LineString3d& aRight,
  lanelet::Id            aLaneletId,
  std::set<lanelet::Id>& aInvalidConnectionsOut) const
{
  /* Diverging triangular lanes (e.g. B) may be an outgoing connections in AutoStream (from A),
   * however, lanelets only have connections if both lane border end points are shared.
   * Connections from A to B will be removed to keep the lanelet map clean.
   *       /-----
   *      /  B
   * ----+--------
   *   A     C
   * ----+--------
   */
  if (isSame(aLeft.front(), aRight.front()))
  {
    aInvalidConnectionsOut.insert(aLaneletId);
    aLeft.front() = aRight.front();
  }
}

void CAutoStreamLaneConverter::setSpeedLimit(
  const AutoStream::HdMap::CHdMapSpeedRestrictions& aMapSpeedRestrictions,
  const AutoStream::HdMap::TArc&                    aArc,
  const uint32_t                                    laneIdx,
  const AutoStream::HdMap::HdRoad::TLaneType        aLaneType,
  lanelet::Lanelet                                  aLanelet) const
{
  auto speedRestrictions = aMapSpeedRestrictions.getSpeedRestrictions(
    aArc, laneIdx, getVehicleType(aLaneType), AutoStream::CCallParameters());

  uint32_t numberOfRestrictions = speedRestrictions.getNrLaneSpeedRestrictions();
  if (numberOfRestrictions == 0)
  {
    return;
  }

  if (numberOfRestrictions > 1)
  {
    std::cerr << numberOfRestrictions << " speed restriction available, only first one will used."
              << std::endl;
  }

  // Set speed limit (only the first one)
  auto laneSpeedRestriction = speedRestrictions.getLaneSpeedRestrictions(0);
  aLanelet.attributes()[lanelet::AttributeName::SpeedLimit] = getSpeedLimit(laneSpeedRestriction);
}

bool CAutoStreamLaneConverter::convertLaneBordersToLineStrings(
  const std::vector<AutoStream::HdMap::HdRoad::CLaneBorder>& aBorders,
  std::vector<lanelet::LineString3d>&                        aLineStrings) const
{
  for (const auto& laneBorder : aBorders)
  {
    if (laneBorder.getSize() == 0)
    {
      std::cerr << "Lane border without components: not supported." << std::endl;
      return false;
    }

    // Store line string
    aLineStrings.emplace_back(convertLaneBorder(laneBorder, mUtmProjector));
  }
  return true;
}

lanelet::Lanelet
CAutoStreamLaneConverter::getLanelet(const lanelet::LineString3d&   aLeftBorder,
                                     const lanelet::LineString3d&   aRightBorder,
                                     const CAutoStreamLaneMetaData& aLaneMetaData) const
{
  lanelet::Lanelet lanelet(lanelet::utils::getId(), aLeftBorder, aRightBorder);

  // Set lanelet attributes
  lanelet.attributes()[lanelet::AttributeName::OneWay]  = !aLaneMetaData.mOpposingTrafficAllowed;
  lanelet.attributes()[lanelet::AttributeName::Subtype] = getLaneSubtype(aLaneMetaData.mType);

  return lanelet;
}

lanelet::Area CAutoStreamLaneConverter::getArea(lanelet::LineString3d&         aLeftBorder,
                                                lanelet::LineString3d&         aRightBorder,
                                                const CAutoStreamLaneMetaData& aLaneMetaData) const
{
  // Get area bounds
  auto areaBounds = getAreaBounds(aLeftBorder, aRightBorder);

  // Connect line strings in clockwise order
  lanelet::Area area(lanelet::utils::getId(),
                     { areaBounds[0], areaBounds[1], areaBounds[2], areaBounds[3] });

  // Set area attributes
  area.attributes()[lanelet::AttributeName::Subtype] = getLaneSubtype(aLaneMetaData.mType);

  return area;
}
}
}
}