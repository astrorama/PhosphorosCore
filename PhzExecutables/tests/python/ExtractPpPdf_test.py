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

import math
import json

import os
from ElementsKernel.Temporary import TempFile
from ElementsKernel.Temporary import TempDir
from astropy.table import Table
from astropy.io import fits

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
        
        pp=['PP1','PP2','REDSHIFT']   
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

    def test_parsePdfBin(self):   
        pp=['PP1','PP2','PP3']   
        # Default values
        bin_number = worker.parsePdfBin(pp, [])
        assert len(bin_number) == 3
        assert 'PP1' in bin_number.keys()
        assert bin_number['PP1'] == 50
        assert 'PP2' in bin_number.keys()
        assert bin_number['PP2'] == 50
        assert 'PP3' in bin_number.keys()
        assert bin_number['PP3'] == 50
        
        # comma separated
        bin_number = worker.parsePdfBin(pp, ['PP1:60,PP2:70'])
        assert len(bin_number) == 3
        assert 'PP1' in bin_number.keys()
        assert bin_number['PP1'] == 60
        assert 'PP2' in bin_number.keys()
        assert bin_number['PP2'] == 70
        assert 'PP3' in bin_number.keys()
        assert bin_number['PP3'] == 50
        
        # array 
        bin_number = worker.parsePdfBin(pp, ['PP1:60','PP2:70'])
        assert len(bin_number) == 3
        assert 'PP1' in bin_number.keys()
        assert bin_number['PP1'] == 60
        assert 'PP2' in bin_number.keys()
        assert bin_number['PP2'] == 70
        assert 'PP3' in bin_number.keys()
        assert bin_number['PP3'] == 50
        
        # unknown element
        bin_number = worker.parsePdfBin(pp, ['PP1:60','PP2:70','PP5:30'])
        assert len(bin_number) == 3
        assert 'PP1' in bin_number.keys()
        assert bin_number['PP1'] == 60
        assert 'PP2' in bin_number.keys()
        assert bin_number['PP2'] == 70
        assert 'PP3' in bin_number.keys()
        assert bin_number['PP3'] == 50
        
    def test_parsePdfRange(self):
         pp=['PP1','PP2','PP3']   
         min_data={'PP1':0,'PP2':10,'PP3':-10}
         max_data={'PP1':100,'PP2':110,'PP3':90}
         
         # default values
         ranges = worker.parsePdfRange(pp, min_data, max_data, [])
         assert len(ranges)==3
         assert 'PP1' in ranges.keys()
         assert ranges['PP1'][0] == 0
         assert ranges['PP1'][1] == 100
         assert 'PP2' in ranges.keys()
         assert ranges['PP2'][0] == 10
         assert ranges['PP2'][1] == 110
         assert 'PP3' in ranges.keys()
         assert ranges['PP3'][0] == -10
         assert ranges['PP3'][1] == 90
         # comma separated
         ranges = worker.parsePdfRange(pp, min_data, max_data, ['PP1:5:105,PP2:7:107'])
         assert len(ranges)==3
         assert 'PP1' in ranges.keys()
         assert ranges['PP1'][0] == 5
         assert ranges['PP1'][1] == 105
         assert 'PP2' in ranges.keys()
         assert ranges['PP2'][0] == 7
         assert ranges['PP2'][1] == 107
         assert 'PP3' in ranges.keys()
         assert ranges['PP3'][0] == -10
         assert ranges['PP3'][1] == 90
         # array elements
         ranges = worker.parsePdfRange(pp, min_data, max_data, ['PP1:5:105','PP2:7:107'])
         assert len(ranges)==3
         assert 'PP1' in ranges.keys()
         assert ranges['PP1'][0] == 5
         assert ranges['PP1'][1] == 105
         assert 'PP2' in ranges.keys()
         assert ranges['PP2'][0] == 7
         assert ranges['PP2'][1] == 107
         assert 'PP3' in ranges.keys()
         assert ranges['PP3'][0] == -10
         assert ranges['PP3'][1] == 90
         # not understood input
         ranges = worker.parsePdfRange(pp, min_data, max_data, ['PP1:5:105','PP2:7:1:107'])
         assert len(ranges)==3
         assert 'PP1' in ranges.keys()
         assert ranges['PP1'][0] == 5
         assert ranges['PP1'][1] == 105
         assert 'PP2' in ranges.keys()
         assert ranges['PP2'][0] == 10
         assert ranges['PP2'][1] == 110
         assert 'PP3' in ranges.keys()
         assert ranges['PP3'][0] == -10
         assert ranges['PP3'][1] == 90
         # invalid element
         ranges = worker.parsePdfRange(pp, min_data, max_data, ['PP1:5:105','PP5:7:107'])
         assert len(ranges)==3
         assert 'PP1' in ranges.keys()
         assert ranges['PP1'][0] == 5
         assert ranges['PP1'][1] == 105
         assert 'PP2' in ranges.keys()
         assert ranges['PP2'][0] == 10
         assert ranges['PP2'][1] == 110
         assert 'PP3' in ranges.keys()
         assert ranges['PP3'][0] == -10
         assert ranges['PP3'][1] == 90
         
         
    def tes_getPP(self):  
        sed_list = ['SED_'+str(i) for i in range(10)] 
        pp=['PP1','PP2','PP3']      
        A=range(len(sed_list)*len(pp))
        B=np.array(range(len(sed_list)*len(pp))) + 100
        seds = sed_list + sed_list + sed_list
        pps = ['PP1']*len(sed_list) + ['PP2']*len(sed_list) + ['PP3']*len(sed_list)
        units =  ['U_PP1']*len(sed_list) + ['U_PP2']*len(sed_list) + ['U_PP3']*len(sed_list)
        
        t_PP = Table()
        t_PP['PARAM_NAME'] = pps
        t_PP['SED'] = seds
        t_PP['A'] = A
        t_PP['B'] = B
        t_PP['UNITS'] = units
        
        with TempFile() as pp_f:
            t_PP.write(pp_f.path())
            pp_read, units_read = worker.getPP('', pp_f.path())
            
        assert len(pp_read) == 3 
        assert 'PP1' in pp_read
        assert 'PP2' in pp_read
        assert 'PP3' in pp_read
        
        assert len(units_read) == 3 
        assert 'U_PP1' in units_read
        assert 'U_PP2' in units_read
        assert 'U_PP3' in units_read
        assert False
         
    def test_getSampleFileList(self):
        t_sample = Table()
        t_sample['OBJECT_ID']=[1,2,3,4,5,6,7,8,9]  
        t_sample['FILE_NAME']=[' file_1.fits ','file_1.fits','file_1.fits','file_1.fits','file_1.fits','file_1.fits','file_1.fits','file_2.fits','file_2.fits']  
        with TempDir() as idx_d:
            os.mkdir(idx_d.path()+'/toto')
            t_sample.write(idx_d.path()+ '/toto/index.fits')
            file_list = worker.getSampleFileList(idx_d.path(), 'toto', 'index.fits')
            
        assert len(file_list) == 2 
        assert 'file_1.fits' in file_list
        assert 'file_2.fits' in file_list   
        
    def test_getData(self):  
        pp=['PP1','PP2'] 
        t1 = Table()
        t1['OBJECT_ID']= [393998]*10+[393999]*10
        t1['REDSHIFT'] = np.random.normal(1, 0.2, 20)
        t1['PP1'] = np.concatenate([np.random.normal(10, 1, 19),[5]])
        t1['PP2'] = np.concatenate([np.random.normal(20, 2, 19), [11]])
        t2 = Table()
        t2['OBJECT_ID']= [394000]*10+[394001]*10
        t2['REDSHIFT'] = np.random.normal(1, 0.2, 20)
        t2['PP1'] = np.concatenate([np.random.normal(10, 1, 19), [21]])
        t2['PP2'] = np.concatenate([np.random.normal(20, 2, 19),[30]]) 
        with TempDir() as idx_d:
            os.mkdir(idx_d.path()+'/toto')
            t1.write(idx_d.path()+ '/toto/f1.fits')
            t2.write(idx_d.path()+ '/toto/f2.fits')
            samples, min_data, max_data = worker.getData(pp, ['f1.fits','f2.fits'], idx_d.path(), 'toto', True) 
        assert len(min_data) == 2
        assert min_data['PP1']==5
        assert min_data['PP2']==11
        assert len(max_data) == 2
        assert max_data['PP1']==21
        assert max_data['PP2']==30
        assert len(samples) == 2
        assert  'PP1' in samples.keys()
        assert  'PP2' in samples.keys()
        assert len(samples['PP1'])==4
        assert len(samples['PP2'])==4
        assert 393998 in samples['PP1'].keys()
        assert 393999 in samples['PP1'].keys()
        assert 394000 in samples['PP1'].keys()
        assert 394001 in samples['PP2'].keys()
        assert 393998 in samples['PP2'].keys()
        assert 393999 in samples['PP2'].keys()
        assert 394000 in samples['PP2'].keys()
        assert 394001 in samples['PP2'].keys()

        assert len(samples['PP1'][393998]) == 10
        assert len(samples['PP1'][393999]) == 10
        for idx in range(10):
            assert samples['PP1'][393998][idx] == t1['PP1'][idx]
            assert samples['PP1'][393999][idx] == t1['PP1'][idx+10]
       
       # With get sample to false the sample are not in the result
        with TempDir() as idx_d:
            os.mkdir(idx_d.path()+'/toto')
            t1.write(idx_d.path()+ '/toto/f1.fits')
            t2.write(idx_d.path()+ '/toto/f2.fits')
            samples, min_data, max_data = worker.getData(pp, ['f1.fits','f2.fits'], idx_d.path(), 'toto', False) 
        assert len(min_data) == 2
        assert min_data['PP1']==5
        assert min_data['PP2']==11
        assert len(max_data) == 2
        assert max_data['PP1']==21
        assert max_data['PP2']==30
        assert len(samples) == 2
        assert len(samples['PP1'])==0
        assert len(samples['PP2'])==0

    def test_outputRange(self):
        min_data = {'PP1': 0, 'PP2': 10, 'PP3': -10}
        max_data = {'PP1': 100, 'PP2': 110, 'PP3': 90}
        units = {'PP1': "U_PP1", 'PP2': "U_PP2", 'PP3': "U_PP3"}
        with TempDir() as idx_d:
            worker.outputRange(idx_d.path() + '/test.fits', min_data, max_data, units)
            t_read = Table.read(idx_d.path() + '/test.fits')
        assert len(t_read) == 3
        assert 'PP' in t_read.colnames
        assert 'MIN' in t_read.colnames
        assert 'MAX' in t_read.colnames
        assert 'UNITS' in t_read.colnames

        # The dict may not be ordered (python2)
        assert set(t_read['PP']) == {'PP1', 'PP2', 'PP3'}
        for row in t_read:
            if row['PP'] == 'PP1':
                assert row['MIN'] == 0
                assert row['MAX'] == 100
                assert row['UNITS'] == 'U_PP1'
            elif row['PP'] == 'PP2':
                assert row['MIN'] == 10
                assert row['MAX'] == 110
                assert row['UNITS'] == 'U_PP2'
            elif row['PP'] == 'PP3':
                assert row['MIN'] == -10
                assert row['MAX'] == 90
                assert row['UNITS'] == 'U_PP3'
         
    def test_computeHisto1d(self):
        pp=['PP1'] 
        samples={'PP1':{'obj_1':[1.5,2.5,3.5,4.5,4.5,4.5,5.5,6.5,7.5,8.5]}}
        pdf_range={'PP1':[0,10]}
        pdf_bin={'PP1':10}
        histo = worker.computeHisto1d(pp, samples, pdf_range, pdf_bin)
       
        assert len(histo)==1
        assert 'PP1' in histo.keys()
        assert len(histo['PP1'])==1
        assert len(histo['PP1'][0])==10
        assert histo['PP1'][0][0] == 0
        assert histo['PP1'][0][1] == 0.1
        assert histo['PP1'][0][2] == 0.1
        assert histo['PP1'][0][3] == 0.1
        assert histo['PP1'][0][4] == 0.3
        assert histo['PP1'][0][5] == 0.1
        assert histo['PP1'][0][6] == 0.1
        assert histo['PP1'][0][7] == 0.1
        assert histo['PP1'][0][8] == 0.1
        assert histo['PP1'][0][9] == 0
        
        histo = worker.computeHisto1d([], samples, pdf_range, pdf_bin)
       
         
    def test_computeHisto2d(self):
        pps=[['PP1','PP2']] 
        samples={'PP1':{'obj_1':[1.5,2.5,3.5,4.5,4.5,4.5,5.5,6.5,7.5,8.5],'obj_2':[1.5,2.5,3.5,4.5,4.5,4.5,5.5,6.5,7.5,8.5]}, 
                 'PP2':{'obj_1':[1.5,2.5,3.5,4.5,4.5,4.5,5.5,5.5,5.5,5.5],'obj_2':[1.5,2.5,3.5,4.5,4.5,4.5,5.5,5.5,5.5,5.5]}}
        pdf_range={'PP1':[0,10], 'PP2':[1,6]}
        pdf_bin={'PP1':10, 'PP2':6}
        histo = worker.computeHisto2d(pps, samples, pdf_range, pdf_bin) 
        assert len(histo)==1
        assert 'PP1_PP2' in histo.keys()
        assert len(histo['PP1_PP2'])==2
        assert histo['PP1_PP2'][0].shape[0] == 10
        assert histo['PP1_PP2'][0].shape[1] == 6
        assert histo['PP1_PP2'][0][0,0] == 0
        np.testing.assert_almost_equal(histo['PP1_PP2'][0][1,0], 0.12)
        np.testing.assert_almost_equal(histo['PP1_PP2'][0][4,4], 0.36)
        
        assert histo['PP1_PP2'][1].shape[0] == 10
        assert histo['PP1_PP2'][1].shape[1] == 6
        
        
        histo = worker.computeHisto2d([], samples, pdf_range, pdf_bin)
        
        
        
    def test_outputPDF(self):
        histo1d = {}
        histo1d['pp1']=[]
        histo1d['pp1'].append([0.1,0.1,0.1,0.1,0.5])
        pdf_1d = ['pp1']
        
        histo2d = {}
        histo2d['pp1_PP2']=[[[0.0,0.1,0.1,0.1,0.3],[0.1,0.0,0.0,0.0,0.2]]]
        pdf_2d = [['pp1','PP2']]
        
        min_data={'pp1':0,'PP2':0}
        max_data={'pp1':5,'PP2':2}
        
        pdf_bin={'pp1':5,'PP2':2}
        units={'pp1':"U_PP1",'PP2':"U_PP2"}
        
        samples = {'pp1':{'obj_1':[1.5,2.5,3.5,4.5,4.5,4.5,5.5,6.5,7.5,8.5]},'PP2':{'obj_1':[1.5,2.5,3.5,4.5,4.5,4.5,5.5,6.5,7.5,8.5]}}
        
        # Mixed case (both 1d and 2d)
        with TempDir() as idx_d:
            worker.outputPDF(pdf_1d, pdf_2d, samples, histo1d, histo2d, min_data, max_data, pdf_bin, units, idx_d.path()+'/test.fits')
            t_read = Table.read(idx_d.path()+'/test.fits',  hdu=1)
            hdul = fits.open(idx_d.path()+'/test.fits')
          
        assert 'ID' in t_read.colnames
        assert 'MC_PP1' in t_read.colnames
        assert 'MC_PP1_PP2' in t_read.colnames
        assert len(t_read) == 1
        assert t_read[ 'ID'][0] == 'obj_1'
        assert t_read[ 'MC_PP1'][0][0] == 0.1
        assert t_read[ 'MC_PP1'][0][1] == 0.1
        assert t_read[ 'MC_PP1'][0][2] == 0.1
        assert t_read[ 'MC_PP1'][0][3] == 0.1
        assert t_read[ 'MC_PP1'][0][4] == 0.5
        assert t_read[ 'MC_PP1_PP2'][0][0,0] == 0.0
        assert t_read[ 'MC_PP1_PP2'][0][0,1] == 0.1
        assert t_read[ 'MC_PP1_PP2'][0][0,2] == 0.1
        assert t_read[ 'MC_PP1_PP2'][0][0,3] == 0.1
        assert t_read[ 'MC_PP1_PP2'][0][0,4] == 0.3
        assert t_read[ 'MC_PP1_PP2'][0][1,0] == 0.1
        assert t_read[ 'MC_PP1_PP2'][0][1,1] == 0.0
        assert t_read[ 'MC_PP1_PP2'][0][1,2] == 0.0
        assert t_read[ 'MC_PP1_PP2'][0][1,3] == 0.0
        assert t_read[ 'MC_PP1_PP2'][0][1,4] == 0.2

  
        assert len(hdul) == 4
        assert hdul[1].name =='PDFS' 
        assert hdul[2].name =='BINS_MC_PDF_PP1' 
        assert hdul[3].name =='BINS_MC_PDF_PP2' 
        
     
        assert len(hdul[2].data['BINS']) == 5
        assert hdul[2].data['BINS'][0] == 0.0
        assert hdul[2].data['BINS'][1] == 1.25
        assert hdul[2].data['BINS'][2] == 2.5
        assert hdul[2].data['BINS'][3] == 3.75
        assert hdul[2].data['BINS'][4] == 5.0
        assert hdul[2].data.columns['BINS'].unit == "U_PP1"
        
        assert len(hdul[3].data['BINS']) == 2
        assert hdul[3].data['BINS'][0] == 0.0
        assert hdul[3].data['BINS'][1] == 2.0
        assert hdul[3].data.columns['BINS'].unit == "U_PP2"


        # only 1d
        units={'pp1':"U_PP1"}
        with TempDir() as idx_d:
            worker.outputPDF(pdf_1d, [], samples, histo1d, {}, min_data, max_data, pdf_bin, units, idx_d.path()+'/test.fits')
            t_read = Table.read(idx_d.path()+'/test.fits',  hdu=1)
            hdul = fits.open(idx_d.path()+'/test.fits')
          
        assert 'ID' in t_read.colnames
        assert 'MC_PP1' in t_read.colnames
        assert len(t_read) == 1
    
        # only 2d
        units={'pp1':"U_PP1",'PP2':"U_PP2"}
        with TempDir() as idx_d:
            worker.outputPDF([], pdf_2d, samples, {}, histo2d, min_data, max_data, pdf_bin, units, idx_d.path()+'/test.fits')
            t_read = Table.read(idx_d.path()+'/test.fits')
            hdul = fits.open(idx_d.path()+'/test.fits')
          
        assert 'ID' in t_read.colnames
        assert 'MC_PP1_PP2' in t_read.colnames
         
         
         
         
         
