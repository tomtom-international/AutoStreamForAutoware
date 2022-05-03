# Known issues

The map models used in AutoStream and lanelet2 are not fully compatible. The currently known limitations of the converter are explained in this file. 

## Lanes

* AutoStream has passing restrictions for a lane border, these are currently not converted, since lanelet2 derives these restrictions from the lane border `subtype` attribute.

* For lanelet2 lane borders of type virtual, lane changes are allowed to enable lane changes around entry or exit lanes that sometimes have virtual lane borders.  

* AutoStream lanes of type `Unknown`, `NonDrivable` and `Restricted` are not converted since there is no lanelet2 equivalent `type` of these non-drivable lanes.

* AutoStream lane borders of type `Unknown`, `None`, `RoadSurfaceUnknown`  and `PhysicalBarrierUnknownBarrier` are not associated with lanelet `type` attributes since it is unknown which attribute values should be used.

* AutoStream lane border lines may have multiple components, for example, in case of a double line, two components are present. In AutoStream all components (i.e. each of the lines) have their own geometry. Lanelets do not allow for multiple border lines with different geometries on one side of the lane. The converter therefore _only_ converts the first component and uses the lanelet `subtype` attribute to reflect double lines, e.g. solid/dashed.

* In AutoStream, one lane may be associated with multiple speed limits. For example, speed limit of X km/h for the first 200 m, then a speed limit of Y km/h for the next 200 m. Lanelets only allow for setting a single speed limit. The converter converts each AutoStream lane to *one* lanelet and _only_ uses the first speed limit in that case. A more generic solution (currently not supported) would be to convert an AutoStream lane with multiple speed limits to multiple lanelets, each with its own speed limit.

* Speed restrictions are vehicle type dependent in AutoStream, however, only one type is used during conversion. The `PublicBus` for bus lanes, `Resident` for bicycle lanes and parking, `passenger car` for Unknown, Drivable, Emergency, Hov and Restricted.

* Lanelet lane borders of `type` `curbstone` have a `subtype` (`low` or `high`) which indicates whether a vehicle may drive over it, AutoStream does not have this information represented explicitly.

* AutoStream does not have the distinction between road (rural) and highway lane types which is present in lanelet2. Lanelet2 uses this to derive speed limits. Speed limits are set during conversion, however, the `location` attribute (`urban` or `nonurban`) is not set.

* AutoStream lanes of type parking are converted to lanelet2 areas. During the conversion of areas, connections are not considered. As a result, connected parking lanes may appear as disconnected after conversion to a lanelet2 map. 

## Traffic signs

* AutoStream traffic sign `category` and `subcategory` are not converted to lanelet2 traffic sign `subtype`. Rational: AutoStream traffic signs have a `category` (e.g. `kCategoryInformationSign`) and `subcategory` (e.g. `kSubcategoryInformationSignHighwayEnd`). With lanelet2, line strings of type traffic sign have a `subtype` tag that contains the actual type of the traffic sign. This `subtype` is encoded as ISO 3166 region code + traffic sign number (e.g. `subtype=de206` for a German stop sign or `subtype=usR1-1` for a US stop sign). There is no trivial mapping from AutoStream `category` and `subcategory` to lanelet2 `subtype`. AutoStream uses the same set of subcategories in all countries and does not distinguish between country specific differences in appearance of signs of the same `type`. Lanelet2 requires the traffic sign number that defines the precise appearance of the traffic sign, which varies from country to country. In addition, the AutoStream `subcategory` is less refined. For example, AutoStream
`kSubcategorySpeedRestrictionSpeed` indicates a traffic sign is about a speed restriction, but the speed restriction value is stored elsewhere.
* We did not convert shape and dominant colors enums present in AutoStream, because lanelet2 traffic signs have no equivalent attributes. The traffic sign bounding box _is_ converted.

## Regulatory elements
* Regulatory elements are not created while converting the map. Speed limits are set within the lanelets using `SpeedLimit` attribute. The `OneWay` attribute is set as well. 