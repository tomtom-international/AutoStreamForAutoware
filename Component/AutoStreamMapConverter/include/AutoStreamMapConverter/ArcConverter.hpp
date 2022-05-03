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

#ifndef TOMTOM_AUTOSTREAM_MAP_CONVERTER_ARC_CONVERTER_H
#define TOMTOM_AUTOSTREAM_MAP_CONVERTER_ARC_CONVERTER_H

#include "AutoStreamInterface.hpp"
#include "DataTypes.hpp"
#include "LaneConverter.hpp"

#include "TomTom/AutoStream/HdMap/HdMapAccess.h"
#include "TomTom/AutoStream/HdMap/HdMapArc.h"
#include "TomTom/AutoStream/HdMap/HdRoadDataTypes.h"

#include <lanelet2_core/primitives/Area.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_projection/UTM.h>

#include <memory>
#include <utility>
#include <vector>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

/**
 * Class that performs the conversion of an AutoStream map arc to a set of lanelet2 lanes.
 */
class CAutoStreamArcConverter
{
public:
  /**
   * Delete default constructor.
   */
  CAutoStreamArcConverter() = delete;

  /**
   * Construct a new CAutoStreamArcConverter object with a UTM projector.
   *
   * @param[in] aUtmProjector Projector needed for converting coordinates to UTM.
   */
  explicit CAutoStreamArcConverter(const lanelet::projection::UtmProjector& aUtmProjector);

  /**
   * Convert a given AutoStream arc to a set of lanelet2 lanes.
   *
   * @param[in] aArc Arc that must be converted.
   * @param[in] aMapAccess Map access that can be used to retrieve required information.
   * @param[out] aAreas Vector used to store converted areas.
   * @param[out] aLanelets Vector used to store converted lanelets.
   * @param[out] aConnections AutoStream arc lane meta data containing connectivity information.
   * @param[in, out] aInvalidConnectionsOut Ids of triangular lanelets that should not be used as
   * outgoing connection.
   * @retval True If conversion succeeded.
   * @retval False If conversion failed.
   */
  bool convertArc(const AutoStream::HdMap::TArc&         aArc,
                  const AutoStream::HdMap::CHdMapAccess* aMapAccess,
                  std::vector<lanelet::Area>&            aAreas,
                  std::vector<lanelet::Lanelet>&         aLanelets,
                  std::vector<CAutoStreamLaneMetaData>&  aConnections,
                  std::set<lanelet::Id>&                 aInvalidConnectionsOut);

private:
  /**
   * Validate a map access pointer.
   *
   * @param[in] aMapAccess Object that needs to be check on validity.
   * @retval True If HD map access is valid.
   * @retval False If HD map access is invalid.
   */
  bool validateMapAccess(const AutoStream::HdMap::CHdMapAccess* aMapAccess) const noexcept;

  /**
   * Get lane information in a more appropriate form for a given arc.
   *
   * @param[in] aArc Arc from which lanes must be retrieved.
   * @param[in] aMapAccess HD map access object that can be used to retrieve required information
   * about lanes.
   * @retval CAutoStreamArcData Lane information in a convenient internal datatype.
   */
  CAutoStreamArcData getLanes(const AutoStream::HdMap::TArc&         aArc,
                              const AutoStream::HdMap::CHdMapAccess* aMapAccess) const;

  /**
   * Store meta data and lane border(s) for the given AutoStream lane.
   *
   * @param[in] aLane AutoStream lane that contains data that must be stored.
   * @param[in] aDrivingSide Driving side for current lane.
   * @param[in] aStoreLeftBorder Boolean that indicates whether or not the left border must be
   * stored (right border will always be stored).
   * @param[out] aLaneData Object to which lane data must be added.
   */
  void getLaneMetaDataAndBorders(const AutoStream::HdMap::HdRoad::CLaneOrTrajectory& aLane,
                                 const AutoStream::HdMap::HdRoad::TDrivingSide       aDrivingSide,
                                 bool                aStoreLeftBorder,
                                 CAutoStreamArcData& aLaneData) const;

  /**
   * Get meta data from a given AutoStream lane in the appropriate format.
   *
   * @param[in] aLane AutoStream lane that contains the required data.
   * @param[in] aDrivingSide Driving side for current lane.
   * @retval CAutoStreamLaneMetaData Converted lane data.
   */
  CAutoStreamLaneMetaData
  getLaneMetaData(const AutoStream::HdMap::HdRoad::CLaneOrTrajectory& aLane,
                  const AutoStream::HdMap::HdRoad::TDrivingSide       aDrivingSide) const;

  /**
   * Store lane borders for the given AutoStream lane in the given structure.
   *
   * @param[in] aLane Lane for which border or borders need to be stored.
   * @param[in] aStoreSecondBorder Boolean indicating whether only the first (false) or both borders
   * (true) must be stored.
   * @param[in] aDrivingSide Driving side for given lane.
   * @param[out] aLaneData Struct to which the lane borders will be added.
   */
  void storeLaneBorders(const AutoStream::HdMap::HdRoad::CLaneOrTrajectory& aLane,
                        const bool                                          aStoreSecondBorder,
                        const AutoStream::HdMap::HdRoad::TDrivingSide       aDrivingSide,
                        CAutoStreamArcData&                                 aLaneData) const;

  std::unique_ptr<CAutoStreamLaneConverter> mLaneConverter;
};
}
}
}
#endif