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

#ifndef TOMTOM_AUTOSTREAM_MAP_CONVERTER_MAP_CONVERTER_H
#define TOMTOM_AUTOSTREAM_MAP_CONVERTER_MAP_CONVERTER_H

#include "ArcConverter.hpp"
#include "AutoStreamInterface.hpp"
#include "TrafficSignConverter.hpp"

#include <lanelet2_core/primitives/Area.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/Polygon.h>
#include <lanelet2_projection/UTM.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace TomTom {
namespace AutoStreamForAutoware {
namespace AutoStreamMapConverter {

typedef std::map<lanelet::Id, std::vector<std::pair<lanelet::Id, lanelet::Id>>> TLinePointIdMap;

/**
 * Class that performs the conversion of a map delivered via AutoStream to lanelet2 map for
 * Autoware.
 *
 */
class CAutoStreamMapConverter
{
public:
  /**
   * Construct a new CAutoStreamMapConverter object
   */
  CAutoStreamMapConverter();

  /**
   * Initialize AutoStream, which is needed for retrieving map data.
   *
   * @param[in] aAutoStreamParams Set of parameters needed for initializing AutoStream.
   * @retval True If AutoStream is initialized successfully.
   * @retval False If AutoStream initialization failed.
   */
  bool initializeAutoStream(const CAutoStreamParameters& aAutoStreamParams);

  /**
   * Store a lanelet2 map for the given bounding box.
   *
   * @param[in] aBoundingBox Area for which map must be stored.
   * @retval True If map was stored successfully.
   * @retval False If storing the map failed.
   */
  bool storeMap(const AutoStream::TBoundingBox& aBoundingBox);

  /**
   * Get the name of the output file.
   *
   * @retval std::string Name of the output file.
   */
  std::string getOutputFileName() const noexcept;

  /**
   * Set the output file name that must be used for storing a converted map.
   *
   * @param[in] aOutputFileName Name of the output file.
   */
  void setOutputFileName(const std::string& aOutputFileName) noexcept;

private:
  /**
   * Convert AutoStream arcs to lanelets and areas. Areas are solved without considering
   * connectivity. Lanelets will be added to lanelet map and connectivity information will be
   * provided in connection map.
   *
   * @param[in] aAutoStreamArcKeys Keys of arcs that must be converted.
   * @param[out] aLaneletMap Map containing the lanelets associated with each of the AutoStream
   * arcs.
   * @param[out] aConnectionMap Map containing the connections associated with each of the
   * AutoStream arcs.
   * @param[in, out] aInvalidConnectionsOut Ids of triangular lanelets that should not be used as
   * outgoing connection.
   */
  void arcSetToLanelet(
    const AutoStream::HdMap::TArcKeys&                                          aAutoStreamArcKeys,
    std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>&        aLaneletMap,
    std::map<AutoStream::HdMap::TArcKey, std::vector<CAutoStreamLaneMetaData>>& aConnectionMap,
    std::set<lanelet::Id>& aInvalidConnectionsOut);

  /**
   * Store connectivity information for lanelets.
   *
   * @param[in] aLaneletMap Set of lanelets per arc for which the connectivity information
   * should be stored.
   * @param[in] aConnectionMap Map including connectivity information for outgoing connections.
   * @param[in] aInvalidConnectionsOut Lanelet IDs of lanes to which no connections are allowed.
   * @retval std::map<lanelet::Id, lanelet::Point3d> Connectivity information.
   */
  std::map<lanelet::Id, lanelet::Point3d> storeLaneletConnectivity(
    std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>& aLaneletMap,
    const std::map<AutoStream::HdMap::TArcKey, std::vector<CAutoStreamLaneMetaData>>&
                                 aConnectionMap,
    const std::set<lanelet::Id>& aInvalidConnectionsOut);

  /**
   * Convert all AutoStream arcs in the given bounding box.
   *
   * @param[in] aBoundingBox Area in which arcs must be retrieved and converted.
   * @retval True If conversion succeeded.
   * @retval False If conversion failed.
   */
  bool convertArcsInBoundingBox(const AutoStream::TBoundingBox& aBoundingBox);

  /**
   * Convert all AutoStream traffic signs in the given bounding box.
   *
   * @param[in] aBoundingBox Area in which traffic signs must be retrieved and converted.
   * @retval True If conversion succeeded.
   * @retval False If conversion failed.
   */
  bool convertTrafficSignsInBoundingBox(const AutoStream::TBoundingBox& aBoundingBox);

  /**
   * Make sure all connection for a given arc are handled properly.
   *
   * @param[in] aCurrentArcLaneMetaData Meta data for current arc, contains connectivity
   * information.
   * @param[in] aCurrentArcLaneletVector Lanelets association with the current arc
   * @param[in|out] aLaneletMap Set of all lanelets. Some will be updated to represent connectivity
   * information.
   * @param[in] aIdPointMap Map that contains point connections.
   * @param[in] aInvalidConnectionsOut Lanelet IDs of lanes to which no connections are allowed.
   */
  void addConnectionsToArc(
    const std::vector<CAutoStreamLaneMetaData>&                          aCurrentArcLaneMetaData,
    std::vector<lanelet::Lanelet>&                                       aCurrentArcLaneletVector,
    std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>& aLaneletMap,
    std::map<lanelet::Id, lanelet::Point3d>&                             aIdPointMap,
    const std::set<lanelet::Id>&                                         aInvalidConnectionsOut);

