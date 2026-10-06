elements_subdir(XYDatasetSet)

#===== Libraries ===============================================================
elements_add_library(XYDatasetSet src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel XYDataset
                     PUBLIC_HEADERS XYDatasetSet )
                     
elements_install_python_modules()       
elements_install_aux_files()


elements_add_python_program(PHZ_BuildXYDatasetSet XYDatasetSet.BuildXYDatasetSet)
elements_add_python_program(PHZ_SplitXYDatasetSet XYDatasetSet.SplitXYDatasetSet)
                     
#===== Unit Tests ===============================================================
elements_add_unit_test(MergeProvider_test tests/src/MergeProvider_test.cpp 
                     LINK_LIBRARIES XYDatasetSet TYPE Boost)
                                          
elements_add_unit_test(FileSetProvider_test tests/src/FileSetProvider_test.cpp 
                     LINK_LIBRARIES XYDatasetSet TYPE Boost)
          
          

          
