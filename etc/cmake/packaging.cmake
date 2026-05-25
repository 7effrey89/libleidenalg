set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "libleidenalg library")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/vtraag/libleidenalg")
set(CPACK_PACKAGE_VENDOR "Vincent Traag")
set(CPACK_PACKAGE_CONTACT "v.a.traag@cwts.leidenuniv.nl")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

#############################################################################
## Configuration of NuGet package metadata
#############################################################################

set(CPACK_NUGET_PACKAGE_NAME "libleidenalg.native")
set(CPACK_NUGET_PACKAGE_TITLE "libleidenalg native library")
set(CPACK_NUGET_PACKAGE_AUTHORS "Vincent Traag")
set(CPACK_NUGET_PACKAGE_OWNERS "Vincent Traag")
set(CPACK_NUGET_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
set(CPACK_NUGET_PACKAGE_TAGS "leiden graph community-detection native")
set(CPACK_NUGET_PACKAGE_PROJECT_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")
set(CPACK_NUGET_PACKAGE_LICENSE_EXPRESSION "GPL-3.0-or-later")
set(CPACK_NUGET_PACKAGE_REQUIRE_LICENSE_ACCEPTANCE FALSE)

#############################################################################
## Configuration of the source package
#############################################################################

# Set source package name and format
set(CPACK_SOURCE_PACKAGE_FILE_NAME "libleidenalg-${CMAKE_PROJECT_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")

# Declare what to include in the source tarball. Unfortunately we can only
# declare full directories here, not individual files.
set(
    CPACK_SOURCE_INSTALLED_DIRECTORIES
    "${CMAKE_SOURCE_DIR}/etc/cmake;/etc/cmake"
    "${CMAKE_SOURCE_DIR}/include;/include"
    "${CMAKE_SOURCE_DIR}/src;/src"
)

# CPack is pretty dumb as it can only copy full directories (sans the ignored
# files) to the target tarball by default. We need to install also the
# correct version file.
set(CPACK_INSTALL_SCRIPT "${CMAKE_SOURCE_DIR}/etc/cmake/cpack_install_script.cmake")

# Ignore the build and all hidden folders
set(
    CPACK_SOURCE_IGNORE_FILES
    "\\\\..*/"
    "${CMAKE_SOURCE_DIR}/build"
)

#############################################################################
## Now we can include CPack
#############################################################################

include(CPack)
