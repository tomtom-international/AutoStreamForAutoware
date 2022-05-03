# AutoStream For Autoware
Many applications in the domain of automated driving rely on map information for a number of purposes such as, 
- Localization
- Planning and control
- Motion prediction of other traffic participants, etc. 

The [TomTom AutoStream](https://www.tomtom.com/products/autostream/) product can be used for retrieving TomTom maps fast and efficiently for highly automated driving purposes.

Maps used by [Autoware.Auto](https://www.autoware.org/autoware-auto) are expected to be present locally in the [Lanelet2](https://www.mrt.kit.edu/z/publ/download/2018/Poggenhans2018Lanelet2.pdf) format. Hence in order to use TomTom's HD map delivered via AutoStream, in Autoware.Auto, the map data delivered via AutoStream must be converted to Lanelet2 format. 

To use TomTom maps within the Autoware framework, the following three steps are required:
1. Retrieve TomTom map data inside a given bounding box around the desired geographic location.
2. Conversion of the TomTom map to a Lanelet2 map.
3. Storage of the converted map, locally, as an osm-file.

Therefore, the map data for a geographic location could be retrieved via a bounding box represented by two of its corner coordinates, that encloses the desired location. AutoStream for Autoware converter tool uses this bounding box information, to convert the TomTom HD map data to a map represented in Lanelet2 format. 

## How to file issue 
For info on how to file or report an issue please refer to `Create a new issue` section in [CONTRIBUTING.md](CONTRIBUTING.md)

## How to contribute
If you would like to contribute to this project please check the `How to contribute` section [CONTRIBUTING.md](CONTRIBUTING.md)

# Build Instructions
- **Dependencies**: See [dependencies.md](docs/dependencies.md) for a list of dependencies
- **Build**: See [build-unix.md](docs/build-unix.md) to know how to build AutoStream for Autoware Converter
# Usage

## Configuration
Below you can see the most relevant parameters in the configuration file that must be updated. 
Notice that there are more parameters. For a complete documentation, please have a look at the 
[Application/config/settings.txt](Application/config/settings.txt) file.
```bash
# AutoStream key, mandatory for retrieving map data from the AutoStream server
apiKey: <your-key>

# Certificates needed to enable retrieving map data via AutoStream
certificatesFile: Component/AutoStream/AutoStreamClient/certs/AutoStreamRootCAs.pem
trustedRootCertificateFile: Component/AutoStream/AutoStreamClient/certs/TomTom_AutoStream_Certificate_Production_Signing_Root_CA_1.pem

# Rectangular area for which map data must be converted(bounding box)
southWestLat: 51.48
southWestLon: 5.48
northEastLat: 51.5
northEastLon: 5.5

# Name of the output file, i.e., the map that will be generated
outputFile: /some/file/path/map.osm
```

## Getting AutoStream client library and the credentials

As discussed in the above section, to run this converter tool, one must have access to `AutoStream client library`, `api key`, `certificate file` and `trusted root certificate file`. In order to get access to these credentials, [Contact us](mailto:Mariko.Saito@tomtom.com) to sign the Non-Disclosure Agreement (NDA) and Evaluation Agreement (EA) for obtaining the AutoStream client library and the API keys.

## Running the code
Once the code has been compiled, as explained [here](docs/build-unix.md), the build folder should contain an executable named `AutoStreamToLaneletApp` in the folder `build/Application`. In order to run this executable we must first update the configuration file as explained above.

The executable requires one argument, which is the path to the configuration file.
```bash
cd build
./Application/AutoStreamToLaneletApp /my/file/path/settings_private.txt
```
After running the executable the converted map for the specified bounding box is available at the 
location specified in the configuration file.

# Changelog
See [CHANGELOG](CHANGELOG.md) for more details.

# Known issues and limitations
The converter has some known limitations, which are explained in [known-issues](docs/known-issues.md).

# License
See [License](LICENSE.md)

# Disclaimer 
TomTom reserves the right to stop supporting specific version of AutoStream and the same will be communicated to the API key owners. As a result, it is at the discretion of the end-users to update the AutoStreamForAutoware converter application to keep it functional by keeping it in sync with the latest release of the same.
