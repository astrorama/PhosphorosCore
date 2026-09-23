#
# Copyright (C) 2012-2022 Euclid Science Ground Segment
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

import os
import argparse
import numpy as np
from astropy.table import Table
from astropy.io import fits

from ElementsKernel import Logging

logger = Logging.getLogger('BuildXYDatasetSet')


def defineSpecificProgramOptions():
    description = """
            Read XYDataset and group them into a single DatasetSet .fits file
    """
    
    parser = argparse.ArgumentParser(description=description)

    parser.add_argument("-id", "--input-dir", type=str, required=True,
                        help="The folder containing the XYDataset to be grouped in a DatasetSet file")
                        
    parser.add_argument("-rs", "--resample", type=str, default="YES",
                        help="If input XYDatasets do not have the same sampling, the tool can resample them ('YES') or return an error ('NO'), default = YES")
                        
    parser.add_argument("-sp", "--strip-parameter", type=str, default="NO",
                        help="If input XYDatasets do not have the same PARAMETERS, the tool can keep only the ones in common ('YES') or return an error ('NO'), default = NO")

    parser.add_argument("-of", "--output-file", type=str, required=True,
                        help="Path of the output DatasetSet .fits file")
    return parser
                        
 
def readXYDataSet(path):
    with open(path,"r") as dataset_file:
            dataset = dataset_file.read().split('\n')
    # get the param
    params_list = [row for row in dataset if row.startswith("#")]
    
    params = []
    for p in params_list:
        bits = p.split(' : ')
        if len(bits)==2:
            params.append([bits[0].replace('#','').replace(' ',''),bits[1]])
    
    # get the XY data
    data_list = [row.strip() for row in dataset if not row.startswith("#") and len(row.strip().split(' '))==2]
    sampling = [float(sample.split(' ')[0]) for sample in data_list]
    values = [float(sample.split(' ')[1]) for sample in data_list]
    return sampling, values, params, path.split("/")[-1].rsplit( ".", 1 )[ 0 ]

def listAndOrderFiles(path):
    all_files = [f for f in os.listdir(path) if os.path.isfile(os.path.join(path, f))]
    # Ordering if order.txt is present
    if "order.txt" in all_files:
        all_files.remove("order.txt")
        with open(os.path.join(path, "order.txt")) as order_file:
            order = order_file.read()
        new_order = []
        missing = []
        for dataset in order.split('\n'):
            if dataset in all_files:
                new_order.append(dataset)
                all_files.remove(dataset)
            else:
                missing.append(dataset)
        if len(missing)!=0:
            logger.warning(f'{len(missing)} files are listed in "order.txt" but not present in folder {path}')
        if len(all_files) > 0:
            logger.warning(f'{len(all_files)} files are in folder {path} but not listed in "order.txt", they will be put at the end of the list')
            for dataset in all_files:
                new_order.append(dataset)
        return new_order
    else:
        return all_files
    

def checkSampling(samplings, values, resample):
    alligned = True
    for idx in range(len(samplings)-1):
        alligned &= np.array_equal(np.array(samplings[idx]), np.array(samplings[-1]))
    if not alligned:
        if not resample:
            raise Exception("XYDatasets have different sampling and the resample option is turned off.")
        else:
            all_samples = np.unique(np.array([b for c in samplings for b in c]))
            out_values = np.zeros((len(samplings), len(all_samples)))
            for idx in range(len(samplings)):
                out_values[idx,:] = np.interp(all_samples, samplings[idx], values[idx],left=0.0, right=0.0)
            return np.matmul(np.ones((len(samplings),1)),all_samples.reshape(1,len(all_samples))), out_values 
    else:
        return samplings, values
        
def getParameters(params):
    # files store the parameters as # PARAMETER : METALLICITY=0*L+0.008[M/H] , get the names.
    return np.unique(np.array([p[1].split("=")[0] for p in params if p[0]=="PARAMETER"]))
    
    
def cleanParameters(param, to_keep):
    # remove PARAMETERS which are not in the to_keep list
    return [p for p in param if p[0]!="PARAMETER" or p[1].split("=")[0] in to_keep]
    
