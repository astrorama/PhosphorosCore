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

logger = Logging.getLogger('XYDatasetSetTools')

class XYDataset:
    def __init__(self, x, y, name, keywords={}):
        self.x = x
        self.y = y
        if len(x)!=len(y):
            raise Exception('x and y should have the same size')
        self.name = name
        self.keywords=keywords
        
    def listKeyword(self):
        return self.keywords
        
    def getKeyword(self, key):
        if key in  self.keywords:
            return self.keywords[key] 
        else :
            return ""
            
    def listParam(self):
        parameters = self.getKeyword('PARAMETER')
        if parameters!='':
            return { p.split('=')[0]:p.split('=')[1]  for p in parameters.split(';')} 
        else:
            return {}
        
    def getParameter(self, parameter):
        param_dict = self.listParam()
        if  parameter in param_dict:
            return param_dict[parameter]
        else :
            return ''
            
    def removeParam(self, parameter):
        param_dict = self.listParam()
        if  parameter in param_dict:
            del param_dict[parameter]
        new_list = ";".join([pm+'='+param_dict[pm] for pm in param_dict])
        self.keywords['PARAMETER']=new_list
        
    
    def addParam(self, key, value):
        param_dict = self.listParam()
        param_dict[key]=value
        new_list = ";".join([pm+'='+param_dict[pm] for pm in param_dict])
        self.keywords['PARAMETER']=new_list
   
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

def listDataset(folder):
    folder = (folder+'/').replace('//','/')
    results = {}
    for root, subdirs, filenames in os.walk(folder):
        base_name = root.replace(folder,'')
        for filename in filenames:
            if os.path.isfile(os.path.join(root,filename)):
                if os.path.splitext(filename)[1]=='.fits':
                    try:
                        group_name = os.path.splitext(filename)[0]
                        hdul = fits.open(os.path.join(root,filename))
                        if len(hdul)==4 and hdul[1].name == 'SAMPLING' and hdul[2].name == 'VALUES' and hdul[3].name == 'PARAMETERS':
                             for name in hdul[2].data['NAME']:
                                results[os.path.join(base_name,group_name,name)] = os.path.join(base_name, filename)
                    except:
                        pass
                else:
                    try:
                        with open(os.path.join(root,filename), 'r') as fh:
                            logger.debug(f'File {os.path.join(root,filename)} is opened ')
                            header=[]
                            content = []
                            for line in fh :
                                if line.startswith('#'):
                                    header.append(line)
                                else:
                                    words = line.replace('\n','').split(' ')
                                    words = [w for w in words if w]
                                    if len(words)==2:
                                        content.append(line)
                            if len(content)>2:
                                path = os.path.join(base_name, filename)
                                name = os.path.join(base_name,os.path.splitext(filename)[0])
                                name_from_file = [l.split(':')[1].strip() for l in header if l.startswith('# NAME :') or l.startswith('# Filter name :')]
                                if len(name_from_file)>0:
                                    name = os.path.join(base_name,name_from_file[0])
                                results[name]=path
                    except:
                        pass
                    
    return results;
            

def readDataset(root_path, name):
    datasets = listDataset(root_path)
    if not name in datasets:
        raise KeyError(f'The datased named {name} is not present under path {root_path}')
        
    dataset_path =  datasets[name]
    if dataset_path.endswith('.fits'):
        hdul = fits.open(os.path.join(root_path,dataset_path))
        x= hdul[1].data['WAVELENGTH']
        mask = hdul[2].data['NAME']==name.split('/')[-1]
        y = hdul[2].data[mask]['VALUES'][0]
        mask = hdul[3].data['NAME']==name.split('/')[-1]
        header={}
        for row in  hdul[3].data[mask]:
            header[row['KEY']] = row['VALUE'].strip()
        return XYDataset(np.array(x), np.array(y), name, header) 
        
    else:
        header={}
        content_x = []
        content_y = []
        with open(os.path.join(root_path, dataset_path), 'r') as fh:
       
            for line in fh :
                if line.startswith('#') and not line.startswith('# NAME :') and not line.startswith('# Filter name :'):
                    pieces = line.replace("#",'').split(':')
                    if len(pieces)==2:
                        keyword = pieces[0].strip()
                        if keyword not in header:
                            header[keyword] = pieces[1].strip()
                        else:
                            header[keyword] = header[keyword]+';'+pieces[1].strip()                        
                else:
                    words = line.replace('\n','').split(' ')
                    words = [w for w in words if w]
                    if len(words)==2:
                        content_x.append(float(words[0]))
                        content_y.append(float(words[1]))
        return XYDataset(np.array(content_x), np.array(content_y), name, header) 
                
    
