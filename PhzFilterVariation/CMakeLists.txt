elements_subdir(PhzFilterVariation)

elements_depends_on_subdirs(PhzModeling PhzDataModel PhzOutput PhzLuminosity PhzUtils)
elements_depends_on_subdirs(PhysicsUtils)
elements_depends_on_subdirs(ElementsKernel)

find_package(GMock)

#===== Libraries ===============================================================
elements_add_library(PhzFilterVariation src/lib/*.cpp
                     LINK_LIBRARIES PhzDataModel PhzModeling PhzOutput PhzLuminosity ElementsKernel PhzUtils PhysicsUtils
                     PUBLIC_HEADERS PhzFilterVariation)


elements_add_unit_test(FilterVariationSingleGridCreator_test tests/src/FilterVariationSingleGridCreator_test.cpp
                       LINK_LIBRARIES PhzDataModel PhzModeling PhzOutput PhzLuminosity ElementsKernel PhzUtils PhysicsUtils PhzFilterVariation  TYPE Boost)
                       
#===== Tests using GMock =======================================================
if(GMOCK_FOUND)

endif(GMOCK_FOUND)
