elements_subdir(PhzOutput)

elements_depends_on_subdirs(PhzUtils PhzDataModel SourceCatalog GridContainer ElementsKernel Table MathUtils NdArray)
elements_depends_on_subdirs(ElementsKernel)
find_package(CCfits)

#===== Libraries ===============================================================
elements_add_library(PhzOutput src/lib/*.cpp src/lib/PhzColumnHandlers/*.cpp
                     LINK_LIBRARIES ElementsKernel Table SourceCatalog PhzDataModel PhzUtils CCfits MathUtils NdArray
                     INCLUDE_DIRS CCfits
                     PUBLIC_HEADERS PhzOutput)

elements_add_unit_test(BestModel_test tests/src/BestModel_test.cpp
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
elements_add_unit_test(BestModelOnlyZ_test tests/src/BestModelOnlyZ_test.cpp
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
elements_add_unit_test(CopyColumns_test tests/src/CopyColumns_test.cpp
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
elements_add_unit_test(MultithreadHandler_test tests/src/MultithreadHandler_test.cpp 
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
elements_add_unit_test(GridSampler_test tests/src/GridSampler_test.cpp 
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
elements_add_unit_test(GridSamplerScale_test tests/src/GridSamplerScale_test.cpp 
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
elements_add_unit_test(ColumnHandlers_Id_test tests/src/ColumnHandlers_Id_test.cpp 
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
elements_add_unit_test(ColumnHandlers_Flag_test tests/src/ColumnHandlers_Flag_test.cpp 
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
elements_add_unit_test(CorrectedPhotometry_test tests/src/CorrectedPhotometry_test.cpp 
                     LINK_LIBRARIES PhzOutput
                     TYPE Boost)
                     
                     