  /**
   * Add the point mapping for a line string with given ID to the lanelet point map.
   *
   * @param[in] aLineStringId Id of the line string that changed.
   * @param[in] aPointIdOld Old ID of a point in the line string.
   * @param[in] aPointIdNew New ID of the point that changed in the line string.
   * @param[in|out] aLaneletPointMap Map in which information has to be added.
   */
  void addToMap(const lanelet::Id& aLineStringId,
                const lanelet::Id& aPointIdOld,
                const lanelet::Id& aPointIdNew,
                TLinePointIdMap&   aLaneletPointMap) const;

  /**
   * Whenever an area contains the given line string, all points with the old ID must be replaced by
   * a new point.
   *
   * @param[in] aLineStringId Areas with containing this line string need to be updated.
   * @param[in] aOldPointId ID of the point that must be replaced within the area.
   * @param[in] aNewPoint New point that must be used instead of the old point.
   */
  void replacePointInAreas(const lanelet::Id       aLineStringId,
                           const lanelet::Id       aOldPointId,
                           const lanelet::Point3d& aNewPoint);

  /**
   * Store a connection between two lanes by adding a mapping between points to the point map. Skip
   * invalid connections.
   *
   * @param[in] aConnectedArcKey AutoStream arc key of the connected arc.
   * @param[in] aConnectedLaneIdx Lane index of the connected lane on the connected arc.
   * @param[in] aCurrentLanelet Current lanelet for which connection must be added.
   * @param[in|out] aLaneletMap Vector with all lanelets that needs to be updated.
   * @param[in] aIdPointMap Map with connected points.
   * @param[in] aInvalidConnectionsOut Lanelet IDs of lanes to which no connections are allowed.
   */
  void
  storeConnection(const AutoStream::HdMap::TArcKey& aConnectedArcKey,
                  const uint32_t                    aConnectedLaneIdx,
                  lanelet::Lanelet&                 aCurrentLanelet,
                  std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>& aLaneletMap,
                  std::map<lanelet::Id, lanelet::Point3d>&                             aIdPointMap,
                  const std::set<lanelet::Id>& aInvalidConnectionsOut);

  /**
   * Add connection given in point map to given lane border.
   * @param[in, out] aLaneBorder Lane border to which connections must be added.
   * @param[in] aIdPointMap Map that contains point connection information.
   * @retval True If a connection has been added.
   * @retval False If no connection was added.
   */
  bool addConnectionsToLaneBorder(lanelet::LineString3d&                   aLaneBorder,
                                  std::map<lanelet::Id, lanelet::Point3d>& aIdPointMap) const;

  /**
   * Add the given connections to the given lanelets.
   *
   * @param[in|out] aLaneletMap All lanelets some of which need to be updated to reflect
   * connections.
   * @param[in] aIdPointMap Map with connections.
   */
  void
  addConnections(std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>& aLaneletMap,
                 std::map<lanelet::Id, lanelet::Point3d>& aIdPointMap) const;

  /**
   * Store the arcs in the given vector which have a valid ID within the appropriate member
   * variable.
   *
   * @param[in] aLaneletMap Vector with lanelets.
   */
  void storeValidLanelets(
    const std::map<AutoStream::HdMap::TArcKey, std::vector<lanelet::Lanelet>>& aLaneletMap);

  /**
   * Add a mapping from the old point ID to the new point to the given map, recursively if needed.
   *
   * @param[in] aOldPointId Id of the point that must later be replaced.
   * @param[in] aNewPoint Point that must replace the old point.
   * @param[in, out] aIdPointMap Map to which the mapping must be added.
   */
  void addPointToMapping(const lanelet::Id                        aOldPointId,
                         const lanelet::Point3d                   aNewPoint,
                         std::map<lanelet::Id, lanelet::Point3d>& aIdPointMap) const;

  /**
   * Get a UTM projector that can be used for converting coordinates for the given bounding box.
   *
   * @param[in] aBoundingBox Bounding box indicating in which area map data will be retrieved.
   * @retval lanelet::projection::UtmProjector UTM projecting object.
   */
  lanelet::projection::UtmProjector
  getUtmProjector(const AutoStream::TBoundingBox& aBoundingBox) const;

  /**
   * Store a lanelet2 map that has been created using the given UTM projector.
   *
   * @param[in] aUtmProjector UTM projector that was used while converting an AutoStream map to
   * lanelet format.
   */
  void storeMap(const lanelet::projection::UtmProjector& aUtmProjector) const;

  /**
   * Update AutoStream HD map access pointer as member variable by retrieving it from the AutoStream
   * interface.
   *
   * @retval True If getting and updating access succeeded.
   * @retval False If getting access failed.
   */
  bool updateMapAccess();

  AutoStream::HdMap::CHdMapAccess* mMapAccess;

  std::unique_ptr<CAutoStreamArcConverter>         mArcConverter;
  CAutoStreamInterface                             mAutoStreamInterface;
  std::unique_ptr<CAutoStreamTrafficSignConverter> mTrafficSignConverter;

  std::string mOutputFilename;

  std::vector<lanelet::Area>      mAreas;
  std::vector<lanelet::Lanelet>   mLanelets;
  std::vector<lanelet::Polygon3d> mTrafficSignPolygons;
};
}
}
}
#endif