def checkParameter(params, strip_param):
    param_lists = []
    alligned = True
    for idx in range(len(params)):
         param_lists.append(getParameters(params[idx])) 
    param_lists = np.array([b for c in param_lists for b in c])
    unique, counts = np.unique(param_lists, return_counts=True)
    if len( np.unique(counts))>1:
        if not strip_param:
            raise Exception("Not All the XYDatasets have the same PARAMETERs and the strip-parameter option is turned off.")
        else:
            to_keep=[]
            for idx in range(len(unique)):
                if counts[idx]==len(params):
                    to_keep.append(unique[idx])
            logger.warning(f"only the following PARAMETERs will be kept : {to_keep}")
            params = [cleanParameters(param, to_keep) for param in params]
    return params
    
def checkName(params, names):
    # Filters have # Filter name : 2MASS/2MASS.H others XYDataset # NAME : Name, if none present keep the name of the file instead
    for idx in range(len(params)):
        name_from_file = [p[1] for p in params[idx] if p[0]=="Filter name" or p[0]=="NAME"]  
        if len(name_from_file)==1:
            names[idx] = name_from_file[0]
    return names
    
def simplifyParam(param):
    param_dict = {}
    for p in param:
        if p[0] not in param_dict:
            param_dict[p[0]]=p[1]
        else:
            param_dict[p[0]] = param_dict[p[0]] + ";" + p[1] 
    new_param = []
    for k in param_dict:
        if k not in ['Filter name','NAME']:
            new_param.append([k, param_dict[k]])
    return new_param
    
    
def simplifyParams(params):
    # Group elements with the same tag
    for idx in range(len(params)):
        params[idx] = simplifyParam(params[idx])
    return params
    
    
def buildSamplingTable(sampling):
    t = Table()
    t['WAVELENGTH'] = sampling
    return t
    
def buildValuesTable(values, names):
    t = Table()
    t['NAME'] = names
    t['VALUES'] = values
    return t
    
def buildParamTable(params, names):
    t = Table()
    col_name=[]
    col_key=[]
    col_val=[]
    for idx in range(len(params)):
        for p in params[idx]:
            col_name.append(names[idx])
            col_key.append(p[0])
            col_val.append(p[1])
    
    t['NAME'] = col_name
    t['KEY'] = col_key
    t['VALUE'] = col_val
    return t
    
def buildHduList(samplings, values, names, params):
    primary_hdu = fits.PrimaryHDU()
    sampling_hdu = fits.BinTableHDU(data=buildSamplingTable(samplings[0]), name='SAMPLING')
    values_hdu = fits.BinTableHDU(data=buildValuesTable(values, names), name='VALUES')
    param_hdu = fits.BinTableHDU(data=buildParamTable(params, names), name='PARAMETERS')
    
    return fits.HDUList(hdus=[primary_hdu, sampling_hdu, values_hdu, param_hdu])        
            

def mainMethod(args):
    logger.info(f'List and order XYDataset from folder {args.input_dir}')
    ordered = listAndOrderFiles(args.input_dir)
    logger.info(f'{len(ordered)} XYDatasets have been found')
    
    samplings = []
    values = []
    params = []
    names = []
    
    logger.info(f'Read the XYDatasets')
    for file_name in ordered:
        logger.debug(f'Read {file_name}')
        sampling, value, param, name = readXYDataSet(os.path.join(args.input_dir, file_name))
        samplings.append(sampling)
        values.append(value)
        params.append(param)
        names.append(name)
    
    logger.info(f'Look for names in the tags')
    names = checkName(params, names)
    logger.info(f'Check sampling')
    samplings, values = checkSampling(samplings, values, args.resample=="YES")
    logger.info(f'Check PARAMETERs')
    params = checkParameter(params, args.strip_parameter)
    logger.info(f'Clean the tags')
    params = simplifyParams(params)
    
    
    logger.info(f'Build the output .fits')
    hdul = buildHduList(samplings, values, names, params)
    hdul.writeto(args.output_file, overwrite=True)
    logger.info(f'Output writen to {args.output_file}')

    
