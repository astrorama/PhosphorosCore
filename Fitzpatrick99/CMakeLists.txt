# Declare Fitzpatrick99 as an Elements module
elements_subdir(Fitzpatrick99)

# Declare Elements module dependencies
#    This module is using of the ElementsKernel module
elements_depends_on_subdirs(ElementsKernel)



#---Python modules, Scripts, conf and aux files ------------------------------------------------------------



elements_install_python_modules()




#===== Python programs =========================================================
elements_add_python_program(F99 Fitzpatrick99.F99Program)
