elements_subdir(PhzExecutables)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(KdTree)
elements_depends_on_subdirs(Table)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(MathUtils)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzOutput)
elements_depends_on_subdirs(PhzPhotometricCorrection)
elements_depends_on_subdirs(PhzReferenceSample)
elements_depends_on_subdirs(PhzGalacticCorrection)
elements_depends_on_subdirs(PhzFilterVariation)

file(GLOB BIN_SRC src/lib/*.cpp)

if (NOT TARGET PhzReferenceSample)
    file(GLOB REF_BIN_SRC src/lib/BuildReferenceSample.cpp)
    list(REMOVE_ITEM BIN_SRC ${REF_BIN_SRC})
endif ()

elements_add_library(PhzExecutables ${BIN_SRC}
        LINK_LIBRARIES
        ElementsKernel PhzConfiguration PhzDataModel PhzModeling PhzLikelihood
        PhzOutput PhzPhotometricCorrection
        PhzGalacticCorrection PhzFilterVariation
        PUBLIC_HEADERS PhzExecutables)

find_package(Boost REQUIRED COMPONENTS program_options)

elements_add_executable(PhosphorosComputeModelGrid src/program/ComputeModelGrid.cpp
        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling PhzExecutables)
elements_add_executable(PhosphorosComputeRedshifts src/program/ComputeRedshifts.cpp
        LINK_LIBRARIES ElementsKernel Boost PhzExecutables)
elements_add_executable(PhosphorosComputePhotometricCorrections src/program/ComputePhotometricCorrections.cpp
        LINK_LIBRARIES ElementsKernel Boost PhzExecutables)
elements_add_executable(PhosphorosComputeModelSed src/program/ComputeModelSed.cpp
        LINK_LIBRARIES ElementsKernel Boost MathUtils PhzModeling PhzExecutables)
elements_add_executable(PhosphorosComputeGalacticAbsorptionCoefficientGrid src/program/ComputeGalacticAbsorptionCoefficientGrid.cpp
        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling PhzGalacticCorrection PhzExecutables)
elements_add_executable(PhosphorosComputeFilterVariationCoefficientGrid src/program/ComputeFilterVariationCoefficientGrid.cpp
        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling PhzFilterVariation)
elements_add_executable(PhzModelGrid2Fits src/program/PhzModelGrid2Fits.cpp
        LINK_LIBRARIES ElementsKernel PhzExecutables)
elements_add_executable(PhosphorosComputeSedWeight src/program/ComputeSedWeight.cpp
        LINK_LIBRARIES ElementsKernel PhzExecutables)
elements_add_executable(PhosphorosBuildPPConfig src/program/BuildPPConfig.cpp
        LINK_LIBRARIES ElementsKernel PhzExecutables)

if (TARGET PhzReferenceSample)
    elements_add_executable(PhosphorosBuildReferenceSample src/program/BuildReferenceSample.cpp
            LINK_LIBRARIES ElementsKernel PhzExecutables)
    elements_add_executable(PhosphorosBuildPhotometry src/program/BuildPhotometry.cpp
            LINK_LIBRARIES PhzExecutables)
endif ()

elements_add_unit_test(ComputeSedWeight_test tests/src/ComputeSedWeight_test.cpp
        LINK_LIBRARIES PhzExecutables
        TYPE Boost)
elements_add_unit_test(BuildPPConfig_test tests/src/BuildPPConfig_test.cpp
        LINK_LIBRARIES PhzExecutables
        TYPE Boost)

#===============================================================================
# Use the following macro for python modules, scripts and aux files:
#  elements_install_python_modules()
#  elements_install_scripts()
#===============================================================================
elements_install_python_modules()

#===============================================================================
# Declare the Python programs here
# Examples :
# elements_add_python_program(PythonProgramExample
#                             ElementsExamples.PythonProgramExample)
#===============================================================================
elements_add_python_program(PhosphorosExtractPpPdf PhzExecutables.ExtractPpPdf)
elements_add_python_program(PhosphorosExtractZList PhzExecutables.ExtractZList)

elements_install_conf_files()
