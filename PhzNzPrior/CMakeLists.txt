elements_subdir(PhzNzPrior)


elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(PhysicsUtils)
elements_depends_on_subdirs(XYDataset)
elements_depends_on_subdirs(PhzModeling)
elements_depends_on_subdirs(PhzDataModel)
elements_depends_on_subdirs(MathUtils)



#===== Libraries ===============================================================
elements_add_library(PhzNzPrior src/lib/*.cpp
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} ElementsKernel PhysicsUtils PhzModeling PhzDataModel XYDataset MathUtils
                  INCLUDE_DIRS ElementsKernel PhysicsUtils PhzModeling PhzDataModel XYDataset MathUtils
                  PUBLIC_HEADERS PhzNzPrior )


#===== Boost tests =============================================================
elements_add_unit_test(SedClassifier_test tests/src/SedClassifier_test.cpp
                       LINK_LIBRARIES PhzNzPrior XYDataset TYPE Boost)

elements_add_unit_test(NzPriorParam_test tests/src/NzPriorParam_test.cpp
                       LINK_LIBRARIES PhzNzPrior TYPE Boost)

elements_add_unit_test(NzPrior_test tests/src/NzPrior_test.cpp
                       LINK_LIBRARIES PhzNzPrior XYDataset TYPE Boost)