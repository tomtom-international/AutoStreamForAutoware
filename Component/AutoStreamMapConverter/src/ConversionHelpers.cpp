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

#include "AutoStreamMapConverter/ConversionHelpers.hpp"
#include "AutoStreamMapConverter/MapConverter.hpp"

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/utility/Units.h>

#include <map>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

namespace Constants {
// Lane border lines in AutoStream may have multiple components, each one with its own geometry.
// E.g. a double line is represented by two components. Lanelet2 supports one border line per lane.
// Use the AutoStream border with index kRelevantBorderIndex as geometry for the lanelet border.
constexpr uint32_t kRelevantBorderIndex = 0;
}

/**
 * Initialize a mapping from AutoStream lane border types to lanetlet2 LineString types.
 *
 * @return std::map<AutoStream::HdMap::HdRoad::TLaneBorderType, std::string> Mapping.
 */
std::map<AutoStream::HdMap::HdRoad::TLaneBorderType, std::string> getLaneBorderTypeMapping()
{
  using namespace lanelet;
  using namespace AutoStream::HdMap::HdRoad;

  // Define mapping for types that have appropriate equivalent in lanelet2
  std::map<TLaneBorderType, std::string> mapping;
  mapping[kLaneBorderTypeRoadSurfaceShortDashedLine]     = AttributeValueString::LineThin;
  mapping[kLaneBorderTypeRoadSurfaceLongDashedLine]      = AttributeValueString::LineThin;
  mapping[kLaneBorderTypeRoadSurfaceSingleSolidLine]     = AttributeValueString::LineThin;
  mapping[kLaneBorderTypeRoadSurfaceShadedAreaMarking]   = AttributeValueString::LineThick;
  mapping[kLaneBorderTypeRoadSurfaceRoadBorder]          = AttributeValueString::RoadBorder;
  mapping[kLaneBorderTypeRoadSurfaceUndefinedRoadBorder] = AttributeValueString::RoadBorder;
  mapping[kLaneBorderTypeRoadSurfaceNoMarking]           = AttributeValueString::Virtual;
  mapping[kLaneBorderTypeCurbLeftCurb]                   = AttributeValueString::Curbstone;
  mapping[kLaneBorderTypeCurbRightCurb]                  = AttributeValueString::Curbstone;
  mapping[kLaneBorderTypeCurbBiCurb]                     = AttributeValueString::Curbstone;
  mapping[kLaneBorderTypePhysicalBarrierGuardrail]       = AttributeValueString::GuardRail;
  mapping[kLaneBorderTypePhysicalBarrierFence]           = AttributeValueString::Fence;

  return mapping;
}

/**
 * Initialize a mapping from AutoStream lane border types to lanetlet2 LineString subtypes.
 *
 * @return std::map<AutoStream::HdMap::HdRoad::TLaneBorderType, std::string> Mapping.
 */
std::map<AutoStream::HdMap::HdRoad::TLaneBorderType, std::string> getLaneBorderSubTypeMapping()
{
  using namespace lanelet;
  using namespace AutoStream::HdMap::HdRoad;

  // Define mapping for types that have appropriate equivalent in lanelet2
  std::map<TLaneBorderType, std::string> mapping;
  mapping[kLaneBorderTypeRoadSurfaceShortDashedLine]   = AttributeValueString::Dashed;
  mapping[kLaneBorderTypeRoadSurfaceLongDashedLine]    = AttributeValueString::Dashed;
  mapping[kLaneBorderTypeRoadSurfaceSingleSolidLine]   = AttributeValueString::Solid;
  mapping[kLaneBorderTypeRoadSurfaceShadedAreaMarking] = AttributeValueString::Solid;

  return mapping;
}

/**
 * Function that turns enums into strings to ease interpretation of warnings.
 *
 * @param[in] aType Border type that must be converted.
 * @retval std::string Human readable equivalent of the type.
 */
std::string toString(AutoStream::HdMap::HdRoad::TLaneBorderType aType)
{
  using namespace AutoStream::HdMap::HdRoad;

  switch (aType)
  {
    case TLaneBorderType::kLaneBorderTypeUnknown:
      return "LaneBorderTypeUnknown";
    case TLaneBorderType::kLaneBorderTypeNone:
      return "LaneBorderTypeNone";
    case TLaneBorderType::kLaneBorderTypeRoadSurfaceUnknown:
      return "LaneBorderTypeRoadSurfaceUnknown";
    case TLaneBorderType::kLaneBorderTypePhysicalBarrierUnknownBarrier:
      return "LaneBorderTypePhysicalBarrierUnknownBarrier";
    case kLaneBorderTypeRoadSurfaceShortDashedLine:
    case kLaneBorderTypeRoadSurfaceLongDashedLine:
    case kLaneBorderTypeRoadSurfaceSingleSolidLine:
    case kLaneBorderTypeRoadSurfaceShadedAreaMarking:
    case kLaneBorderTypeRoadSurfaceRoadBorder:
    case kLaneBorderTypeRoadSurfaceUndefinedRoadBorder:
    case kLaneBorderTypeRoadSurfaceNoMarking:
    case kLaneBorderTypeCurbLeftCurb:
    case kLaneBorderTypeCurbRightCurb:
    case kLaneBorderTypeCurbBiCurb:
    case kLaneBorderTypePhysicalBarrierGuardrail:
    case kLaneBorderTypePhysicalBarrierFence:
    default:
      return std::to_string(aType);
  }
}

