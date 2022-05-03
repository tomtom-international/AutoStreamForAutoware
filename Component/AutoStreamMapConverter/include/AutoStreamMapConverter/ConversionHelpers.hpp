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

#ifndef TOMTOM_AUTOSTREAM_MAP_CONVERTER_CONVERSION_HELPERS_H
#define TOMTOM_AUTOSTREAM_MAP_CONVERTER_CONVERSION_HELPERS_H

#include "TomTom/AutoStream/HdMap/HdMapSpeedRestrictions.h"
#include "TomTom/AutoStream/HdMap/HdRoadDataTypes.h"
#include "TomTom/AutoStream/HdMap/SpeedRestrictionDataTypes.h"
#include "TomTom/AutoStream/MapBaseTypes.h"

#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_projection/UTM.h>

#include <array>
#include <math.h>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

namespace Constants {
// Areas are derived from AutoStream arcs, hence they always have four boundary lines (sides)
constexpr int kNumberOfSidesArea = 4;

// Converting units
constexpr double kCm2meter     = 0.01;
constexpr double kMm2meter     = 0.001;
constexpr double kMilliDeg2deg = 0.001;
constexpr double kDeg2rad      = M_PI / 180.;
constexpr double kRad2deg      = 180. / M_PI;

constexpr int kEarthRadiusMeters = 6378137;
}

/**
 * Set the type and subtype attributes in the given line string according to the given lane border.
 *
 * @param[in] aLaneBorder Lane border containing required data.
 * @param[out] aConvertedLineString Line string for which type and subtype must be set.
 */
void setTypeAndSubtype(const AutoStream::HdMap::HdRoad::CLaneBorder& aLaneBorder,
                       lanelet::LineString3d&                        aConvertedLineString);

/**
 * Convert given NDS coordinates to corresponding UTM coordinates.
 *
 * @param[in] aPointInNds NDS point to be converted to UTM.
 * @param[in] aUTMProjector Projector object that must be used for the conversion.
 * @return lanelet::Point3d Corresponding point in UTM coordinates.
 */
lanelet::Point3d toUtm(const AutoStream::TCoordinate3D          aPointInNds,
                       const lanelet::projection::UtmProjector& aUTMProjector);

/**
 * Convert a 3D line from AutoStream format to line string.
 *
 * @param[in] aLineIn AutoStream vector of 3D coordinates that must be converted.
 * @param[in] aUTMProjector Projector that must be used for conversion.
 * @return lanelet::LineString3d Points converted to UTM and in lanelet line string format.
 */
lanelet::LineString3d convertLine(const AutoStream::HdMap::HdRoad::CIterable3DShapePoints& aLineIn,
                                  const lanelet::projection::UtmProjector& aUTMProjector);

/**
 * Convert the given lane border to a line string.
 *
 * @param[in] aLaneBorder Lane border that must be converted.
 * @param[in] aUtmProjector Projector that must be used for conversion.
 * @retval lanelet::LineString3d Line string representing given lane border.
 */
lanelet::LineString3d convertLaneBorder(const AutoStream::HdMap::HdRoad::CLaneBorder& aLaneBorder,
                                        const lanelet::projection::UtmProjector& aUtmProjector);

/**
 * Check if two 3D points are the same.
 *
 * @param[in] aPoint1 First point, unit for each component is assumed to be meters.
 * @param[in] aPoint2 Second point, unit for each component is assumed to be meters.
 * @param[in] aThreshold Maximum distance in meters allowed between 'identical' points.
 * @retval True If both points are within the specified distance.
 * @retval False If both points are considered different.
 */
bool isSame(const lanelet::Point3d& aPoint1, const lanelet::Point3d& aPoint2, double aThreshold = 0.05);

/**
 * Check if a lane with given type must be converted to a lanelet or not.
 *
 * @param[in] aLaneType Type of the lane.
 * @retval True If lane must be converted to a lanelet.
 * @retval False If lane should not be converted to a lanelet.
 */
bool isLanelet(const AutoStream::HdMap::HdRoad::TLaneType aLaneType);

/**
 * Check if a lane with given type must be converted to an area or not.
 *
 * @param[in] aLaneType Type of the lane.
 * @retval True If lane must be converted to an area.
 * @retval False If lane should not be converted to an area.
 */
bool isArea(const AutoStream::HdMap::HdRoad::TLaneType aLaneType);

/**
 * Get the bounds that must be used for creating an area for the given lane border.
 *
 * @param[in] aLeftBorder Left lane border.
 * @param[in] aRightBorder Right lane border.
 * @return std::array<lanelet::LineString3d, Constants::kNumberOfSidesArea> Array with area bounds
 * in fixed order: top, right, bottom, left.
 */
std::array<lanelet::LineString3d, Constants::kNumberOfSidesArea>
getAreaBounds(lanelet::LineString3d& aLeftBorder, lanelet::LineString3d& aRightBorder);

/**
 * Create a line string which has same points as given line string but in reverse order.
 *
 * @param[in] aLineStringIn Line string that must be copied and flipped.
 * @return lanelet::LineString3d Flipped line string with new unique id.
 */
lanelet::LineString3d createFlippedLineString(const lanelet::LineString3d& aLineStringIn);

/**
 * Check if the given border type is a dashed line.
 *
 * @param[in] aType Border type of line that must be inspected.
 * @retval True If it is dashed.
 * @retval False If it is not dashed.
 */
bool isDashed(const AutoStream::HdMap::HdRoad::TLaneBorderType aType);

/**
 * Convert the given AutoStream lane type to lanelet2 subtype.
 *
 * @param[in] aType Type that must be converted.
 * @retval std::string Matching lanelet or area subtype.
 */
std::string getLaneSubtype(const AutoStream::HdMap::HdRoad::TLaneType aType);

/**
 * Get the vehicle type that is expected to drive on a lane of given type. Default is passenger car.
 *
 * @param[in] aLaneType Lane type.
 * @retval AutoStream::HdMap::HdMapSpeedRestrictionLayer::TVehicleType Vehicle type expected on
 * given lane.
 */
AutoStream::HdMap::HdMapSpeedRestrictionLayer::TVehicleType
getVehicleType(const AutoStream::HdMap::HdRoad::TLaneType aLaneType);

/**
 * Check if the given border is painted.
 *
 * @param[in] aLaneBorder Border that must be checked.
 * @retval True If it is painted.
 * @retval False If it is not painted.
 */
bool isPainted(const AutoStream::HdMap::HdRoad::CLaneBorder& aLaneBorder);

/**
 * Get speed limit for given AutoStream lane speed restriction object as lanelet velocity.
 *
 * @param[in] aSpeedLimits Speed restrictions from AutoStream.
 * @retval lanelet::Velocity Speed restriction as lanelet velocity object.
 */
lanelet::Velocity getSpeedLimit(
  const AutoStream::HdMap::HdMapSpeedRestrictionLayer::CLaneSpeedRestrictions& aSpeedLimits);

/**
 * Move a given coordinate a predefined distance in the direction of the given heading.
 *
 * @param[in] aPosition2D Coordinate to be moved.
 * @param[in] aDistanceMeter Distance that the position must be moved.
 * @param[in] aHeadingDeg Direction in which point must be moved.
 * @retval AutoStream::TCoordinate Coordinate after moving given distance in given direction.
 */
AutoStream::TCoordinate moveCoordinateDistance(const AutoStream::TCoordinate& aPosition2D,
                                               const double                   aDistanceMeter,
                                               const double                   aHeadingDeg);
}
}
}
#endif