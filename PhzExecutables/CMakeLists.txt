elements_subdir(PhzExecutables)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzOutput)
elements_depends_on_subdirs(PhzPhotometricCorrection)

elements_add_library(PhzExecutables src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel PhzConfiguration PhzDataModel PhzModeling PhzLikelihood PhzOutput
                     PUBLIC_HEADERS PhzExecutables)

find_package(Boost REQUIRED COMPONENTS program_options)
elements_add_executable(PhosphorosComputeModelGrid src/program/ComputeModelGrid.cpp
                     LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling)
elements_add_executable(PhosphorosComputeLuminosityModelGrid src/program/ComputeLuminosityModelGrid.cpp
                     LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling)
elements_add_executable(PhosphorosComputeRedshifts src/program/ComputeRedshifts.cpp
                     LINK_LIBRARIES ElementsKernel Boost PhzExecutables)
elements_add_executable(PhosphorosComputePhotometricCorrections src/program/ComputePhotometricCorrections.cpp
                     LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzPhotometricCorrection)

elements_install_conf_files()