/**
 * Initialize a mapping from AutoStream lane types to lanetlet2 subtypes.
 *
 * @return std::map<AutoStream::HdMap::HdRoad::TLaneType, std::string> Mapping.
 */
std::map<AutoStream::HdMap::HdRoad::TLaneType, std::string> getLaneTypeMapping()
{
  using namespace lanelet;
  using namespace AutoStream::HdMap::HdRoad;

  // Define mapping for types that have appropriate equivalent in lanelet2
  std::map<TLaneType, std::string> mapping;
  mapping[kLaneTypeBicycle]   = AttributeValueString::BicycleLane;
  mapping[kLaneTypeBus]       = AttributeValueString::BusLane;
  mapping[kLaneTypeDrivable]  = AttributeValueString::Road;
  mapping[kLaneTypeEmergency] = AttributeValueString::EmergencyLane;
  mapping[kLaneTypeHov]       = AttributeValueString::Road;
  mapping[kLaneTypeParking]   = AttributeValueString::Parking;

  return mapping;
}

/**
 * Initialize a mapping from AutoStream lane types to type of vehicle that will be using the lane.
 *
 * @return std::map<AutoStream::HdMap::HdRoad::TLaneType,
 * AutoStream::HdMap::HdMapSpeedRestrictionLayer::TVehicleType> Mapping.
 */
std::map<AutoStream::HdMap::HdRoad::TLaneType,
         AutoStream::HdMap::HdMapSpeedRestrictionLayer::TVehicleType>
getLaneTypeVehicleMapping()
{
  using namespace AutoStream::HdMap;

  // Define mapping for types that have appropriate equivalent in lanelet2
  std::map<HdRoad::TLaneType, HdMapSpeedRestrictionLayer::TVehicleType> mapping;
  mapping[HdRoad::kLaneTypeUnknown]    = HdMapSpeedRestrictionLayer::kVehicleTypePassengerCar;
  mapping[HdRoad::kLaneTypeBicycle]    = HdMapSpeedRestrictionLayer::kVehicleTypeResident;
  mapping[HdRoad::kLaneTypeBus]        = HdMapSpeedRestrictionLayer::kVehicleTypePublicBus;
  mapping[HdRoad::kLaneTypeDrivable]   = HdMapSpeedRestrictionLayer::kVehicleTypePassengerCar;
  mapping[HdRoad::kLaneTypeEmergency]  = HdMapSpeedRestrictionLayer::kVehicleTypePassengerCar;
  mapping[HdRoad::kLaneTypeHov]        = HdMapSpeedRestrictionLayer::kVehicleTypePassengerCar;
  mapping[HdRoad::kLaneTypeParking]    = HdMapSpeedRestrictionLayer::kVehicleTypeResident;
  mapping[HdRoad::kLaneTypeRestricted] = HdMapSpeedRestrictionLayer::kVehicleTypePassengerCar;

  return mapping;
}

lanelet::Point3d toUtm(const AutoStream::TCoordinate3D          aPointInNds,
                       const lanelet::projection::UtmProjector& aUTMProjector)
{
  lanelet::GPSPoint pointInWGS84 { aPointInNds.getXY().getLatDegree(),
                                   aPointInNds.getXY().getLonDegree(),
                                   aPointInNds.getHeight() * Constants::kMm2meter };

  const auto pointInUTM = aUTMProjector.forward(pointInWGS84);

  lanelet::Point3d utmPoint3D(
    lanelet::utils::getId(), pointInUTM.x(), pointInUTM.y(), pointInUTM.z());

  return lanelet::Point3d(lanelet::utils::getId(), pointInUTM.x(), pointInUTM.y(), pointInUTM.z());
}

