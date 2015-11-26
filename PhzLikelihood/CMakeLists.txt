elements_subdir(PhzLikelihood)

elements_depends_on_subdirs(PhzDataModel PhzOutput PhzLuminosity PhzUtils)
elements_depends_on_subdirs(ElementsKernel)

find_package(GMock)

if(ELEMENTS_HIDE_WARNINGS)
  if(UNIX)
    set_property(SOURCE tests/src/LikelihoodAlgorithm_test.cpp
                 PROPERTY COMPILE_FLAGS -Wno-float-equal)
  endif()
endif()

#===== Libraries ===============================================================
elements_add_library(PhzLikelihood src/lib/*.cpp
                     LINK_LIBRARIES PhzDataModel PhzOutput PhzLuminosity ElementsKernel PhzUtils
                     PUBLIC_HEADERS PhzLikelihood)

#===== Tests using GMock =======================================================
if(GMOCK_FOUND)
elements_add_unit_test(ScaleFactorFunctor_test tests/src/ScaleFactorFunctor_test.cpp
                     LINK_LIBRARIES GMock PhzLikelihood
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(ChiSquareLikelihoodLogarithm_test tests/src/ChiSquareLikelihoodLogarithm_test.cpp
                     LINK_LIBRARIES GMock PhzLikelihood
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(LikelihoodAlgorithm_test tests/src/LikelihoodAlgorithm_test.cpp
                     LINK_LIBRARIES GMock PhzLikelihood
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(SourcePhzFunctor_test tests/src/SourcePhzFunctor_test.cpp
                     LINK_LIBRARIES GMock PhzLikelihood
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(SumMarginalizationFunctor_test tests/src/SumMarginalizationFunctor_test.cpp
                     LINK_LIBRARIES GMock PhzLikelihood
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(MaxMarginalizationFunctor_test tests/src/MaxMarginalizationFunctor_test.cpp
                     LINK_LIBRARIES GMock PhzLikelihood
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(GroupedAxisCorrection_test tests/src/GroupedAxisCorrection_test.cpp
                     LINK_LIBRARIES PhzLikelihood
                     TYPE Boost)
endif(GMOCK_FOUND)
