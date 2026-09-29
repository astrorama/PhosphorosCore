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
     

def mainMethod(args):
    logger.info(f'List and order XYDataset from folder {args.input_dir}')
    ordered = listAndOrderFiles(args.input_dir)
    logger.info(f'{len(ordered)} XYDatasets have been found')
     
    ds_dict = DatasetTool.listDataset(args.input_dir)
    print(ds_dict)
    dataset_list = []
    
    logger.info(f'Read the XYDatasets')
    for file_name in ordered:
        logger.info(f'looking for {file_name}')   
        name =''
        for nm in ds_dict:
            if ds_dict[nm] == file_name:
                name=nm
                dataset_list.append(DatasetTool.readDataset(args.input_dir, name))
                break
        
        
    logger.info(f'Build the output .fits')   
    DatasetTool.writeDatasetSet(dataset_list, args.output_file, args.resample=="YES", args.strip_parameter=="YES")
    logger.info(f'Output writen to {args.output_file}')

    
