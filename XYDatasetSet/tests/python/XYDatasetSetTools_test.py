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
File: tests/python/XYDatasetSetTools_test.py.py

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
import zipfile

from ElementsKernel.Auxiliary import getAuxiliaryPath

import XYDatasetSet.XYDatasetSetTools as handler



class TestXYDatasetSetTools(object):
    """
    @class TestXYDatasetSetTools(
    @brief Unit Test class
    """
    
    def test_XYDataset(self):
        dataset = handler.XYDataset([1,2,3,4,5,6],[6,5,4,3,2,1],'toto',{'PARAMETER':'METALLICITY=0*L+0.009[M/H] ;TAU=0*L+0.4[Gyr]'})

        keywords = dataset.listKeyword()
        assert len(keywords)==1
        assert 'PARAMETER' in keywords
        assert keywords['PARAMETER']=='METALLICITY=0*L+0.009[M/H] ;TAU=0*L+0.4[Gyr]'
        
        assert dataset.getKeyword('COMMENT')==''
        assert dataset.getKeyword('PARAMETER')=='METALLICITY=0*L+0.009[M/H] ;TAU=0*L+0.4[Gyr]'
        
        params = dataset.listParam()
        assert len(params)==2
        assert 'TAU' in params
        assert 'METALLICITY' in params
        

        assert dataset.getParameter('TAU') == '0*L+0.4[Gyr]'
        assert dataset.getParameter('METALLICITY') == '0*L+0.009[M/H] '
        assert dataset.getParameter('AGE') == ''

        assert np.array_equal(dataset.x, [1,2,3,4,5,6])
        assert np.array_equal(dataset.y, [6,5,4,3,2,1])

        assert dataset.name == 'toto'
        
        dataset.removeParam('TAU') 
        assert dataset.getParameter('TAU') == ''
        
        dataset.addParam('TAU', '0*L+0.3[Gyr]') 
        assert dataset.getParameter('TAU') == '0*L+0.3[Gyr]'

        raised = False
        try:
             handler.XYDataset([1,2,3,4,5,6],[6,5,4,3,1],'toto',{'PARAMETER':'METALLICITY=0*L+0.009[M/H] ;TAU=0*L+0.4[Gyr]'})
        except Exception:
            raised = True
        assert raised
        
        dataset = handler.XYDataset([1,2,3,4,5,6],[6,5,4,3,2,1],'toto',{})
        keywords = dataset.listKeyword()
        assert len(keywords)==0
        params = dataset.listParam()
        assert len(params)==0
        
        
    def test_listDataset(self):
        with TempDir() as three:
            dir_path = three.path()
            zip_file = getAuxiliaryPath("XYDatasetSet/test_data.zip")
            with zipfile.ZipFile(zip_file, 'r') as zip_ref:
                zip_ref.extractall(dir_path)
                
            dataset_dict = handler.listDataset(dir_path)
            
            expected_datasets = {'base_folder/name_sample_sed':'base_folder/sample_sed.dat',
                                 'base_folder/file_1/f1':'base_folder/file_1.fits',
                                 'base_folder/file_1/f2':'base_folder/file_1.fits',
                                 'base_folder/file_1/f3':'base_folder/file_1.fits',
                                 'base_folder/sub_folder/file_2/f4':'base_folder/sub_folder/file_2.fits',
                                 'base_folder/sub_folder/file_2/f5':'base_folder/sub_folder/file_2.fits',
                                 'base_folder/sub_folder/file_2/f6':'base_folder/sub_folder/file_2.fits'}
            
            assert len(dataset_dict) == len(expected_datasets)
            for key in dataset_dict.keys():
                assert key in expected_datasets
                assert dataset_dict[key]==expected_datasets[key]
                                 
    def test_readDataset(self):        
        with TempDir() as three:
            dir_path = three.path()
            zip_file = getAuxiliaryPath("XYDatasetSet/test_data.zip")
            with zipfile.ZipFile(zip_file, 'r') as zip_ref:
                zip_ref.extractall(dir_path)

            raised = False
            try:
                 handler.readDataset(dir_path, 'not_a_dataset')
            except Exception:
                raised = True
            assert raised
                    
                    
            dataset = handler.readDataset(dir_path, 'base_folder/name_sample_sed')
            assert dataset.getKeyword('COMMENT')=='WAVELENGTH UNIT=Angstrom;FLUX UNIT=erg/s/cm^2/A @ 10pc;IMF is Chabrier - strings in params are not supported yet.;stellar mass is live + remnant stellar mass);L is defined through filter, HSC-i2.txt'
            assert dataset.getKeyword('PARAMETER')=='METALLICITY=0*L+0.008[M/H];TAU=0*L+1[Gyr];AGE=0*L+7.60206[log10(Gyr)];STELLARMASS=0.03406512887661409*L+0[Msun];SFR=1.87260648060497e-09*L+0[Msun/yr]'
            expected_x = np.array([91.0, 94.0 ,96.0 ,98.0 ,100.0 ,102.0 ,104.0 ,106.0 ,108.0 ])
            expected_y = [1.5774248227073961e-12 , 1.7231299937262023e-12 , 1.7699566855558513e-12 , 2.388693626578786e-12 , 3.7962770947641555e-12 , 3.870814693079927e-12 , 3.8858118112106155e-12 , 3.921626044733895e-12 , 3.954817810042366e-12]
            for idx in range(len(dataset.x)):
                assert math.isclose(expected_x[idx], dataset.x[idx])
                assert math.isclose(expected_y[idx], dataset.y[idx])
            assert dataset.name == 'base_folder/name_sample_sed'
            
            dataset = handler.readDataset(dir_path, 'base_folder/file_1/f1')
            assert dataset.getKeyword('COMMENT')=='WAVELENGTH UNIT=Angstrom'
            assert dataset.getKeyword('PARAMETER')=='METALLICITY=0*L+0.008[M/H] ;TAU=0*L+0.3[Gyr]'
            expected_x = np.array([1,2,3,4,5,6,7,8,9,10])
            expected_y = [0,0,0,1,1,0,0,0,0,0]
            for idx in range(len(dataset.x)):
                assert math.isclose(expected_x[idx], dataset.x[idx])
                assert math.isclose(expected_y[idx], dataset.y[idx])
            assert dataset.name == 'base_folder/file_1/f1'
                
    def test_writeDataset(self):
         with TempDir() as three:
            dir_path = three.path()
            zip_file = getAuxiliaryPath("XYDatasetSet/test_data.zip")
            with zipfile.ZipFile(zip_file, 'r') as zip_ref:
                zip_ref.extractall(dir_path)
                
            dataset = handler.XYDataset([1,2,3,4,5,6],[6,5,4,3,2,1],'base_folder/sub_folder/toto',{'PARAMETER':'METALLICITY=0*L+0.009[M/H] ;TAU=0*L+0.4[Gyr]'})
            
            handler.writeDataset(dataset, os.path.join(dir_path,'base_folder/sub_folder'))
            
            readed_dataset = handler.readDataset(dir_path, 'base_folder/sub_folder/toto')
            
            assert np.array_equal(readed_dataset.x, [1,2,3,4,5,6])
            assert np.array_equal(readed_dataset.y, [6,5,4,3,2,1])
            assert readed_dataset.getKeyword('PARAMETER') == 'METALLICITY=0*L+0.009[M/H];TAU=0*L+0.4[Gyr]'


    def test_checkSampling(self):
        dataset_1 = handler.XYDataset([1,2,3,4,5,6],[1,1,1,0,0,0],'toto',{'PARAMETER':'METALLICITY=0*L+0.009[M/H];TAU=0*L+0.4[Gyr]'})
        dataset_2 = handler.XYDataset([1,2,3,4,5,6],[0,0,0,1,1,1],'titi',{'PARAMETER':'METALLICITY=0*L+0.007[M/H];TAU=0*L+0.3[Gyr]'})
        dataset_list = [dataset_1,dataset_2]
        
        dataset_list_out = handler.checkSampling(dataset_list, False)
        assert np.array_equal(dataset_list_out[0].x, [1,2,3,4,5,6])
        assert np.array_equal(dataset_list_out[1].x, [1,2,3,4,5,6])
        
        dataset_list_out = handler.checkSampling(dataset_list, True)
        assert np.array_equal(dataset_list_out[0].x, [1,2,3,4,5,6])
        assert np.array_equal(dataset_list_out[1].x, [1,2,3,4,5,6])
        
        dataset_1 = handler.XYDataset([1,3,4,5,6],[1,1,0,0,0],'toto',{'PARAMETER':'METALLICITY=0*L+0.009[M/H];TAU=0*L+0.4[Gyr]'})
        dataset_2 = handler.XYDataset([1,2,3,4,6],[0,0,0,1,1],'titi',{'PARAMETER':'METALLICITY=0*L+0.007[M/H];TAU=0*L+0.3[Gyr]'})
        dataset_list = [dataset_1,dataset_2]
        
        raised = False
        try:
              dataset_list_out = handler.checkSampling(dataset_list, False)
        except Exception:
            raised = True
        assert raised
        
        dataset_list_out = handler.checkSampling(dataset_list, True)
        assert np.array_equal(dataset_list_out[0].x, [1,2,3,4,5,6])
        assert np.array_equal(dataset_list_out[0].y, [1,1,1,0,0,0])
        assert np.array_equal(dataset_list_out[1].x, [1,2,3,4,5,6])
        assert np.array_equal(dataset_list_out[1].y, [0,0,0,1,1,1])

    def test_checkParameter(self):
        dataset_1 = handler.XYDataset([1,2,3,4,5,6],[1,1,1,0,0,0],'toto',{'PARAMETER':'METALLICITY=0*L+0.009[M/H];TAU=0*L+0.4[Gyr]'})
        dataset_2 = handler.XYDataset([1,2,3,4,5,6],[0,0,0,1,1,1],'titi',{'PARAMETER':'METALLICITY=0*L+0.007[M/H];TAU=0*L+0.3[Gyr]'})
        dataset_list = [dataset_1,dataset_2]
        
        dataset_list_out = handler.checkParameter(dataset_list, False) 
        
        print(dataset_list_out[0].listParam() )
        
        assert len(dataset_list_out[0].listParam()) == 2
        assert 'METALLICITY' in dataset_list_out[0].listParam()
        assert 'TAU' in dataset_list_out[0].listParam()
        assert len(dataset_list_out[1].listParam()) == 2
        assert 'METALLICITY' in dataset_list_out[1].listParam()
        assert 'TAU' in dataset_list_out[1].listParam()
        
        dataset_list_out = handler.checkParameter(dataset_list, True) 
        assert len(dataset_list_out[0].listParam()) == 2
        assert 'METALLICITY' in dataset_list_out[0].listParam()
        assert 'TAU' in dataset_list_out[0].listParam()
        assert len(dataset_list_out[1].listParam()) == 2
        assert 'METALLICITY' in dataset_list_out[1].listParam()
        assert 'TAU' in dataset_list_out[1].listParam()
        
        dataset_1 = handler.XYDataset([1,2,3,4,5,6],[1,1,1,0,0,0],'toto',{'PARAMETER':'METALLICITY=0*L+0.009[M/H];TAU=0*L+0.4[Gyr];AGE=1.0*L+0.0[GYr]'})
        dataset_2 = handler.XYDataset([1,2,3,4,5,6],[0,0,0,1,1,1],'titi',{'PARAMETER':'METALLICITY=0*L+0.007[M/H];TAU=0*L+0.3[Gyr]'})
        dataset_list = [dataset_1,dataset_2]
         
        raised = False
        try:
              dataset_list_out = handler.checkParameter(dataset_list, False)
        except Exception:
            raised = True
        assert raised
        
        dataset_list_out = handler.checkParameter(dataset_list, True) 
        assert len(dataset_list_out[0].listParam()) == 2
        assert 'METALLICITY' in dataset_list_out[0].listParam()
        assert 'TAU' in dataset_list_out[0].listParam()
        assert len(dataset_list_out[1].listParam()) == 2
        assert 'METALLICITY' in dataset_list_out[1].listParam()
        assert 'TAU' in dataset_list_out[1].listParam()
        
        
    def test_buildParamTable(self):
         params=[ {"PARAMETER":"METALLICITY=0*L+0.008[M/H] ;TAU=0*L+0.3[Gyr] ", "COMMENT":"WAVELENGTH UNIT=Angstrom "},  
                   {"PARAMETER":"METALLICITY=0*L+0.009[M/H] ;TAU=0*L+0.4[Gyr] ", "COMMENT":"WAVELENGTH UNIT=Angstrom "}, 
                   {"PARAMETER":"METALLICITY=0*L+0.010[M/H] ;TAU=0*L+0.5[Gyr] ", "COMMENT":"WAVELENGTH UNIT=Angstrom "}]
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
    
    def test_writeDatasetSet(self):
        with TempDir() as three:
            dir_path = three.path()
            zip_file = getAuxiliaryPath("XYDatasetSet/test_data.zip")
            with zipfile.ZipFile(zip_file, 'r') as zip_ref:
                zip_ref.extractall(dir_path)
                
            dataset_1 = handler.XYDataset([1,2,3,4,5,6],[1,1,1,0,0,0],'toto',{'PARAMETER':'METALLICITY=0*L+0.009[M/H];TAU=0*L+0.4[Gyr]'})
            dataset_2 = handler.XYDataset([1,2,3,4,5,6],[0,0,0,1,1,1],'titi',{'PARAMETER':'METALLICITY=0*L+0.007[M/H];TAU=0*L+0.3[Gyr]'})
            
            handler.writeDatasetSet([dataset_1,dataset_2], os.path.join(dir_path,'base_folder/sub_folder/test.fits'))
            
            
            
    
        
        
