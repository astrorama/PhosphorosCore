elements_subdir(PhzExecutables)

elements_depends_on_subdirs(ElementsKernel)
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
elements_add_library(PhzExecutables src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel PhzConfiguration PhzDataModel PhzModeling PhzLikelihood PhzOutput PhzPhotometricCorrection PhzReferenceSample
                     PUBLIC_HEADERS PhzExecutables)


find_package(Boost REQUIRED COMPONENTS program_options)
elements_add_executable(PhosphorosComputeModelGrid src/program/ComputeModelGrid.cpp
                     LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling)
elements_add_executable(PhosphorosComputeRedshifts src/program/ComputeRedshifts.cpp
                     LINK_LIBRARIES ElementsKernel Boost PhzExecutables)
elements_add_executable(PhosphorosComputePhotometricCorrections src/program/ComputePhotometricCorrections.cpp
                     LINK_LIBRARIES ElementsKernel Boost PhzExecutables)
elements_add_executable(PhosphorosComputeModelSed src/program/ComputeModelSed.cpp
                     LINK_LIBRARIES ElementsKernel Boost MathUtils PhzModeling PhzExecutables)
elements_add_executable(PhosphorosBuildReferenceSample src/program/BuildReferenceSample.cpp
                     LINK_LIBRARIES ElementsKernel PhzExecutables)
elements_add_executable(PhosphorosComputeGalacticAbsorptionCoefficientGrid src/program/ComputeGalacticAbsorptionCoefficientGrid.cpp
                     LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling PhzGalacticCorrection)
elements_add_executable(PhzModelGrid2Fits src/program/PhzModelGrid2Fits.cpp
                     LINK_LIBRARIES ElementsKernel PhzExecutables)
elements_add_executable(PhosphorosComputeSedWeight src/program/ComputeSedWeight.cpp
                     LINK_LIBRARIES ElementsKernel PhzExecutables)
                     
                     
                     
elements_add_unit_test(ComputeSedWeight_test tests/src/ComputeSedWeight_test.cpp 
                     LINK_LIBRARIES PhzExecutables
                     TYPE Boost)

elements_install_conf_files()
