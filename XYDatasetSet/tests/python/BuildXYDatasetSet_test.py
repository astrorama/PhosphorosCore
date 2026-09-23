#
# Copyright (C) 2012-2020 Euclid Science Ground Segment
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#

"""
File: tests/python/BuildXYDatasetSet_test.py.py

Created on: 2026-09-22
Author: fdubath
"""

import sys

import pytest
import math
import json
import numpy as np
import os
from ElementsKernel.Temporary import TempFile
from ElementsKernel.Temporary import TempDir
from astropy.table import Table
from astropy.io import fits

import XYDatasetSet.BuildXYDatasetSet as handler



class TestBuildXYDatasetSet(object):
    """
    @class TestBuildXYDatasetSet(
    @brief Unit Test class
    """
    
    def test_defineSpecificProgramOptions(self):
        parser = handler.defineSpecificProgramOptions()

        with TempFile() as three:
            test_path = three.path()
            ofs = open(test_path, "w")
            parser.print_help(ofs)
            ofs.close()

            help_file = open(test_path, 'r')
            help_txt =  help_file.read()
            help_file.close()
            assert '--input-dir' in help_txt
            assert '--resample' in help_txt
            assert '--strip-parameter' in help_txt
            assert '--output-file' in help_txt

    def test_readXYDataSet(self):
        with TempFile() as three:
            test_path = three.path()
            ofs = open(test_path, "w")
            
            ofs.write("# NAME : Calzetti\n")
            ofs.write("# PARAMETER : METALLICITY=0*L+0.008[M/H] \n")
            ofs.write("# PARAMETER : TAU=0*L+0.3[Gyr] \n")
            ofs.write("# COMMENT : WAVELENGTH UNIT=Angstrom \n")
            ofs.write("# COMMENT : FLUX UNIT=erg/s/cm^2/A @ 10pc \n")
            ofs.write("#\n")
            ofs.write("91.0 0.0 \n")
            ofs.write("100.0 1.0\n")
            ofs.write("140.0 9.0\n")
            ofs.write("170.0 2.0\n")
            ofs.write("210.0 3.0\n")
            ofs.write("230.0 1.0\n")
            ofs.close()
            
            sampling, values, params, name = handler.readXYDataSet(test_path)
            
        expected_sampling = [91.0,100.0,140.0,170.0,210.0,230.0]
        expected_values = [0.0,1.0,9.0,2.0,3.0,1.0]
        expected_param = [["NAME","Calzetti"],["PARAMETER","METALLICITY=0*L+0.008[M/H] "],["PARAMETER","TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "],["COMMENT","FLUX UNIT=erg/s/cm^2/A @ 10pc "]]
        expected_name = test_path.split("/")[-1].split('.')[0]
        print(expected_name)
        assert np.array_equal(sampling, expected_sampling)
        assert np.array_equal(values, expected_values)
        assert np.array_equal(params, expected_param)
        assert name == expected_name

    def test_listAndOrderFiles(self):
        # Nominal
        names = ["Z.txt","A.txt","C.txt"]
        with TempDir() as three:
            dir_path = three.path()
            order = open(os.path.join(dir_path, "order.txt"), "w")
            for name in names: 
                order.write(f"{name}\n")
            order.close()

            for idx in range(len(names)): 
                fl = open(os.path.join(dir_path, names[len(names)-1-idx]), "w")
                fl.write("empty!")
                fl.close()
                
            ordered = handler.listAndOrderFiles(dir_path)  
            assert len(ordered)==3
            for idx in range(len(names)):
                assert ordered[idx]==names[idx]
        
        # Too few files
        with TempDir() as three:
            dir_path = three.path()
            order = open(os.path.join(dir_path, "order.txt"), "w")
            for name in names: 
                order.write(f"{name}\n")
            order.close()

            for name in names : 
                if "A" not in name:
                    fl = open(os.path.join(dir_path, name), "w")
                    fl.write("empty!")
                    fl.close()
                    
            ordered = handler.listAndOrderFiles(dir_path)  
            assert len(ordered)==2
            ordered[0]=="Z.txt"
            ordered[1]=="C.txt"
            
        # Too many files
        with TempDir() as three:
            dir_path = three.path()
            order = open(os.path.join(dir_path, "order.txt"), "w")
            for name in names: 
                order.write(f"{name}\n")
            order.close()
            fl = open(os.path.join(dir_path, "B.txt"), "w")
            fl.write("empty!")
            fl.close()
            for name in names : 
                fl = open(os.path.join(dir_path, name), "w")
                fl.write("empty!")
                fl.close()

                
            ordered = handler.listAndOrderFiles(dir_path)  
            assert len(ordered)==4
            ordered[0]=="Z.txt"
            ordered[1]=="A.txt"
            ordered[2]=="C.txt"
            ordered[3]=="B.txt"
            
    def test_checkSampling(self):
        # No missmatch
        samplings = [[1,2,3,4,5],[1,2,3,4,5]]
        values=     [[1,2,3,4,5],[5,4,3,2,1]]
        resample = False
        
        newsamplings, newvalues = handler.checkSampling(samplings, values, resample)
        assert np.array_equal(samplings, newsamplings)
        assert np.array_equal(values, newvalues)
        
        resample = True
        
        newsamplings, newvalues = handler.checkSampling(samplings, values, resample)
        assert np.array_equal(samplings, newsamplings)
        assert np.array_equal(values, newvalues)
        
        # Missmatch no resampling
        samplings = [[1,2,3,4,5],[1,2,4,5]]
        values=     [[1,2,3,4,5],[5,4,2,1]]
        resample = False
        raised = False
        try:
            handler.checkSampling(samplings, values, resample)
        except Exception:
            raised = True
        assert raised
        
        # Missmatch resampling
        resample = True
        newsamplings, newvalues = handler.checkSampling(samplings, values, resample)
        assert np.array_equal(newsamplings[0], samplings[0])
        assert np.array_equal(newsamplings[0], newsamplings[1])
        assert np.array_equal(values[0], newvalues[0])
        assert np.array_equal([5,4,3,2,1], newvalues[1])
        
        # Missmatch resampling border case 
        samplings = [[2,3,4,5],[0,1,2,3,4]]
        values=     [[2,3,4,5],[4,3,2,1,0]]
        resample = True
        newsamplings, newvalues = handler.checkSampling(samplings, values, resample)
        assert np.array_equal(newsamplings[0], [0,1,2,3,4,5])
        assert np.array_equal([0,0,2,3,4,5], newvalues[0])
        assert np.array_equal([4,3,2,1,0,0], newvalues[1])
        
    def test_getParameters(self):
        params = [["NAME","Calzetti"],["PARAMETER","METALLICITY=0*L+0.008[M/H] "],["PARAMETER","METALLICITY=0*L+0.008[M/H] "],["PARAMETER","TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "],["COMMENT","FLUX UNIT=erg/s/cm^2/A @ 10pc "]]
        parameter_list = handler.getParameters(params)
        assert len(parameter_list) == 2
        assert "METALLICITY" in parameter_list
        assert "TAU" in parameter_list
    
    def test_cleanParameters(self):
        params = [["NAME","Calzetti"],["PARAMETER","METALLICITY=0*L+0.008[M/H] "],["PARAMETER","TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "],["COMMENT","FLUX UNIT=erg/s/cm^2/A @ 10pc "]]
        parameter_list=["TAU"]
        cleaned = handler.cleanParameters(params, parameter_list)
        
        expected = [["NAME","Calzetti"],["PARAMETER","TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "],["COMMENT","FLUX UNIT=erg/s/cm^2/A @ 10pc "]]
        assert np.array_equal(cleaned, expected)
        
        
    def test_checkParameter(self):
        # all the same OK
        params=[ [["NAME","A"],["PARAMETER","METALLICITY=0*L+0.008[M/H] "],["PARAMETER","TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]],  [["NAME","B"],["PARAMETER","METALLICITY=0*L+0.009[M/H] "],["PARAMETER","TAU=0*L+0.4[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]], [["NAME","C"],["PARAMETER","METALLICITY=0*L+0.01[M/H] "],["PARAMETER","TAU=0*L+0.5[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]]]
        
        checked = handler.checkParameter(params, False)
        assert np.array_equal(params, checked)
        
        checked = handler.checkParameter(params, True)
        assert np.array_equal(params, checked)
        
        #different no stripping
        params=[ [["NAME","A"],["PARAMETER","METALLICITY=0*L+0.008[M/H] "],["PARAMETER","TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]],  [["NAME","B"],["PARAMETER","METALLICITY=0*L+0.009[M/H] "],["PARAMETER","TAU=0*L+0.4[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]], [["NAME","C"],["PARAMETER","TAU=0*L+0.5[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]]]
        try:
            checked = handler.checkParameter(params, False)
        except Exception:
            raised = True
        assert raised
        
        #different  stripping
        checked = handler.checkParameter(params, True)
        expected = [ [["NAME","A"],["PARAMETER","TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]],  [["NAME","B"],["PARAMETER","TAU=0*L+0.4[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]], [["NAME","C"],["PARAMETER","TAU=0*L+0.5[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]]]
        assert np.array_equal(expected, checked)
    
    def test_checkName(self):
        params=[ [["NAME","A"],["PARAMETER","METALLICITY=0*L+0.008[M/H] "],["PARAMETER","TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]],  [["PARAMETER","METALLICITY=0*L+0.009[M/H] "],["PARAMETER","TAU=0*L+0.4[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]], [["Filter name","C"],["PARAMETER","TAU=0*L+0.5[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]]]
        names = ['f1','f2','f3']
        
        checked =  handler.checkName(params, names)
        assert np.array_equal(['A','f2','C'], checked)
        
    def test_simplifyParam(self):
        params=[["NAME","Calzetti"],["PARAMETER","METALLICITY=0*L+0.008[M/H] "],["PARAMETER","TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "],["COMMENT","FLUX UNIT=erg/s/cm^2/A @ 10pc "]]
        
        simplified = handler.simplifyParam(params)
        expected = [["PARAMETER","METALLICITY=0*L+0.008[M/H] ;TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom ;FLUX UNIT=erg/s/cm^2/A @ 10pc "]]
        assert np.array_equal(expected, simplified)
        
    def test_buildParamTable(self):
         params=[ [["PARAMETER","METALLICITY=0*L+0.008[M/H] ;TAU=0*L+0.3[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]],  
                   [["PARAMETER","METALLICITY=0*L+0.009[M/H] ;TAU=0*L+0.4[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]], 
                   [["PARAMETER","METALLICITY=0*L+0.010[M/H] ;TAU=0*L+0.5[Gyr] "],["COMMENT","WAVELENGTH UNIT=Angstrom "]]]
         names = ['n1','n2','n3']
         
         t = handler.buildParamTable(params, names)
         assert 'NAME'  in t.colnames
         assert 'KEY'  in t.colnames
         assert 'VALUE'  in t.colnames
         n1 = t[t['NAME']=='n1']
         print(n1)
         assert len(n1)==2
         assert 'PARAMETER' in n1['KEY']
         assert 'COMMENT' in n1['KEY']
