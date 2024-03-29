elements_subdir(PhzLikelihood)

elements_depends_on_subdirs(MathUtils PhzDataModel PhzModeling PhzOutput PhzLuminosity PhzUtils)
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
                     LINK_LIBRARIES PhzDataModel MathUtils PhzModeling PhzOutput PhzLuminosity ElementsKernel PhzUtils
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
elements_add_unit_test(SharedPriorAdapter_test tests/src/SharedPriorAdapter_test.cpp 
                     LINK_LIBRARIES SourceCatalog PhzDataModel PhzLikelihood XYDataset GMock
                     INCLUDE_DIRS GMock
                     TYPE Boost)
elements_add_unit_test(VolumePrior_test tests/src/VolumePrior_test.cpp 
                     LINK_LIBRARIES PhzLikelihood
                     TYPE Boost)
elements_add_unit_test(AxisFunctionPrior_test tests/src/AxisFunctionPrior_test.cpp 
                     LINK_LIBRARIES PhzLikelihood
                     TYPE Boost)
elements_add_unit_test(AxisWeightPrior_test tests/src/AxisWeightPrior_test.cpp 
                     LINK_LIBRARIES PhzLikelihood
                     TYPE Boost)
elements_add_unit_test(GenericGridPrior_test tests/src/GenericGridPrior_test.cpp 
                     LINK_LIBRARIES PhzLikelihood
                     TYPE Boost)
endif(GMOCK_FOUND)  

elements_add_unit_test(SigmaScaleFactorFunctor_test tests/src/SigmaScaleFactorFunctor_test.cpp 
                     LINK_LIBRARIES PhzLikelihood
                     TYPE Boost)   
                
elements_add_unit_test(FixRedshiftProcessModelGridFunctor_test tests/src/FixRedshiftProcessModelGridFunctor_test.cpp 
                     LINK_LIBRARIES PhzLikelihood
                     TYPE Boost)
                     
elements_add_unit_test(GalacticAbsorptionProcessModelGridFunctor_test tests/src/GalacticAbsorptionProcessModelGridFunctor_test.cpp 
                     LINK_LIBRARIES PhzLikelihood
                     TYPE Boost)

elements_add_unit_test(FilterShiftProcessModelGridFunctor_test tests/src/FilterShiftProcessModelGridFunctor_test.cpp
                     LINK_LIBRARIES PhzLikelihood
                     TYPE Boost)

                     


