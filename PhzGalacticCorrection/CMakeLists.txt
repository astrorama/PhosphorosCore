elements_subdir(PhzGalacticCorrection)

elements_depends_on_subdirs(PhzModeling PhzDataModel PhzOutput PhzLuminosity PhzUtils)
elements_depends_on_subdirs(ElementsKernel)

find_package(GMock)

#===== Libraries ===============================================================
elements_add_library(PhzGalacticCorrection src/lib/*.cpp
                     LINK_LIBRARIES PhzDataModel PhzModeling PhzOutput PhzLuminosity ElementsKernel PhzUtils
                     PUBLIC_HEADERS PhzGalacticCorrection)

#===== Tests using GMock =======================================================
if(GMOCK_FOUND)

endif(GMOCK_FOUND)