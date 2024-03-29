elements_subdir(PhzPhotometricCorrection)

elements_depends_on_subdirs(SourceCatalog)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzUtils)


find_package(GMock)

if(ELEMENTS_HIDE_WARNINGS)
  if(UNIX)
    set_property(SOURCE tests/src/PhotometricCorrectionCalculator_test.cpp
                 PROPERTY COMPILE_FLAGS -Wno-float-equal)
  endif()
endif()

#===== Libraries ===============================================================
elements_add_library(PhzPhotometricCorrection src/lib/*.cpp
                  LINK_LIBRARIES PhzDataModel SourceCatalog PhzLikelihood
                  PUBLIC_HEADERS PhzPhotometricCorrection )

#===== Boost tests =============================================================

elements_add_unit_test(PhotometricCorrectionAlgorithm_test tests/src/PhotometricCorrectionAlgorithm_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(DefaultStopCriteria_test tests/src/DefaultStopCriteria_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)

elements_add_unit_test(FindMedianPhotometricCorrectionsFunctor_test tests/src/FindMedianPhotometricCorrectionsFunctor_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(FindWeightedMedianPhotometricCorrectionsFunctor_test tests/src/FindWeightedMedianPhotometricCorrectionsFunctor_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(FindMeanPhotometricCorrectionsFunctor_test tests/src/FindMeanPhotometricCorrectionsFunctor_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(FindWeightedMeanPhotometricCorrectionsFunctor_test tests/src/FindWeightedMeanPhotometricCorrectionsFunctor_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)
elements_add_unit_test(ParallelSourceHandler_test tests/src/ParallelSourceHandler_test.cpp
                       LINK_LIBRARIES PhzPhotometricCorrection TYPE Boost)

#===== Tests using GMock =======================================================
if(GMOCK_FOUND)
  elements_add_unit_test(CalculateScaleFactorMap_test tests/src/CalculateScaleFactorMap_test.cpp
                         LINK_LIBRARIES GMock PhzPhotometricCorrection TYPE Boost
                         INCLUDE_DIRS GMock)
  elements_add_unit_test(FindBestFitModels_test tests/src/FindBestFitModels_test.cpp
                         LINK_LIBRARIES GMock PhzPhotometricCorrection TYPE Boost
                         INCLUDE_DIRS GMock)
  elements_add_unit_test(PhotometricCorrectionCalculator_test tests/src/PhotometricCorrectionCalculator_test.cpp
                         LINK_LIBRARIES GMock PhzPhotometricCorrection TYPE Boost
                         INCLUDE_DIRS GMock)
endif(GMOCK_FOUND)
