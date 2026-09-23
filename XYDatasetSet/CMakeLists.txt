elements_subdir(XYDatasetSet)

#===== Libraries ===============================================================
elements_add_library(XYDatasetSet src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel XYDataset
                     PUBLIC_HEADERS XYDatasetSet )
                     
elements_add_unit_test(MergeProvider_test tests/src/MergeProvider_test.cpp 
                     LINK_LIBRARIES XYDatasetSet
                     TYPE Boost)
          
          
elements_install_python_modules()

elements_add_python_program(PHZ_BuildXYDatasetSet XYDatasetSet.BuildXYDatasetSet)
          
