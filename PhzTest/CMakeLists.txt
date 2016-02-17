elements_subdir(PhzTest)

elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(PhzConfiguration)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(PhzLikelihood)
elements_depends_on_subdirs(PhzLuminosity)

find_package(Boost COMPONENTS serialization)

elements_add_executable(MagnitudeForComparison src/program/MagnitudeForComparison.cpp
                        LINK_LIBRARIES ElementsKernel Boost PhzConfiguration PhzDataModel 
                        PhzModeling PhzLikelihood PhzLuminosity)
