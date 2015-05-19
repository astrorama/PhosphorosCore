elements_subdir(PhzOutput)

elements_depends_on_subdirs(PhzUtils PhzDataModel SourceCatalog GridContainer ElementsKernel Table)
find_package(Cfitsio)
find_package(CCfits)

#===== Libraries ===============================================================
elements_add_library(PhzOutput src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel Table SourceCatalog PhzDataModel PhzUtils Cfitsio CCfits Table
                     INCLUDE_DIRS Cfitsio CCfits
                     PUBLIC_HEADERS PhzOutput )