elements_subdir(PhzLuminosity)


elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(PhysicsUtils)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(PhzDataModel)


#===== Libraries ===============================================================
elements_add_library(PhzLuminosity src/lib/*.cpp
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} ElementsKernel PhysicsUtils PhzDataModel XYDataset
                  INCLUDE_DIRS ElementsKernel PhysicsUtils PhzDataModel XYDataset
                  PUBLIC_HEADERS PhzLuminosity )

#===== Boost tests =============================================================
elements_add_unit_test(LuminosityFunctionValidityDomain_Test tests/src/LuminosityFunctionValidityDomain_Test.cpp
                       LINK_LIBRARIES XYDataset PhzLuminosity TYPE Boost)
                       
elements_add_unit_test(LuminosityFunctionSet_test tests/src/LuminosityFunctionSet_test.cpp
                       LINK_LIBRARIES  MathUtils XYDataset PhzLuminosity TYPE Boost)

elements_add_unit_test(SchechterLuminosityFunction_test tests/src/SchechterLuminosityFunction_test.cpp
                       LINK_LIBRARIES XYDataset PhzLuminosity TYPE Boost)

elements_add_unit_test(LuminosityCalculator_test tests/src/LuminosityCalculator_test.cpp
                      LINK_LIBRARIES XYDataset PhzLuminosity TYPE Boost)

elements_add_unit_test(ReddenedLuminosityCalculator_test tests/src/ReddenedLuminosityCalculator_test.cpp
                      LINK_LIBRARIES XYDataset PhzLuminosity TYPE Boost)

elements_add_unit_test(UnreddenedLuminosityCalculator_test tests/src/UnreddenedLuminosityCalculator_test.cpp
                      LINK_LIBRARIES XYDataset PhzLuminosity TYPE Boost)