lanelet::LineString3d convertLaneBorder(const AutoStream::HdMap::HdRoad::CLaneBorder& aLaneBorder,
                                        const lanelet::projection::UtmProjector&      aUtmProjector)
{
  auto convertedLineString = convertLine(
    aLaneBorder.getLaneBorderComponent(Constants::kRelevantBorderIndex).laneBorderLine(),
    aUtmProjector);

  setTypeAndSubtype(aLaneBorder, convertedLineString);

  // Remaining attributes
  convertedLineString.attributes()["width"] = aLaneBorder.width() * Constants::kCm2meter;
  if (isPainted(aLaneBorder))
  {
    convertedLineString.attributes()["color"] =
      aLaneBorder.getLaneBorderComponent(Constants::kRelevantBorderIndex).laneBorderColor()
          == AutoStream::HdMap::HdRoad::kLaneBorderColorYellow
        ? "yellow"
        : "white";
  }

  return convertedLineString;
}

bool isSame(const lanelet::Point3d& aPoint1, const lanelet::Point3d& aPoint2, double aThreshold)
{
  double dx = aPoint1.x() - aPoint2.x();
  double dy = aPoint1.y() - aPoint2.y();
  double dz = aPoint1.z() - aPoint2.z();
  return dx * dx + dy * dy + dz * dz < aThreshold * aThreshold;
}

void setTypeAndSubtype(const AutoStream::HdMap::HdRoad::CLaneBorder& aLaneBorder,
                       lanelet::LineString3d&                        aConvertedLineString)
{
  static auto laneBorderTypeMap    = getLaneBorderTypeMapping();
  static auto laneBorderSubTypeMap = getLaneBorderSubTypeMapping();

  bool singleBorderLine = aLaneBorder.getSize() == 1;

  if (singleBorderLine)
  {
    auto type =
      aLaneBorder.getLaneBorderComponent(Constants::kRelevantBorderIndex).laneBorderType();
    if (laneBorderTypeMap.find(type) == laneBorderTypeMap.end())
    {
      std::cerr << "No support for converting AutoStream borders of type " << toString(type)
                << std::endl;
      return;
    }

    aConvertedLineString.attributes()[lanelet::AttributeName::Type] = laneBorderTypeMap.at(type);

    // Allow lane changes for virtual lane borders
    if (laneBorderTypeMap.at(type) == lanelet::AttributeValueString::Virtual)
    {
      aConvertedLineString.attributes()["lane_change"] = "yes";
    }

    // Set subtype if needed (only needed for painted lines which are present in the map)
    if (laneBorderSubTypeMap.find(type) != laneBorderSubTypeMap.end())
    {
      aConvertedLineString.attributes()[lanelet::AttributeName::Subtype] =
        laneBorderSubTypeMap.at(type);
    }
  }
  else
  {
    // Type must be LineThin for all double lines
    aConvertedLineString.attributes()[lanelet::AttributeName::Type] =
      lanelet::AttributeValueString::LineThin;

    // Lanelet2 only support solid/solid, dashed/solid, solid/dashed
    if (isDashed(aLaneBorder.getLaneBorderComponent(0).laneBorderType()))
    {
      aConvertedLineString.attributes()[lanelet::AttributeName::Subtype] =
        lanelet::AttributeValueString::DashedSolid;
    }
    else if (isDashed(aLaneBorder.getLaneBorderComponent(1).laneBorderType()))
    {
      aConvertedLineString.attributes()[lanelet::AttributeName::Subtype] =
        lanelet::AttributeValueString::SolidDashed;
    }
    else
    {
      aConvertedLineString.attributes()[lanelet::AttributeName::Subtype] =
        lanelet::AttributeValueString::SolidSolid;
    }
  }
}

lanelet::LineString3d convertLine(const AutoStream::HdMap::HdRoad::CIterable3DShapePoints& aLineIn,
                                  const lanelet::projection::UtmProjector& aUTMProjector)
{
  lanelet::LineString3d lineString;
  lineString.setId(lanelet::utils::getId());
  for (const AutoStream::TCoordinate3D& point : aLineIn)
  {
    lineString.push_back(toUtm(point, aUTMProjector));
  }

  return lineString;
}

bool isLanelet(const AutoStream::HdMap::HdRoad::TLaneType aLaneType)
{
  return (aLaneType == AutoStream::HdMap::HdRoad::TLaneType::kLaneTypeBicycle
          || aLaneType == AutoStream::HdMap::HdRoad::TLaneType::kLaneTypeBus
          || aLaneType == AutoStream::HdMap::HdRoad::TLaneType::kLaneTypeHov
          || aLaneType == AutoStream::HdMap::HdRoad::TLaneType::kLaneTypeRestricted
          || aLaneType == AutoStream::HdMap::HdRoad::TLaneType::kLaneTypeEmergency
          || aLaneType == AutoStream::HdMap::HdRoad::TLaneType::kLaneTypeDrivable);
}

bool isArea(const AutoStream::HdMap::HdRoad::TLaneType aLaneType)
{
  return (aLaneType == AutoStream::HdMap::HdRoad::TLaneType::kLaneTypeParking);
}

