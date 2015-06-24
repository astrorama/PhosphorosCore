elements_subdir(PhzOutput)

elements_depends_on_subdirs(PhzUtils PhzDataModel SourceCatalog GridContainer ElementsKernel Table)
find_package(CCfits)

#===== Libraries ===============================================================
elements_add_library(PhzOutput src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel Table SourceCatalog PhzDataModel PhzUtils CCfits Table
                     INCLUDE_DIRS CCfits
                     PUBLIC_HEADERS PhzOutput )