def writeDataset(dataset, path, name_keyword="NAME"):
    file_name = os.path.join(path, dataset.name.split('/')[-1] +'.dat')
    with open(file_name, 'w') as fh:
        fh.write(f'# {name_keyword} : {dataset.name.split('/')[-1]}\n')
        for key in dataset.keywords:
            for val in  dataset.keywords[key].split(';'):
               fh.write(f'# {key} : {val.strip()}\n') 
        for idx in range(len(dataset.x)):
            fh.write(f'{dataset.x[idx]} {dataset.y[idx]}\n') 
    return file_name

def checkSampling(dataset_list, resample):
    alligned = True
    for idx in range(len(dataset_list)-1):
        alligned &= np.array_equal(np.array(dataset_list[idx].x), np.array(dataset_list[-1].x))
    if not alligned:
        if not resample:
            raise Exception("XYDatasets have different sampling and the resample option is turned off.")
        else:
            all_samples = np.sort(np.unique([x_val for ds in dataset_list for x_val in ds.x]))
            for ds in dataset_list:
                ds.y= np.interp(all_samples, ds.x, ds.y, left=0.0, right=0.0)
                ds.x = all_samples
    
    return dataset_list
        
def checkParameter(dataset_list, strip_param):
    all_param = [key for ds in dataset_list for key in ds.listParam()]
    unique, counts = np.unique(all_param, return_counts=True)
    for idx in range(len(counts)):
        if counts[idx]<len(dataset_list):
            if not strip_param:
                raise Exception("Not All the XYDatasets have the same PARAMETERs and the strip-parameter option is turned off.")
            else:
                logger.warning(f"Removing the PARAMETER : {unique[idx]}")
                for ds in dataset_list:
                    if unique[idx] in ds.listParam():
                        ds.removeParam(unique[idx])
    return dataset_list
  
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
            col_key.append(p)
            col_val.append(params[idx][p])
    
    t['NAME'] = col_name
    t['KEY'] = col_key
    t['VALUE'] = col_val
    return t
    
def buildHduList(dataset_list):
    primary_hdu = fits.PrimaryHDU()
    sampling_hdu = fits.BinTableHDU(data=buildSamplingTable(dataset_list[0].x), name='SAMPLING')
    
    names = [ds.name.split('/')[-1] for ds in dataset_list]
    
    if len(names)!=len(np.unique(names)):
        raise Exception("Multiple Dataset With the same name cannot be saved in a single set.")
    values = [ds.y for ds in dataset_list]
    values_hdu = fits.BinTableHDU(data=buildValuesTable(values, names), name='VALUES')
    
    params = [ds.listKeyword() for ds in dataset_list]
    param_hdu = fits.BinTableHDU(data=buildParamTable(params, names), name='PARAMETERS')
    
    return fits.HDUList(hdus=[primary_hdu, sampling_hdu, values_hdu, param_hdu])        
            

def writeDatasetSet(dataset_list, path, resample=False, strip_param=False):
    dataset_list = checkSampling(dataset_list, resample)
    dataset_list = checkParameter(dataset_list, strip_param) 
    hdul = buildHduList(dataset_list)
    hdul.writeto(path, overwrite=True)