std::array<lanelet::LineString3d, Constants::kNumberOfSidesArea>
getAreaBounds(lanelet::LineString3d& aLeftBorder, lanelet::LineString3d& aRightBorder)
{
  // Bottom line string of area is equal to right border flipped
  lanelet::LineString3d bottomBound = createFlippedLineString(aRightBorder);

  // Clockwise ordering: create borders
  lanelet::LineString3d leftBound, rightBound;
  rightBound.push_back(aLeftBorder.back());
  rightBound.push_back(aRightBorder.back());
  rightBound.setId(lanelet::utils::getId());
  leftBound.push_back(aRightBorder.front());
  leftBound.push_back(aLeftBorder.front());
  leftBound.setId(lanelet::utils::getId());

  // Top line string is equal to the left border
  return { aLeftBorder, rightBound, bottomBound, leftBound };
}

lanelet::LineString3d createFlippedLineString(const lanelet::LineString3d& aLineStringIn)
{
  // Copy line string, but reverse order of points
  lanelet::LineString3d flippedLineString = aLineStringIn.invert();

  // Set unique ID
  flippedLineString.setId(lanelet::utils::getId());

  return flippedLineString;
}

bool isDashed(const AutoStream::HdMap::HdRoad::TLaneBorderType aType)
{
  using namespace AutoStream::HdMap::HdRoad;

  return aType == TLaneBorderType::kLaneBorderTypeRoadSurfaceShortDashedLine
         || aType == TLaneBorderType::kLaneBorderTypeRoadSurfaceLongDashedLine;
}

std::string getLaneSubtype(const AutoStream::HdMap::HdRoad::TLaneType aType)
{
  static auto mapping = getLaneTypeMapping();

  // Default value
  std::string convertedType = "road";
  if (mapping.find(aType) != mapping.end())
  {
    convertedType = mapping[aType];
  }

  return convertedType;
}

AutoStream::HdMap::HdMapSpeedRestrictionLayer::TVehicleType
getVehicleType(const AutoStream::HdMap::HdRoad::TLaneType aLaneType)
{
  static auto mapping = getLaneTypeVehicleMapping();

  // Default
  AutoStream::HdMap::HdMapSpeedRestrictionLayer::TVehicleType type =
    AutoStream::HdMap::HdMapSpeedRestrictionLayer::kVehicleTypePassengerCar;
  if (mapping.find(aLaneType) != mapping.end())
  {
    type = mapping[aLaneType];
  }

  return type;
}

bool isPainted(const AutoStream::HdMap::HdRoad::CLaneBorder& aLaneBorder)
{
  auto type = aLaneBorder.getLaneBorderComponent(Constants::kRelevantBorderIndex).laneBorderType();
  return type == AutoStream::HdMap::HdRoad::kLaneBorderTypeRoadSurfaceSingleSolidLine
         || type == AutoStream::HdMap::HdRoad::kLaneBorderTypeRoadSurfaceLongDashedLine
         || type == AutoStream::HdMap::HdRoad::kLaneBorderTypeRoadSurfaceShadedAreaMarking
         || type == AutoStream::HdMap::HdRoad::kLaneBorderTypeRoadSurfaceShortDashedLine;
}

lanelet::Velocity getSpeedLimit(
  const AutoStream::HdMap::HdMapSpeedRestrictionLayer::CLaneSpeedRestrictions& aSpeedLimits)
{
  auto limit = static_cast<double>(aSpeedLimits.getSpeedLimitValue(0));
  if (aSpeedLimits.getSpeedUnit(0) == AutoStream::HdMap::HdMapSpeedRestrictionLayer::kSpeedUnitMph)
  {
    return lanelet::Velocity { limit * lanelet::units::MPH() };
  }

  return lanelet::Velocity { limit * lanelet::units::KmH() };
}

AutoStream::TCoordinate moveCoordinateDistance(const AutoStream::TCoordinate& aCoordinate2D,
                                               const double                   aDistanceMeter,
                                               const double                   aHeadingDeg)
{
  double headingRad = aHeadingDeg * Constants::kDeg2rad;

  // Compute new latitude and longitude
  const double distRad = aDistanceMeter / Constants::kEarthRadiusMeters;
  double       newLat  = asin(sin(aCoordinate2D.getLatRadian()) * cos(distRad)
                       + cos(aCoordinate2D.getLatRadian()) * sin(distRad) * cos(headingRad));
  double       newLon  = aCoordinate2D.getLonRadian()
                  + atan2(sin(headingRad) * sin(distRad) * cos(aCoordinate2D.getLatRadian()),
                          cos(distRad) - sin(aCoordinate2D.getLatRadian()) * sin(newLat));

  return AutoStream::TCoordinate::createFromDegrees(newLat * Constants::kRad2deg,
                                                    newLon * Constants::kRad2deg);
}
}
}
}