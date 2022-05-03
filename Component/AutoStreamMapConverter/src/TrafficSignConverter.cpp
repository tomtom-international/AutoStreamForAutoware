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

#include "AutoStreamMapConverter/TrafficSignConverter.hpp"

#include "AutoStreamMapConverter/ConversionHelpers.hpp"

#include "TomTom/AutoStream/HdMap/HdMapTrafficSigns.h"

#include <lanelet2_core/LaneletMap.h>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

CAutoStreamTrafficSignConverter::CAutoStreamTrafficSignConverter(
  const lanelet::projection::UtmProjector& aUtmProjector)
  : mUtmProjector(aUtmProjector)
{
}

bool CAutoStreamTrafficSignConverter::convertTrafficSign(
  const AutoStream::HdMap::TTrafficSign& aTrafficSign,
  const AutoStream::HdMap::CHdMapAccess* aMapAccess,
  lanelet::Polygon3d&                    aTrafficSignPolygon) const
{
  // Collect data needed for reconstructing bounding box around traffic sign
  const AutoStream::TCoordinate3D centerOfMass =
    aMapAccess->getTrafficSigns().getCenterOfMass(aTrafficSign);
  double headingNormalDeg =
    aMapAccess->getTrafficSigns().getNormal(aTrafficSign) * Constants::kMilliDeg2deg;
  const AutoStream::HdMap::HdMapTrafficSignLayer::TSignSize size =
    aMapAccess->getTrafficSigns().getSize(aTrafficSign);

  // Complete traffic sign
  aTrafficSignPolygon = getTrafficSignBoundingBox(centerOfMass, headingNormalDeg, size);
  aTrafficSignPolygon.attributes()[lanelet::AttributeName::Type] =
    lanelet::AttributeValueString::TrafficSign;

  return true;
}

lanelet::Polygon3d CAutoStreamTrafficSignConverter::getTrafficSignBoundingBox(
  const AutoStream::TCoordinate3D&                           aPosition,
  const double&                                              aNormal,
  const AutoStream::HdMap::HdMapTrafficSignLayer::TSignSize& aSize) const
{
  // Position
  const AutoStream::TCoordinate pos          = aPosition.getXY();
  const int32_t                 heightSignMm = aPosition.getHeight();

  // Traffic sign size
  const double signWidthMeter   = aSize.widthCentimeter * Constants::kCm2meter;
  const auto   halfSignHeightMm = 5 * static_cast<int32_t>(aSize.heightCentimeter);

  // Compute coordinate of edges
  double     normalTransDeg = transformNormal(aNormal);
  const auto leftEdge  = moveCoordinateDistance(pos, signWidthMeter / 2, normalTransDeg - 90.0);
  const auto rightEdge = moveCoordinateDistance(pos, signWidthMeter / 2, normalTransDeg + 90.0);
  const auto low       = heightSignMm - halfSignHeightMm;
  const auto high      = heightSignMm + halfSignHeightMm;

  // Convert sign corner points to UTM
  const lanelet::Point3d lowerLeft  = toUtm({ leftEdge, low }, mUtmProjector);
  const lanelet::Point3d upperLeft  = toUtm({ leftEdge, high }, mUtmProjector);
  const lanelet::Point3d lowerRight = toUtm({ rightEdge, low }, mUtmProjector);
  const lanelet::Point3d upperRight = toUtm({ rightEdge, high }, mUtmProjector);

  return lanelet::Polygon3d(lanelet::utils::getId(),
                            { lowerLeft, upperLeft, upperRight, lowerRight });
}

double CAutoStreamTrafficSignConverter::transformNormal(const double aNormalDeg) const
{
  // Axes are flipped
  const double xNew = sin(aNormalDeg * Constants::kDeg2rad);
  const double yNew = cos(aNormalDeg * Constants::kDeg2rad);

  // Compute angle
  return atan2(yNew, xNew) * Constants::kRad2deg;
}
}
}
}