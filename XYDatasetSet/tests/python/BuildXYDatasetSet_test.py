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
            
 

