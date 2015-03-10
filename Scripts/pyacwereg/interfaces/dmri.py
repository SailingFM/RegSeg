#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Author: oesteban
# @Date:   2015-03-10 16:15:07
# @Last Modified by:   oesteban
# @Last Modified time: 2015-03-10 16:31:55

import os
import os.path as op
import nibabel as nb
import numpy as np

from nipype.interfaces.base import (BaseInterface, traits, TraitedSpec, File,
                                    InputMultiPath, OutputMultiPath,
                                    BaseInterfaceInputSpec, isdefined,
                                    DynamicTraitedSpec, Directory,
                                    CommandLine, CommandLineInputSpec)

from nipype import logging
iflogger = logging.getLogger('interface')


class PhaseUnwrapInputSpec(BaseInterfaceInputSpec):
    in_file = File(exists=True, mandatory=True,
                   desc='phase file to be unwrapped')
    in_mask = File(exists=True, desc='mask file')
    rescale = traits.Bool(True, usedefault=True,
                          desc='rescale range to 2*pi')
    out_file = File('unwrapped.nii.gz', usedefault=True,
                    desc='output file name')


class PhaseUnwrapOutputSpec(TraitedSpec):
    out_file = File(exists=True, desc='output file name')


class PhaseUnwrap(BaseInterface):

    """
    Unwraps a phase file
    """
    input_spec = PhaseUnwrapInputSpec
    output_spec = PhaseUnwrapOutputSpec

    def _run_interface(self, runtime):
        from unwrap import unwrap
        from math import pi
        im = nb.load(self.inputs.in_file)
        wrapped = im.get_data()

        if self.inputs.rescale:
            wrapped = wrapped - wrapped.min()
            wrapped = (2.0 * pi * wrapped) / wrapped.max()
            wrapped -= pi

        if isdefined(self.inputs.in_mask):
            msk = nb.load(self.inputs.in_mask)
            msk[msk > 0.0] = 1.0
            msk[msk < 1.0] = 0.0
            wrapped *= msk
            wrapped = np.ma.masked_equal(wrapped, 0)
        unw = unwrap(wrapped)

        hdr = im.get_header().copy()
        hdr.set_data_dtype(np.float32)
        nb.Nifti1Image(unw.astype(np.float32), im.get_affine(),
                       hdr).to_filename(op.abspath(self.inputs.out_file))
        return runtime

    def _list_outputs(self):
        outputs = self._outputs().get()
        outputs['out_file'] = op.abspath(self.inputs.out_file)
        return outputs
