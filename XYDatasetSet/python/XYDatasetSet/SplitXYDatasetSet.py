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
import XYDatasetSet.XYDatasetSetTools as DatasetTool

logger = Logging.getLogger('SplitXYDatasetSet')


def defineSpecificProgramOptions():
    description = """
            Read a XYDataset group and split it in individual text files
    """
    
    parser = argparse.ArgumentParser(description=description)

    parser.add_argument("-if", "--input-file", type=str, required=True,
                        help="The path of the DatasetSet (.fits) file to be split")
                        
  
    parser.add_argument("-of", "--output-folder", type=str, required=True,
                        help="Path of the output folderthe individual files will be stored into")
    return parser
                    



def mainMethod(args):
    input_file = args.input_file
    if '/' not in input_file:
        input_file = './'+input_file
    input_folder =  '/'.join(input_file.split('/')[:-1])
    file_group = '.'.join(input_file.split('/')[-1].split('.')[:-1])
    file_ext = input_file.split('/')[-1].split('.')[-1]
    if file_ext!='fits':
        raise Exception("The input file should be a .fits file")
        
    ds_dict = DatasetTool.listDataset(input_folder) 
    names = [key for key in ds_dict if ds_dict[key]==file_group+'.'+file_ext]
    
    logger.info('Check output folder')
    out_folder = args.output_folder
    os.makedirs(out_folder, exist_ok=True)
    
    logger.info('Create the output files')
    files_names = []
    for ds in [DatasetTool.readDataset(input_folder, nm) for nm in names]:
        files_names.append(DatasetTool.writeDataset(ds,out_folder).replace(out_folder,''))
    
    logger.info('Add the order file')
    with open(out_folder+'/order.txt', 'w') as fh:
        for names in files_names:
            fh.write(f'{names}\n') 
    
    
    
    
