
@echo on
pushd %~dp0
mkdir build 
cd build

cmake.exe ^
-G "Ninja" ^
-DCMAKE_INSTALL_PREFIX:PATH="." ^
-DCMAKE_TOOLCHAIN_FILE="C:/Program Files (x86)/Microsoft Azure Sphere SDK/CMakeFiles/AzureSphereToolchain.cmake" ^
-DAZURE_SPHERE_TARGET_APPLICATION_RUNTIME_VERSION="2" ^
-DAZURE_SPHERE_TARGET_BETA_APIS="" ^
-DAZURE_SPHERE_TARGET_HARDWARE_DEFINITION_DIRECTORY="%CD%/../Hardware/mt3620_rdb" ^
-DAZURE_SPHERE_TARGET_HARDWARE_DEFINITION="sample_hardware.json" ^
--no-warn-unused-cli ^
-DCMAKE_BUILD_TYPE="Debug" ^
-DCMAKE_MAKE_PROGRAM="ninja.exe" ^
..

ninja

popd