elements_subdir(PhzLuminosity 0.1)


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
elements_add_unit_test(LuminosityComputationFunctor_test tests/src/LuminosityComputationFunctor_test.cpp
                       LINK_LIBRARIES XYDataset PhzLuminosity TYPE Boost)
                       
elements_add_unit_test(LuminosityFunctionValidityDomain_Test tests/src/LuminosityFunctionValidityDomain_Test.cpp
                       LINK_LIBRARIES XYDataset PhzLuminosity TYPE Boost)

elements_add_unit_test(SchechterLuminosityFunction_test tests/src/SchechterLuminosityFunction_test.cpp
                       LINK_LIBRARIES XYDataset PhzLuminosity TYPE Boost)
                       
elements_add_unit_test(CustomLuminosityFunction_test tests/src/CustomLuminosityFunction_test.cpp
                       LINK_LIBRARIES MathUtils XYDataset PhzLuminosity TYPE Boost)
                       
elements_add_unit_test(CompositeLuminosityFunction_test tests/src/CompositeLuminosityFunction_test.cpp
                       LINK_LIBRARIES MathUtils XYDataset PhzLuminosity TYPE Boost)
