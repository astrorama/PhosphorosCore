CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(PhzReferenceSample)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel SourceCatalog XYDataset MathUtils NdArray)

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================
find_package(Boost REQUIRED COMPONENTS iostreams)

# Can not compile the PhzReferenceSample module with old versions of boost
if (Boost_VERSION GREATER "105800")

    #===============================================================================
    # Declare the library dependencies here
    # Example:
    #         elements_add_library(ElementsExamples src/Lib/*.cpp
    #                     LINK_LIBRARIES Boost ElementsKernel
    #                     INCLUDE_DIRS Boost ElementsKernel
    #                     PUBLIC_HEADERS ElementsExamples)
    #===============================================================================
    elements_add_library(PhzReferenceSample src/lib/*.cpp
            LINK_LIBRARIES ElementsKernel SourceCatalog XYDataset MathUtils NdArray
            PUBLIC_HEADERS PhzReferenceSample)

    #===============================================================================
    # Declare the executables here
    # Example:
    # elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
    #                        LINK_LIBRARIES Boost ElementsExamples
    #                        INCLUDE_DIRS Boost ElementsExamples)
    #===============================================================================

    #===============================================================================
    # Declare the Boost tests here
    # Example:
    # elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
    #                       EXECUTABLE BoostClassExample_test
    #                       INCLUDE_DIRS ElementsExamples
    #                       LINK_LIBRARIES ElementsExamples TYPE Boost)
    #===============================================================================
    elements_add_unit_test(PhzReferenceSample tests/src/ReferenceSample_test.cpp
            EXECUTABLE ReferenceSample_ReferenceSample_test
            LINK_LIBRARIES PhzReferenceSample
            TYPE Boost)
    elements_add_unit_test(IndexProvider tests/src/IndexProvider_test.cpp
            EXECUTABLE ReferenceSample_IndexProvider_test
            LINK_LIBRARIES PhzReferenceSample
            TYPE Boost)
    elements_add_unit_test(PdzDataProvider tests/src/PdzDataProvider_test.cpp
            EXECUTABLE ReferenceSample_PdzDataProvider_test
            LINK_LIBRARIES PhzReferenceSample
            TYPE Boost)
    elements_add_unit_test(SedDataProvider tests/src/SedDataProvider_test.cpp
            EXECUTABLE ReferenceSample_SedDataProvider_test
            LINK_LIBRARIES PhzReferenceSample
            TYPE Boost)

    #===============================================================================
    # Use the following macro for python modules, scripts and aux files:
    #  elements_install_python_modules()
    #  elements_install_scripts()
    #===============================================================================

    #===============================================================================
    # Declare the Python programs here
    # Examples :
    # elements_add_python_program(PythonProgramExample
    #                             ElementsExamples.PythonProgramExample)
    #===============================================================================

    #===============================================================================
    # Add the elements_install_conf_files macro
    # Examples:
    #          elements_install_conf_files()
    #===============================================================================
    # elements_install_conf_files()

endif ()