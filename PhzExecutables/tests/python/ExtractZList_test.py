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
File: tests/python/ExtractZList_test.py

Created on: 2024/02/19
Author: fdubath
"""

import sys

import math
import json

import os
from ElementsKernel.Temporary import TempFile
from ElementsKernel.Temporary import TempDir
from astropy.table import Table
from astropy.io import fits

import PhzExecutables.ExtractZList as worker

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
            assert '--input_catalog' in help_txt
            assert '--ref_z_col' in help_txt
            assert '--min_z_step' in help_txt
            assert '--skip_n_first' in help_txt
            assert '--max_record' in help_txt
            assert '--output_file' in help_txt

        args = parser.parse_args(['--input_catalog','test_path'])
        #check the default values
        assert args.input_catalog == 'test_path'
        assert args.ref_z_col == 'Z_TRUE'
        assert args.min_z_step == 0.0001
        assert args.skip_n_first == 0
        assert args.max_record == -1
        assert args.output_file == ''

    def test_extract_sub_sample(self):
        # given
        zs = np.array(range(601))/100
        first=0
        max_number=-1
        # when
        sub_zs = worker.extract_sub_sample(zs, first, max_number)
        # then
        assert np.array_equal(zs, sub_zs)

        # given
        first=10
        max_number=-1
        # when
        sub_zs = worker.extract_sub_sample(zs, first, max_number)
        # then
        assert np.array_equal(zs[10:], sub_zs)

        # given
        first=0
        max_number=10
        # when
        sub_zs = worker.extract_sub_sample(zs, first, max_number)
        # then
        assert np.array_equal(zs[0:10], sub_zs)

        # given
        first=0
        max_number=1000
        # when
        sub_zs = worker.extract_sub_sample(zs, first, max_number)
        # then
        assert np.array_equal(zs, sub_zs)

    def group_z_test(self):
        zs = [1,2,3,4,5,6,7,8,9]
        step = 0.5
        extracted = worker.group_z(zs, step)
        assert len(extracted)==len(zs)
        for z in zs:
            assert z in extracted.keys()
            assert len(extracted[z])==1
            assert extracted[z][0]==z

        zs = [1,2,10,11,20,21,11,10]
        step = 5
        extracted = worker.group_z(zs, step)
        assert len(extracted)==3
        for z in [1,10,20]:
            assert z in extracted.keys()
        assert len(extracted[1])==2
        assert len(extracted[10])==4
        assert len(extracted[20])==2

    def get_out_table_test(self):
        extracted = {10:[9,10,11], 1:[1,2,3]}
        t =worker. get_out_table(extracted)
        assert 'Z' in t.colnames
        assert len(t.colnames)==0
        assert len(t)==2
        assert t[0]==2
        assert t[1]==10
