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
File: python/PhzExecutables/ExtractZList.py

Created on: 2024/02/19
Author: fdubath

"""
import os.path
import sys
import argparse
from copy import copy, deepcopy

import ElementsKernel.Logging as log
import numpy as np
from astropy.table import Table, Column
from astropy.io import fits
from os.path import exists
from os.path import join

logger = log.getLogger('ExtractZList')


def defineSpecificProgramOptions():
    parser = argparse.ArgumentParser()

    parser.add_argument('-ic','--input_catalog', type=str, required=True,
                        help='Path to the input catalog file')
    parser.add_argument('-zc','--ref_z_col', type=str, default='Z_TRUE',
                        help='Name of the column containing the reference redshift (default = Z_TRUE)')
    parser.add_argument('-zf','--min_z_step', type=float, default=0.0001,
                        help='minimal redshift step (default = 0.0001)')
    parser.add_argument('-zt','--scale_min_z_step', type=bool,
                        help='If set multiply the minimal redshift step by (1+z)')
    parser.add_argument('-sf','--skip_n_first', type=int, default='0',
                        help='Skip n first entry of the catalog (default n=0)')
    parser.add_argument('-mr','--max_record', type=int, default='-1',
                        help='If>0 limit the number of object taken into account (default n=-1)')
    parser.add_argument('-of','--output_file', type=str, default='',
                        help='Path to the fits file containing the redshift list')

    return parser

#--------------------------

def extract_sub_sample(zs, first, max_number):
    zs_l = zs[first:]
    if max_number>0:
       zs_l=zs_l[0:max_number]
    return zs_l

def group_z(zs, min_step, do_scale_step=False):
    zs_extracted = {}
    for z in zs:
        if np.isfinite(z):
           found = False
           for z_know in zs_extracted.keys():
               if np.isclose(z, z_know, atol=min_step*(1+do_scale_step*z_know)):
                   zs_extracted[z_know].append(z)
                   foud=True
                   break
           if not found:
               zs_extracted[z]=[z]
    return zs_extracted

def get_out_table(zs_extracted):
    z_mean = np.sort([np.mean(zs_extracted[z_know]) for z_know in zs_extracted.keys()])
    out_t = Table()
    out_t['Z']=z_mean
    return out_t

#--------------------------
def mainMethod(args):

    zs = extract_sub_sample(Table.read(args.input_catalog)[args.ref_z_col], args.skip_n_first, args.max_record)
    logger.info(f'Read {len(zs)} records from file {args.input_catalog} starting at record {args.skip_n_first}')

    zs_extracted = group_z(zs, args.min_z_step)
    logger.info(f'Found {len(zs_extracted)} distinct zs (with tolerence {args.min_z_step}')

    out_t = get_out_table(zs_extracted, args.scale_min_z_step)
    out_t.write(args.output_file, overwrite=True)
    logger.info(f'Output writen to {args.output_file}')
