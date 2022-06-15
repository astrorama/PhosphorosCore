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
File: tests/python/ExtractPpPdf_test.py

Created on: 2022/06/15
Author: fdubath
"""

from __future__ import division, print_function
import sys
if sys.version_info[0] < 3:
    from future_builtins import *

import py.test
import math
import json

import os
from ElementsKernel.Temporary import TempFile

import PhzExecutable.ExtractPpPdf as worker

import numpy as np

class TestExtractPpPdf(object):
    """
    @class TestExtractPpPdf(
    @brief Unit Test class
    """ 
        
    def test_ProgramOption(self):
        test_path = ""
        parser = worker.defineSpecificProgramOptions();
        with TempFile() as three:
            test_path = three.path()
            ofs = open(test_path, "w")
            parser.print_help(ofs)
            ofs.close()
            
            help_file = open(test_path, 'r') 
            help_txt =  help_file.read() 
            help_file.close()  
            # Check the list of option          
            assert '--result-dir' in help_txt 
            assert '--physical-parameter-config' in help_txt 
            assert '--posterior-folder' in help_txt 
            assert '--index-file' in help_txt 
            assert '--output-range-file' in help_txt 
            assert '--pdf-bin' in help_txt 
            assert '--pdf-range' in help_txt 
            assert '--pdf-list' in help_txt 
            assert '--output-pdf-file' in help_txt 
              
        args = parser.parse_args(['--result-dir','test_path'])   
        #check the default values     
        assert args.result_dir == 'test_path'
        assert args.physical_parameter_config == 'PhysicalParameterConfig.fits'
        assert args.posterior_folder == 'posteriors'
        assert args.index_file == 'Index_File_posterior.fits'
        assert args.output_range_file == ''
        assert args.pdf_bin == []
        assert args.pdf_range == []
        assert args.pdf_list == []
        assert args.output_pdf_file == ''
    
    def test_parsePdfList(self):
        pp=['PP1','PP2','PP3']   

        pdf_1d, pdf_2d = worker.parsePdfList(pp, [])
        # nothing provided, should return pp as pdf_1d and empty as pdf_2d
        assert len(pdf_1d)==3
        assert pdf_1d[0]=='PP1'
        assert pdf_1d[1]=='PP2'  
        assert pdf_1d[2]=='PP3'
        assert len(pdf_2d)==0
        
        # inputs as array
        pdf_1d, pdf_2d = worker.parsePdfList(pp, ['PP1','PP2'])
        assert len(pdf_1d)==2
        assert pdf_1d[0]=='PP1'
        assert pdf_1d[1]=='PP2'  
        assert len(pdf_2d)==0
        
        # input coma separated
        pdf_1d, pdf_2d = worker.parsePdfList(pp, ['PP1,PP2'])
        assert len(pdf_1d)==2
        assert pdf_1d[0]=='PP1'
        assert pdf_1d[1]=='PP2'  
        assert len(pdf_2d)==0
              
        # 2D spectra 
        pdf_1d, pdf_2d = worker.parsePdfList(pp, ['PP1_PP2'])
        assert len(pdf_1d)==0
        assert len(pdf_2d)==1
        assert pdf_2d[0][0]=='PP1' 
        assert pdf_2d[0][1]=='PP2'  
        
        # mixed case
        pdf_1d, pdf_2d = worker.parsePdfList(pp, ['PP1_PP2,PP3'])
        assert len(pdf_1d)==1 
        assert pdf_1d[0]=='PP3'
        assert len(pdf_2d)==1
        assert pdf_2d[0][0]=='PP1' 
        assert pdf_2d[0][1]=='PP2'  
                
        # REDSHIFT case
        pdf_1d, pdf_2d = worker.parsePdfList(pp, ['PP1_REDSHIFT,REDSHIFT'])
        assert len(pdf_1d)==1 
        assert pdf_1d[0]=='REDSHIFT'
        assert len(pdf_2d)==1
        assert pdf_2d[0][0]=='PP1' 
        assert pdf_2d[0][1]=='REDSHIFT'  
        
        # invalid input (unknow PP) 
        try:
            worker.parsePdfList(pp,  ['PP1,PP2,PP4'])
            assert False
        except ValueError:
            assert True
            
        # invalid input (unknow PP) 
        try:
            worker.parsePdfList(pp,  ['PP1,PP2_PP4'])
            assert False
        except ValueError:
            assert True
            
        # invalid input (Higher dim Pdf) 
        try:
            worker.parsePdfList(pp,  ['PP1_PP2_PP4'])
            assert False
        except ValueError:
            assert True

        
