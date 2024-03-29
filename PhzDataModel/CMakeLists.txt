elements_subdir(PhzDataModel)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(MathUtils)
elements_depends_on_subdirs(SourceCatalog)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(GridContainer)

find_package(Boost REQUIRED COMPONENTS serialization)

#===== Libraries ===============================================================
elements_add_library(PhzDataModel src/lib/*.cpp src/lib/CatalogAttributes/*.cpp
                     LINK_LIBRARIES ${CMAKE_DL_LIBS} Boost ElementsKernel MathUtils XYDataset SourceCatalog GridContainer
                     INCLUDE_DIRS Boost ElementsKernel MathUtils XYDataset SourceCatalog
                     PUBLIC_HEADERS PhzDataModel)


#===== Boost tests =============================================================                     
elements_add_unit_test(FilterInfo_test tests/src/FilterInfo_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(PhotometricCorrectionMap_test tests/src/PhotometricCorrectionMap_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(AdjustErrorParamMap_test tests/src/AdjustErrorParamMap_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(PhotometryCellManagerTraits_test tests/src/PhotometryGrid_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(PhzModel_test tests/src/PhzModel_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(PhotometryGridSerialization_test tests/src/serialization/PhotometryGrid_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(QualifiedNameSerialization_test tests/src/serialization/QualifiedName_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(QualifiedNameGroupManager_test tests/src/QualifiedNameGroupManager_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(SourceResults_test tests/src/SourceResults_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)  
elements_add_unit_test(PPConfig_test tests/src/PPConfig_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(FixedRedshift_test tests/src/CatalogAttributes/FixedRedshift_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(ObservationCondition_test tests/src/CatalogAttributes/ObservationCondition_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
elements_add_unit_test(Sed_test tests/src/Sed_test.cpp 
                     LINK_LIBRARIES PhzDataModel
                     TYPE Boost)
