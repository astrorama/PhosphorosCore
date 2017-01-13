elements_subdir(PhzExecutables)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzOutput)
elements_depends_on_subdirs(PhzPhotometricCorrection)

find_package(Boost REQUIRED COMPONENTS program_options)

elements_add_executable(PhosphorosBuildTemplates src/program/BuildTemplates.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling)

elements_add_executable(PhosphorosFitTemplates src/program/FitTemplates.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel PhzModeling PhzLikelihood PhzOutput)

elements_add_executable(PhosphorosDeriveZeroPoints src/program/DeriveZeroPoints.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzPhotometricCorrection)
                        
elements_install_conf_files()