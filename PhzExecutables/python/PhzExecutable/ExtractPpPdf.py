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
File: python/PhzExecutable/ExtractPpPdf.py

Created on: 2022/06/15
Author: fdubath

"""
import sys
import argparse
import ElementsKernel.Logging as log
import numpy as np
from astropy.table import Table, Column
from astropy.io import fits
from os.path import exists
from os.path import join

logger = log.getLogger('PdfPP')


def defineSpecificProgramOptions():
    parser = argparse.ArgumentParser()
    
    parser.add_argument('-rd','--result-dir', type=str, required=True,
                        help='Path to the folder containing the result of the compute redshift run')
    parser.add_argument('-ppc','--physical-parameter-config', type=str, default='PhysicalParameterConfig.fits',
                        help='Name of the file containing the Physical Parameter config (default="PhysicalParameterConfig.fits")')
    parser.add_argument('-pf','--posterior-folder', type=str, default='posteriors',
                        help='Name of the folder containing the sampling (default="posteriors")')
    parser.add_argument('-z','--add-redshift', type=str, default='',
                        help='If set add the redshift to the PP')
    parser.add_argument('-if','--index-file', type=str, default='Index_File_posterior.fits',
                        help='Name of the sampling\'s index file (default="Index_File_posterior.fits")')   
    parser.add_argument('-orf','--output-range-file', type=str, default='',
                        help='Path to the file containing the ranges of the PP, if not set the file is not produced')                 
    parser.add_argument('-pb','--pdf-bin', action='append', default=[],
                        help='Number of bin for each PP, use the  syntax <ppname>:<number>,<ppname>:<number>,...,<ppname>:<number>. If a pp is not provided the default value is 50')
    parser.add_argument('-pr','--pdf-range', action='append', default=[],  
                        help='Range for the binning of the PP, use the syntax <ppname>:<min>:<max>,<ppname>:<min>:<max>,...,<ppname>:<min>:<max>. If a pp is not provided the default value is the extremal range of this pp') 
    parser.add_argument('-pl','--pdf-list', action='append', default=[],  
                        help='list of 1d or 2d pdf to be produced, syntax is <pp1>,<pp1>_<pp2>,... where pp can be any of the physical paramater or the redshift (REDSHIFT), if not provided the tool produce 1d pdf for each pp. ')   
    parser.add_argument('-opf','--output-pdf-file', type=str, default='',
                        help='Path to the file containing the PP\'s pdf, if not set the file is not produced')
    
    return parser

#--------------------------   
def parsePdfList(pp, user_input): 
    ''' 
    Returns the lists of 1d and 2d pdfs to be computed.

            Parameters:
                    pp (array): The list of available physical parameter
                    user_input (array): A list describing the pdf to be computed in the form <pp1>,<pp1>_<pp2>,..

            Returns:
                    pdf_1d (array): List of pp for which 1D pdf should be computed 
                    pdf_2d (array): list of pair of pp for which 2D pdf should be computed 
    '''   
    full_list = pp[:] 
    pdf_1d = []
    pdf_2d = []
    for raw in user_input:
        for el in raw.split(','):
            pieces = el.split('_')
            if len(pieces) == 1 :
                pdf_1d.append(pieces[0])
            elif len(pieces) == 2 : 
                pdf_2d.append(pieces) 
            else :
                raise ValueError('Only 1D and 2D pdf are supported.')
    error = False  
    for param in pdf_1d:
        if not param in full_list:
            logger.error(param+' is not a valid parameter')
            error=True
    for pair in pdf_2d:
        if not pair[0] in full_list:
            logger.error(pair[0]+' is not a valid parameter')
            error=True
        if not pair[1] in full_list:
            logger.error(pair[1]+' is not a valid parameter')
            error=True
    if error:
        raise ValueError('One of the Parameter was not recognised.')
    if len(pdf_1d)==0 and len(pdf_2d)==0:
        pdf_1d = pp.copy() 
    return pdf_1d, pdf_2d

#--------------------------    
def parsePdfBin(pp, user_input):
    ''' 
    Returns the list of sampling number for the pp. If a given pp is not specified 
    in the user_input the default values (50) is used.

            Parameters:
                    pp (array): The list of available physical parameter
                    user_input (array): A list describing the number of bin for the  
                                        pps in the form <ppname>:<number>,<ppname>:<number>,...


            Returns:
                    elem (dict): a dictionary which key is the pp and value is the number of sample for this pp.
    '''   
    elem = {}
    for raw in user_input:
        for el in raw.split(','):
            pieces = el.split(':')
            if len(pieces) == 2 and pieces[1].isdigit():
                elem[pieces[0]]= int(pieces[1])
    for param in pp:
        if not param in elem.keys():
           elem[param] = 50
    unknow = []       
    for param in elem.keys():
        if not param in pp:
            unknow.append(param)
    for param in unknow:
        del elem[param]
    return elem
    
#-------------------------- 
def parsePdfRange(pp, min_data, max_data, user_input):  
    ''' 
    Returns the list of range for the pp. If a given pp is not specified in the user_input the 
    default values is the range min_data, max_data for the given pp.

            Parameters:
                    pp (array): The list of available physical parameter
                    min_data (dict): Dictionary containing the minimal value taken by each pp
                    max_data (dict): Dictionary containing the maximum value taken by each pp
                    user_input (array): A list describing the min/max values for the pps in the form  
                                       <ppname>:<min>:<max>,<ppname>:<min>:<max>,...

            Returns:
                    elem (dict): a dictionary which key is the pp and value is the couple min, max value for this pp.
    '''    
    elem = {}
    for raw in user_input:
        for el in raw.split(','):
            pieces = el.split(':')
            if len(pieces) == 3 and pieces[1].isdigit() and pieces[2].isdigit():
                elem[pieces[0]]= [float(pieces[1]), float(pieces[2])]
    for param in pp:
        if not param in elem.keys():
           elem[param] = [min_data[param], max_data[param]]
    unknow = []       
    for param in elem.keys():
        if not param in pp:
            unknow.append(param)
    for param in unknow:
        del elem[param]
    return elem
    
#-------------------------- 
def getPP(res_dir, pp_conf_file):
    '''
    Returns the list of physical parameters (pp) and a dictionary storing the units for each pp
    
            Parameters:
                res_dir (str): folder containing the result files of a Phosphoros run
                pp_conf_file (str): name of the file containing the configuration of the pp
                
            Returns:
                    pp (array): A list containing the names of the available pp.
                    units (dict): A dictionary storing the units for each pp.
    '''
    pp_config_table = Table.read(join(res_dir, pp_conf_file))
    pp=[]
    units = {}
    for index in range(len(pp_config_table)):
        fl = pp_config_table['PARAM_NAME'][index]
        if not fl.strip() in pp:
            pp.append(fl.strip())
            units[fl.strip()] = pp_config_table['UNITS'][index];
    if not 'REDSHIFT' in pp:
        pp.append('REDSHIFT')  
        units['REDSHIFT'] = '';
    return pp, units
        
#-------------------------- 
def getSampleFileList(res_dir, sample_folder, index_file):
    '''
    Returns the list of files containing full pdf sampling (with pp)
    
            Parameters:
                res_dir (str): folder containing the result files of a Phosphoros run
                sample_folder (str): name of the folder into which the sample files are stored
                index_file (str): name of the file containing the list of objects and in 
                                  which file the sampling are stored
                
            Returns:
                    filelist (array): list of files containing full pdf sampling.
    '''
    index_file = join(res_dir, sample_folder, index_file)
    ref_index = Table.read(index_file)
    filelist = set(map(str.strip, ref_index['FILE_NAME']))
    
    return filelist

#-------------------------- 
def getData(pp, filelist, res_dir, sample_folder, get_sample):
    '''
    Returns the samples and the min/max values for each pp
    
            Parameters:
                pp (array): The list of available physical parameter
                filelist (array): list of files containing full pdf sampling.
                res_dir (str): folder containing the result files of a Phosphoros run
                sample_folder (str): name of the folder into which the sample files are stored
                get_sample (bool): switch to return or not the sampling (min/max are always returned)
                
            Returns:
                    samples (dict(dict)): Dictionary which key is the pp and value is a dictionary 
                                          which key is the object id and value the list of sampling.
                    min_data (dict): Dictionary containing the minimal value taken by each pp
                    max_data (dict): Dictionary containing the maximum value taken by each pp
    '''
    samples = {}
    min_data={}
    max_data={}
    for param in pp:
        samples[param]={}
        min_data[param]=np.finfo(float).max
        max_data[param]=np.finfo(float).min
     
    f_index=1    
    for fl in filelist:
        logger.info('Processing sampling file %d of %d', f_index, len(filelist))
        sampling_table = Table.read(join(res_dir,sample_folder,str(fl)))
        for param in pp:
            if (get_sample):
                ''' 
                The sampling files contains a column "OBJECT_ID". In this columns we have a repetition 
                (one for each sample of the source). Looking for the position of the change in this column
                we can split the table in subset: one for each object.
                '''
                indexes = np.where(np.diff(sampling_table['OBJECT_ID']))[0]+1
                for dat, idx in zip(np.split(sampling_table[param], indexes), np.split(sampling_table['OBJECT_ID'], indexes)):
                    samples[param][idx[0]]=np.array(dat)
            min_data[param] = min(np.min(sampling_table[param]),min_data[param]) 
            max_data[param] = max(np.max(sampling_table[param]),max_data[param]) 
        f_index+=1;
    return samples, min_data, max_data

#-------------------------- 
def outputRange(range_file, min_data, max_data, units):
    '''
    Outputs the ranges in a fits file
            Parameters:
                range_file (str): The name of the file into which export the ranges
                min_data (dict): Dictionary containing the minimal value taken by each pp
                max_data (dict): Dictionary containing the maximum value taken by each pp
                units (dict): A dictionary storing the units for each pp.
    '''
    outTable = Table()
    outTable['PP'] = list(min_data.keys())
    outTable['MIN'] = list(min_data.values())
    outTable['MAX'] = list(max_data.values())
    outTable['UNITS'] = list(units.values())
    outTable.write(range_file, overwrite=True)

#-------------------------- 
def computeHisto1d(pp, samples, pdf_range, pdf_bin):
    '''
    Returns the 1D pdfs
            Parameters:
                pp (array): The list of physical parameter for which the 1D pdf must be computed 
                samples (dict(dict)): Dictionary which key is the pp and value is a dictionary 
                                          which key is the object id and value the list of sampling.
                pdf_range (dict): a dictionary which key is the pp and value is the couple min, max value for this pp.
                pdf_bin (dict): a dictionary which key is the pp and value is the number of sample for this pp.
                    
            Returns:
                    histo (dict): A dictionary which key is the pp and values is a list of 1D histogram for each source
    '''
    histo = {}
    if len(pp)>0:
        for param in pp:
            histo[param]=[]
        all_object_id = list(samples[pp[0]].keys())
        for obj_id in all_object_id:
            for param in pp:
                curr_samples = samples[param][obj_id]
                min_v = pdf_range[param][0]
                max_v = pdf_range[param][1]
                hist, bin_edges = np.histogram(curr_samples,bins= pdf_bin[param],range=(min_v,max_v), density=True)
                histo[param].append(hist)
    return histo
    
#-------------------------- 
def computeHisto2d(pdf_2d, samples, pdf_range, pdf_bin):
    '''
    Returns the 2D pdfs
            Parameters:
                pdf_2d (array): The list of physical parameter for which the 2D pdf must be computed 
                samples (dict(dict)): Dictionary which key is the pp and value is a dictionary 
                                          which key is the object id and value the list of sampling.
                pdf_range (dict): a dictionary which key is the pp and value is the couple min, max value for this pp.
                pdf_bin (dict): a dictionary which key is the pp and value is the number of sample for this pp.
                    
            Returns:
                    histo (dict): A dictionary which key is the pp couple and values is a list of 2D histogram for each source
    '''
    histo = {}
    if len(pdf_2d)>0:
        for param in pdf_2d:
            histo[param[0]+'_'+param[1]]=[]
        all_object_id = list(samples[pdf_2d[0][0]].keys())
        for obj_id in all_object_id:
            for param in pdf_2d:
                key = param[0]+'_'+param[1]
                curr_samples_x = samples[param[0]][obj_id]
                curr_samples_y = samples[param[1]][obj_id]
                min_v_x = pdf_range[param[0]][0]
                max_v_x = pdf_range[param[0]][1]
                min_v_y = pdf_range[param[1]][0]
                max_v_y = pdf_range[param[1]][1]
                bin_x = pdf_bin[param[0]]
                bin_y = pdf_bin[param[1]]
                hist, bin_edges_x, bin_edges_y = np.histogram2d(curr_samples_x, curr_samples_y, bins=[bin_x,bin_y], range=[[min_v_x, max_v_x],[min_v_y, max_v_y]], density=True)
                histo[key].append(hist)
    return histo
    
#-------------------------- 
def outputPDF(pdf_1d, pdf_2d, samples, histo1d, histo2d, min_data, max_data, pdf_bin, units, out_file):
    '''
    Outputs the pdfs into a fits file
            Parameters:
                pdf_1d (array): The list of physical parameter for which the 1D pdf must be computed 
                pdf_2d (array): The list of physical parameter for which the 2D pdf must be computed 
                samples (dict(dict)): Dictionary which key is the pp and value is a dictionary 
                                          which key is the object id and value the list of sampling.
                histo1d (dict): A dictionary which key is the pp couple and values is a list of 1D histogram for each source
                histo2d (dict): A dictionary which key is the pp couple and values is a list of 2D histogram for each source
                min_data (dict): Dictionary containing the minimal value taken by each pp
                max_data (dict): Dictionary containing the maximum value taken by each pp
                pdf_bin (dict): a dictionary which key is the pp and value is the number of sample for this pp.
                units (dict): A dictionary storing the units for each pp.
                out_file (str): Name of the file into which storing the pdfs.
    '''
    all_object_id=[]
    if len(pdf_1d)>0:
        all_object_id = list(samples[pdf_1d[0]].keys())
    elif len(pdf_2d)>0:
        all_object_id = list(samples[pdf_2d[0][0]].keys())
    outTable = Table()
    outTable['ID'] = all_object_id
    for param in pdf_1d:
        outTable['MC_' + param.upper()] = histo1d[param]  

    for param in pdf_2d:
        key = param[0] + '_' + param[1]
        out_key = 'MC_'+ param[0].upper() + '_' + param[1].upper()
        np_array = np.array(histo2d[key])
        # print(np_array.shape)
        outTable[out_key] = np.reshape(np_array, (np_array.shape[0], (np_array.shape[1] * np_array.shape[2]))) 
    
    hdul = fits.HDUList(hdus=[fits.PrimaryHDU(), fits.BinTableHDU(outTable)])
    hdul[1].name = 'PDFs'
    
    full_pp = pdf_1d.copy()
    for param in pdf_2d:
        if not param[0] in full_pp:
            full_pp.append(param[0]) 
        if not param[1] in full_pp:
            full_pp.append(param[1]) 
    for param in full_pp:
        min_v = min_data[param]
        max_v = max_data[param]
        number = pdf_bin[param]
        nodes = [min_v + i*(max_v - min_v)/(number-1) for i in range(number)]
        name = ('BINS_MC_PDF_'+param.upper())
        bins_table = Table()
        bins_table['BINS'] = Column(data=nodes, unit=units[param])
        hdul.append(fits.BinTableHDU(bins_table))
        hdul[len(hdul)-1].name = name
   
    
    hdul.writeto(out_file, overwrite=True)
    hdul.writeto('/home/dubathf/test.fits', overwrite=True)

    
#-------------------------- 
def mainMethod(args):
    pp, units = getPP(args.result_dir, args.physical_parameter_config)
    pdf_1d, pdf_2d = parsePdfList(pp, args.pdf_list)
    pdf_bin = parsePdfBin(pp, args.pdf_bin)
    
    file_list = getSampleFileList(args.result_dir, args.posterior_folder, args.index_file)
    
    do_get_sample = args.output_pdf_file != ''    
    if len(args.add_redshift)>0 and not 'REDSHIFT' in pp:
        pp.append('REDSHIFT')
    samples, min_data, max_data = getData(pp, file_list, args.result_dir, args.posterior_folder, do_get_sample)

    if args.output_range_file != '':
        outputRange(args.output_range_file, min_data, max_data, units)
    
    if do_get_sample:
        pdf_range = parsePdfRange(pp, min_data, max_data, args.pdf_range)
        histo1d = computeHisto1d(pdf_1d, samples, pdf_range, pdf_bin)
        histo2d = computeHisto2d(pdf_2d, samples, pdf_range, pdf_bin)
        outputPDF(pdf_1d, pdf_2d, samples, histo1d, histo2d, min_data, max_data, pdf_bin, units, args.output_pdf_file)
    
    
    
    

