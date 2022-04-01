elements_subdir(PhzConfiguration)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(MathUtils)
elements_depends_on_subdirs(PhysicsUtils)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzOutput)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzPhotometricCorrection)
elements_depends_on_subdirs(PhzUtils)
elements_depends_on_subdirs(PhzLuminosity)
elements_depends_on_subdirs(PhzNzPrior)
elements_depends_on_subdirs(PhzReferenceSample)
elements_depends_on_subdirs(Configuration)


if(ELEMENTS_HIDE_WARNINGS)
  if(UNIX)
    set_property(SOURCE src/lib/CheckLuminosityParameter.cpp
                 PROPERTY COMPILE_FLAGS -Wno-float-equal)
  endif()
endif()
elements_add_library(PhzConfiguration src/lib/*.cpp
        LINK_LIBRARIES
          XYDataset PhzDataModel PhzOutput PhzPhotometricCorrection PhzNzPrior PhzLikelihood
          PhzModeling PhzUtils PhzLuminosity ElementsKernel Configuration PhzReferenceSample
        INCLUDE_DIRS XYDataset MathUtils PhzDataModel PhzUtils PhzLuminosity PhzNzPrior
        PUBLIC_HEADERS PhzConfiguration)
elements_add_unit_test(CatalogTypeConfig_test tests/src/CatalogTypeConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(PhosphorosRootDirConfig_test tests/src/PhosphorosRootDirConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(AuxDataDirConfig_test tests/src/AuxDataDirConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(IntermediateDirConfig_test tests/src/IntermediateDirConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ResultsDirConfig_test tests/src/ResultsDirConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(SedProviderConfig_test tests/src/SedProviderConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(SedConfig_test tests/src/SedConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(RedshiftConfig_test tests/src/RedshiftConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(IgmConfig_test tests/src/IgmConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzModeling PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(FilterProviderConfig_test tests/src/FilterProviderConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(FilterVariationConfig_test tests/src/FilterVariationConfig_test.cpp
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(CatalogDirConfig_test tests/src/CatalogDirConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ReddeningProviderConfig_test tests/src/ReddeningProviderConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ReddeningConfig_test tests/src/ReddeningConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ParameterSpaceConfig_test tests/src/ParameterSpaceConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzModeling PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(PhotometryGridConfig_test tests/src/PhotometryGridConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(PhotometricCorrectionConfig_test tests/src/PhotometricCorrectionConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ErrorAdjustmentConfig_test tests/src/ErrorAdjustmentConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(PhosphorosCatalogConfig_test tests/src/PhosphorosCatalogConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(FilterConfig_test tests/src/FilterConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzModeling PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ModelGridOutputConfig_test tests/src/ModelGridOutputConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzModeling PhzUtils PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ComputeModelGridConfig_test tests/src/ComputeModelGridConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzModeling PhzUtils PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(PriorConfig_test tests/src/PriorConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(LikelihoodGridFuncConfig_test tests/src/LikelihoodGridFuncConfig_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(LuminosityFunctionConfig_test tests/src/LuminosityFunctionConfig_test.cpp 
                     LINK_LIBRARIES XYDataset MathUtils PhzDataModel PhzLuminosity PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(LuminositySedGroupConfig_test tests/src/LuminositySedGroupConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(MarginalizationConfig_test tests/src/MarginalizationConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(CosmologicalParameterConfig_test tests/src/CosmologicalParameterConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration PhysicsUtils
                     TYPE Boost)
elements_add_unit_test(VolumePriorConfig_test tests/src/VolumePriorConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(MultithreadConfig_test tests/src/MultithreadConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(AxisFunctionPriorConfig_test tests/src/AxisFunctionPriorConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(AxisFunctionPriorProviderConfig_test tests/src/AxisFunctionPriorProviderConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(AxisWeightPriorConfig_test tests/src/AxisWeightPriorConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(GenericGridPriorConfig_test tests/src/GenericGridPriorConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(PdfOutputFlagsConfig_test tests/src/PdfOutputFlagsConfig_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ModelNormalizationConfig_test tests/src/ModelNormalizationConfig_test.cpp
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)     
elements_add_unit_test(ComputeReferenceSampleConfig tests/src/BuildReferenceSampleConfig_test.cpp
                     EXECUTABLE PhzConfiguration_ComputeReferenceSampleConfig_test
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(RedshiftFunctorConfig_test tests/src/RedshiftFunctorConfig_test.cpp
                     EXECUTABLE PhzConfiguration_RedshiftFunctorConfig_test
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)     
elements_add_unit_test(ScaleFactorMarginalizationConfig_test tests/src/ScaleFactorMarginalizationConfig_test.cpp
                     EXECUTABLE PhzConfiguration_ScaleFactorMarginalizationConfig_test
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)     
elements_add_unit_test(BuildPPConfigConfig_test tests/src/BuildPPConfigConfig_test.cpp
                     EXECUTABLE PhzConfiguration_BuildPPConfigConfig_test
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)   
elements_add_unit_test(PhysicalParametersConfig_test tests/src/PhysicalParametersConfig_test.cpp
                     EXECUTABLE PhzConfiguration_PhysicalParametersConfig_test
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)                                  

# Install auxiliary files
elements_install_aux_files()
