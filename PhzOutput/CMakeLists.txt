elements_subdir(PhzOutput)

elements_depends_on_subdirs(PhzUtils PhzDataModel SourceCatalog GridContainer ElementsKernel Table)
elements_depends_on_subdirs(ElementsKernel)
find_package(CCfits)

#===== Libraries ===============================================================
elements_add_library(PhzOutput src/lib/*.cpp src/lib/PhzColumnHandlers/*.cpp
                     LINK_LIBRARIES ElementsKernel Table SourceCatalog PhzDataModel PhzUtils CCfits Table
                     INCLUDE_DIRS CCfits
                     PUBLIC_HEADERS PhzOutput)
elements_add_unit_test(MultithreadHandler_test tests/src/MultithreadHandler_test.cpp 
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
