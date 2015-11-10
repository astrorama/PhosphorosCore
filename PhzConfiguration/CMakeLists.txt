elements_subdir(PhzConfiguration)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzOutput)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzPhotometricCorrection)
elements_depends_on_subdirs(PhzUtils)
elements_depends_on_subdirs(PhzLuminosity)
elements_depends_on_subdirs(Configuration)


if(ELEMENTS_HIDE_WARNINGS)
  if(UNIX)
    set_property(SOURCE src/lib/CheckLuminosityParameter.cpp
                 PROPERTY COMPILE_FLAGS -Wno-float-equal)
  endif()
endif()
elements_add_library(PhzConfiguration src/lib/*.cpp
                     LINK_LIBRARIES XYDataset PhzDataModel PhzOutput PhzPhotometricCorrection PhzLikelihood PhzModeling PhzUtils PhzLuminosity ElementsKernel Configuration
                     INCLUDE_DIRS XYDataset PhzDataModel PhzUtils PhzLuminosity
                     PUBLIC_HEADERS PhzConfiguration)
elements_add_unit_test(FilterConfiguration_test tests/src/FilterConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(SedConfiguration_test tests/src/SedConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(RedshiftConfiguration_test tests/src/RedshiftConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ReddeningConfiguration_test tests/src/ReddeningConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ParameterSpaceConfiguration_test tests/src/ParameterSpaceConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ComputeModelGridConfiguration_test tests/src/ComputeModelGridConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration PhzUtils
                     TYPE Boost)
elements_add_unit_test(ComputeRedshiftsConfiguration_test tests/src/ComputeRedshiftsConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration PhzUtils
                     TYPE Boost)
elements_add_unit_test(PhotometryGridConfiguration_test tests/src/PhotometryGridConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(CatalogConfiguration_test tests/src/CatalogConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(SpectroscopicRedshiftCatalogConfiguration_test tests/src/SpectroscopicRedshiftCatalogConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(ComputePhotometricCorrectionsConfiguration_test tests/src/ComputePhotometricCorrectionsConfiguration_test.cpp 
                     LINK_LIBRARIES PhzDataModel PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(PhotometricCorrectionConfiguration_test tests/src/PhotometricCorrectionConfiguration_test.cpp 
                     LINK_LIBRARIES XYDataset PhzDataModel PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(PhosphorosPathConfiguration_test tests/src/PhosphorosPathConfiguration_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(CatalogTypeConfiguration_test tests/src/CatalogTypeConfiguration_test.cpp 
                     LINK_LIBRARIES PhzConfiguration
                     TYPE Boost)
elements_add_unit_test(LuminosityTypeConfiguration_test tests/src/LuminosityTypeConfiguration_test.cpp 
                     LINK_LIBRARIES PhzConfiguration PhzLuminosity
                     TYPE Boost)
elements_add_unit_test(LuminositySedGroupConfiguration_test tests/src/LuminositySedGroupConfiguration_test.cpp 
                     LINK_LIBRARIES PhzConfiguration PhzLuminosity
                     TYPE Boost)
elements_add_unit_test(LuminosityFunctionConfiguration_test tests/src/LuminosityFunctionConfiguration_test.cpp 
                     LINK_LIBRARIES PhzConfiguration PhzLuminosity
                     TYPE Boost)
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
