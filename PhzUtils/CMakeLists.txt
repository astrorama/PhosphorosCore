elements_subdir(PhzUtils)

#===== Libraries ===============================================================
elements_add_library(PhzUtils src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel 
                     PUBLIC_HEADERS PhzUtils )
          
elements_add_unit_test(FileUtils_test tests/src/FileUtils_test.cpp
                       LINK_LIBRARIES PhzUtils TYPE Boost)
          