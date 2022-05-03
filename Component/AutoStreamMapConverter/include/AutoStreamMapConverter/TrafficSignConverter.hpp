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

#ifndef TOMTOM_AUTOSTREAM_MAP_CONVERTER_TRAFFIC_SIGN_CONVERTER_H
#define TOMTOM_AUTOSTREAM_MAP_CONVERTER_TRAFFIC_SIGN_CONVERTER_H

#include "TomTom/AutoStream/HdMap/HdMapAccess.h"
#include "TomTom/AutoStream/HdMap/HdMapTrafficSign.h"
#include "TomTom/AutoStream/HdMap/TrafficSignDataTypes.h"

#include <lanelet2_core/primitives/Polygon.h>
#include <lanelet2_projection/UTM.h>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

/**
 * Class that performs the conversion of an AutoStream traffic sign to a lanelet polygon
 * representing the bounding box around the traffic sign.
 */
class CAutoStreamTrafficSignConverter
{
public:
  /**
   * Constructing a CAutoStreamTrafficSignConverter object requires a projector.
   */
  CAutoStreamTrafficSignConverter() = delete;

  /**
   * Construct a new CAutoStreamTrafficSignConverter object with a UTM projector.
   *
   * @param[in] aUtmProjector Projector needed for converting coordinates to UTM.
   */
  explicit CAutoStreamTrafficSignConverter(const lanelet::projection::UtmProjector& aUtmProjector);

  /**
   * Convert a given traffic sign to a polygon.
   *
   * @param[in] aTrafficSign Traffic sign that must be converted.
   * @param[in] aMapAccess Map access that can be used to retrieve required information.
   * @param[out] aTrafficSignPolygon Instance used to store converted traffic
   * sign.
   * @retval True If conversion succeeded.
   * @retval False If conversion failed.
   */
  bool convertTrafficSign(const AutoStream::HdMap::TTrafficSign& aTrafficSign,
                          const AutoStream::HdMap::CHdMapAccess* aMapAccess,
                          lanelet::Polygon3d&                    aTrafficSignPolygon) const;

private:
  /**
   * Compute the corners of the traffic sign bounding box in UTM coordinates and turn points into a
   * lanelet polygon.
   *
   * @param[in] aPosition Traffic sign center of mass position.
   * @param[in] aNormal Traffic sign normal in degrees with respect to north.
   * @param[in] aSize Traffic sign size.
   * @retval lanelet::Polygon3d Polygon representing the traffic sign bounding box.
   */
  lanelet::Polygon3d
  getTrafficSignBoundingBox(const AutoStream::TCoordinate3D&                           aPosition,
                            const double&                                              aNormal,
                            const AutoStream::HdMap::HdMapTrafficSignLayer::TSignSize& aSize) const;

  /**
   * Transform a normal in deg from AutoStream (azimuth of the sign face relative to the Equator) to
   * angle with respect to north (positive clockwise).
   *
   * @param[in] aNormalDeg Angle that must be converted.
   * @retval double Converted angle in degrees.
   */
  double transformNormal(const double aNormalDeg) const;

  lanelet::projection::UtmProjector mUtmProjector;
};
}
}
}
#endif