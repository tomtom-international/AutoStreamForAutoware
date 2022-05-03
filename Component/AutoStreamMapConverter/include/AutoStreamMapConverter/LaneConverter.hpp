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

#ifndef TOMTOM_AUTOSTREAM_MAP_CONVERTER_LANE_CONVERTER_H
#define TOMTOM_AUTOSTREAM_MAP_CONVERTER_LANE_CONVERTER_H

#include "DataTypes.hpp"

#include "TomTom/AutoStream/HdMap/HdMapSpeedRestrictions.h"
#include "TomTom/AutoStream/HdMap/HdRoadDataTypes.h"
#include "TomTom/AutoStream/HdMap/HdRoadEnums.h"

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_projection/UTM.h>

#include <vector>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

/**
 * Class that performs the conversion of an AutoStream arc to a set of lanelet2 lanes.
 */
class CAutoStreamLaneConverter
{
public:
  /**
   * CAutoStreamLaneConverter objects cannot be constructed without projector.
   */
  CAutoStreamLaneConverter() = delete;

  /**
   * Construct a new CAutoStreamLaneConverter object.
   *
   * @param[in] aUtmProjector Projector that can be used for converting coordinates to UTM.
   */
  explicit CAutoStreamLaneConverter(const lanelet::projection::UtmProjector& aUtmProjector);

  /**
   * Convert a given AutoStream arc to lanelet2 lanes. An AutoStream arc typically contains multiple
   * lanes.
   *
   * @param[in] aArcData AutoStream arc that must be converted.
   * @param[in] aArc Arc to which lanes belong (needed for retrieving speed limits).
   * @param[in] aSpeedRestrictions Speed restrictions object (needed for retrieving speed limits).
   * @param[in,out] aAreas Areas created from the given lanes will be added to this vector.
   * @param[in,out] aLanelets Lanelets created from the given lanes will be added to this vector.
   * @param[in, out] aInvalidConnectionsOut Ids of triangular lanelets that should not be used as
   * outgoing connection.
   * @retval True If conversion succeeded.
   * @retval False If conversion failed.
   */
  bool convertLanes(CAutoStreamArcData&                               aArcData,
                    const AutoStream::HdMap::TArc&                    aArc,
                    const AutoStream::HdMap::CHdMapSpeedRestrictions& aSpeedRestrictions,
                    std::vector<lanelet::Area>&                       aAreas,
                    std::vector<lanelet::Lanelet>&                    aLanelets,
                    std::set<lanelet::Id>&                            aInvalidConnectionsOut);

private:
  /**
   * Retrieve speed restriction and set it in the given lanelet.
   *
   * @param[in] aMapSpeedRestrictions AutoStream object containing speed restrictions.
   * @param[in] aArc Arc from which speed restriction can be retrieved.
   * @param[in] laneIdx Index of the lane for which speed restriction must be retrieved.
   * @param[in] aLanelet Lane type for which the speed restriction must be retrieved.
   * @param[out] aLanelet Lanelet to which the speed restriction must be added as a speed limit.
   */
  void setSpeedLimit(const AutoStream::HdMap::CHdMapSpeedRestrictions& aMapSpeedRestrictions,
                     const AutoStream::HdMap::TArc&                    aArc,
                     const uint32_t                                    laneIdx,
                     const AutoStream::HdMap::HdRoad::TLaneType        aLaneType,
                     lanelet::Lanelet                                  aLanelet) const;

  /**
   * Convert all given borders to line strings.
   *
   * @param[in] aBorders Borders that require conversion.
   * @param[out] aLineStrings Vector in which converted borders will be stored.
   * @retval True If conversion succeeded.
   * @retval False If conversion failed.
   */
  bool convertLaneBordersToLineStrings(
    const std::vector<AutoStream::HdMap::HdRoad::CLaneBorder>& aBorders,
    std::vector<lanelet::LineString3d>&                        aLineStrings) const;

  /**
   * Remove connections of this lane if both lane border lines end at the same position (converging
   * triangular lane).
   *
   * @param[in] aLeft Left border line of the lanelet.
   * @param[in] aRight Right border line of the lanelet.
   * @param[in, out] aLaneletMetaData Lanelet meta data.
   */
  void removeConnectionsIfConvergingTriangularLane(lanelet::LineString3d&   aLeft,
                                                   lanelet::LineString3d&   aRight,
                                                   CAutoStreamLaneMetaData& aLaneletMetaData) const;

  /**
   * Store lane if both border lines start at the same position (diverging triangular lane).
   *
   * @param[in] aLeft Left border line of the lanelet.
   * @param[in] aRight Right border line of the lanelet.
   * @param[in] aLaneletId Id of the lanelet for which the border lines are provided.
   * @param[in] aInvalidConnectionsOut Set in which IDs of inverse triangular lanes must be stored.
   */
  void storeIfDivergingTriangularLane(lanelet::LineString3d& aLeft,
                                      lanelet::LineString3d& aRight,
                                      lanelet::Id            aLaneletId,
                                      std::set<lanelet::Id>& aInvalidConnectionsOut) const;

  /**
   * Create a lanelet with given borders and use meta data to set properties.
   *
   * @param[in] aLeftBorder Left border line of the lanelet.
   * @param[in] aRightBorder Right border line of the lanelet.
   * @param[in] aLaneMetaData Meta that can be used for setting lanelet attributes.
   * @retval lanelet::Lanelet Lanelet that has been created.
   */
  lanelet::Lanelet getLanelet(const lanelet::LineString3d&   aLeftBorder,
                              const lanelet::LineString3d&   aRightBorder,
                              const CAutoStreamLaneMetaData& aLaneMetaData) const;

  /**
   * Create an area using the given borders and use meta data to set properties.
   *
   * @param[in] aLeftBorder Left border line of the area.
   * @param[in] aRightBorder Right border line of the area.
   * @param[in] aLaneMetaData Meta that can be used for setting area attributes.
   * @retval lanelet::Area Area that has been created.
   */
  lanelet::Area getArea(lanelet::LineString3d&         aLeftBorder,
                        lanelet::LineString3d&         aRightBorder,
                        const CAutoStreamLaneMetaData& aLaneMetaData) const;

private:
  lanelet::projection::UtmProjector mUtmProjector;
};
}
}
}
#endif