# coding: utf-8
# emacs: -*- mode: python; py-indent-offset: 4; indent-tabs-mode: nil -*-
# vi: set ft=python sts=4 ts=4 sw=4 et:

import os
import os.path as op

import nipype.interfaces.io as nio              # Data i/o
import nipype.interfaces.utility as niu         # utility
import nipype.pipeline.engine as pe             # pypeline engine
import nipype.interfaces.fsl as fsl             # fsl
import nipype.interfaces.freesurfer as fs       # freesurfer
import nipype.interfaces.ants as ants           # ANTS
import nipype.pipeline.engine as pe

from smri import fieldmap_preparation

def isbi_workflow( name='ISBI2014' ):
    workflow = pe.Workflow(name=name)

    # Setup i/o
    inputnode = pe.Node( niu.IdentityInterface( fields=[ 'subject_id', 'in_fmap_mag', 'in_fmap_pha', 'in_t1w_brain', 'in_t2w', 'fs_subjects_dir','te_incr', 'echospacing','enc_dir' ]), name='inputnode' )
    outputnode = pe.Node(niu.IdentityInterface(fields=['out_file', 'out_vsm', 'out_mask', 'out_tpms' ]), name='outputnode' )
    
    # Setup internal workflows
    prepare = fieldmap_preparation()
    distort = distortion_workflow()


    workflow.connect( [
                         ( inputnode,  prepare, [ ('subject_id','inputnode.subject_id'),('in_fmap_mag','inputnode.in_fmap_mag'),
                                                  ('in_fmap_pha','inputnode.in_fmap_pha'),('in_t1w_brain','inputnode.in_t1w_brain'),
                                                  ('in_t2w','inputnode.in_t2w'),('fs_subjects_dir','inputnode.fs_subjects_dir') ])
                        ,( inputnode,  distort, [ ('te_incr','inputnode.te_incr'),('echospacing','inputnode.echospacing'),('enc_dir','inputnode.enc_dir') ])
                        ,( prepare,    distort, [ ('outputnode.out_smri','inputnode.in_file'),('outputnode.out_fmap_pha','inputnode.in_pha'),
                                                  ('outputnode.out_fmap_mag','inputnode.in_mag'),('outputnode.out_mask','inputnode.in_mask'),
                                                  ('outputnode.out_tpms','inputnode.in_tpms') ])
                        ,( distort, outputnode, [ ('outputnode.out_file','out_file'),('outputnode.out_vsm','out_vsm'),
                                                  ('outputnode.out_mask','out_mask'),('outputnode.out_tpms','out_tpms') ])
                    ])

    return workflow 


def distortion_workflow(name="synthetic_distortion", nocheck=False ):
    pipeline = pe.Workflow(name=name)
    
    inputnode = pe.Node(niu.IdentityInterface(fields=['in_file', 'in_mag', 'in_pha', 'in_mask', 'te_incr', 'echospacing','enc_dir','in_tpms' ]), name='inputnode' )
    prepare = pe.Node( fsl.PrepareFieldmap(nocheck=nocheck), name='fsl_prepare_fieldmap' )
    vsm = pe.Node(fsl.FUGUE(save_shift=True,poly_order=2 ), name="gen_VSM")
    dm = pe.Node( niu.Function(input_names=['in_file','in_mask'],output_names=['out_file'], function=demean ), name='demean' )
    applyWarp = fugue_all_workflow()
    vsm_fwd_mask = pe.Node(fsl.FUGUE(forward_warping=True), name='Fugue_WarpMask')
    binarize = pe.Node( fs.Binarize( min=0.75 ), name="binarize" )
    warpimages = pe.MapNode( fsl.FUGUE(forward_warping=True,icorr=True), iterfield=['in_file'], name='WarpImages' )
    normalize_tpms = pe.Node( niu.Function( input_names=['in_files','in_mask'], output_names=['out_files'], function=normalize ), name='Normalize' )
    outputnode = pe.Node(niu.IdentityInterface(fields=['out_file', 'out_vsm', 'out_mask', 'out_tpms' ]), name='outputnode' )
    
    pipeline.connect([
                       (inputnode,      prepare, [('in_mag','in_magnitude'), ('in_pha','in_phase'),(('te_incr',_sec2ms),'delta_TE')])
                      ,(prepare,            vsm, [('out_fieldmap','phasemap_file')])
                      ,(inputnode,          vsm, [('in_mag','in_file'),('in_mask','mask_file'),('te_incr','asym_se_time'),('echospacing','dwell_time'),('enc_dir','unwarp_direction') ])
                      ,(vsm,                 dm, [('shift_out_file', 'in_file')])
                      ,(inputnode,           dm, [('in_mask','in_mask')])
                      ,(inputnode, vsm_fwd_mask, [('in_mask','in_file'),('in_mask','mask_file'),('enc_dir','unwarp_direction') ])
                      ,(dm,        vsm_fwd_mask, [('out_file','shift_in_file')])
                      ,(dm,           applyWarp, [('out_file', 'inputnode.in_vsm')])
                      ,(inputnode,    applyWarp, [('in_file', 'inputnode.in_file'),('in_mask', 'inputnode.in_mask'),('enc_dir','inputnode.unwarp_direction') ])
                      ,(dm,          outputnode, [('out_file', 'out_vsm')])
                      ,(applyWarp,   outputnode, [('outputnode.out_file', 'out_file')])
                      #,(applyWarp,    fixsignal, [('outputnode.out_file', 'in_distorted')])
                      #,(inputnode,    fixsignal, [('in_file','in_reference')])
                      #,(vsm_fwd_mask, fixsignal, [('warped_file','in_mask')])
                      #,(fixsignal,   outputnode, [('out_distorted','out_file')])
                      ,(vsm_fwd_mask,  binarize, [('warped_file','in_file')])
                      ,(binarize,    outputnode, [('binary_file','out_mask')])
                      ,(inputnode,  warpimages, [('in_tpms','in_file'),('in_mask', 'mask_file'),('enc_dir','unwarp_direction') ])
                      ,(dm      ,   warpimages, [('out_file','shift_in_file') ])
                      ,(binarize,    normalize_tpms, [('binary_file','in_mask')])
                      ,(warpimages,  normalize_tpms, [('warped_file','in_files')])
                      ,(normalize_tpms,  outputnode, [('out_files','out_tpms')])
    ])
    
    return pipeline

#
# AUXILIARY FUGUE ALL WORKFLOW ---------------------------------------------------------------------------------
#

def fugue_all_workflow(name="Fugue_WarpDWIs"):
    pipeline = pe.Workflow(name=name)
    
    def _split_dwi(in_file):
        import nibabel as nib
        import os.path as op
        out_files = []
        frames = nib.funcs.four_to_three(nib.load(in_file))
        name, fext = op.splitext(op.basename(in_file))
        if fext == '.gz':
            name, _ = op.splitext(name)
        for i, frame in enumerate(frames):
            out_file = op.abspath('./%s_%03d.nii.gz' % (name, i))
            nib.save(frame, out_file)
            out_files.append(out_file)
        return out_files
    
    inputnode = pe.Node(niu.IdentityInterface(fields=['in_file', 'in_mask', 'in_vsm','unwarp_direction' ]), name='inputnode' )
    dwi_split = pe.Node(niu.Function(input_names=['in_file'], output_names=['out_files'], function=_split_dwi), name='split_DWI')
    vsm_fwd = pe.MapNode(fsl.FUGUE(forward_warping=True, icorr=False), iterfield=['in_file'], name='Fugue_Warp')
    dwi_merge = pe.Node(fsl.utils.Merge(dimension='t'), name='merge_DWI')
    
    outputnode = pe.Node(niu.IdentityInterface(fields=['out_file' ]), name='outputnode' )
    pipeline.connect([
                       (inputnode,  dwi_split, [('in_file', 'in_file')])
                      ,(dwi_split,    vsm_fwd, [('out_files', 'in_file')])
                      ,(inputnode,    vsm_fwd, [('in_mask', 'mask_file'),('in_vsm','shift_in_file'),('unwarp_direction','unwarp_direction') ])
                      ,(vsm_fwd,    dwi_merge, [('warped_file', 'in_files')])
                      ,(dwi_merge, outputnode, [('merged_file', 'out_file')])
                    ])    
    
    return pipeline




#
# HELPER FUNCTIONS ------------------------------------------------------------------------------------
#

def get_vox_size( in_file ):
    import nibabel as nib
    pixdim = nib.load( in_file ).get_header()['pixdim']
    return (pixdim[1],pixdim[2],pixdim[3])

def normalize( in_files, in_mask=None, out_files=[] ):
    import nibabel as nib
    import numpy as np
    import os.path as op
 
    imgs = [ nib.load(fim) for fim in in_files ]
    img_data = np.array( [ im.get_data() for im in imgs ] ).astype( 'f32' )
    img_data[img_data>1.0] = 1.0
    img_data[img_data<0.0] = 0.0
    weights = np.sum( img_data, axis=0 )

    img_data[0][weights==0] = 1.0
    weights[weights==0] = 1.0

    msk = np.ones_like( imgs[0].get_data() )
    
    if not in_mask is None:
        msk = nib.load( in_mask ).get_data()
        msk[ msk<=0 ] = 0
        msk[ msk>0 ] = 1


    if len( out_files )==0:    
        for i,finname in enumerate( in_files ):
            fname,fext = op.splitext( op.basename( finname ) )
            if fext == '.gz':
                fname,fext2 = op.splitext( fname )
                fext = fext2 + fext
             
            out_file = op.abspath( fname+'_norm'+fext )
            out_files+= [ out_file ]


    for i,out_file in enumerate( out_files ):
            data = img_data[i] / weights
            data = data * msk
            hdr = imgs[i].get_header().copy()
            hdr['data_type']= 16
            hdr.set_data_dtype( 'float32' )   
            nib.save( nib.Nifti1Image( data, imgs[i].get_affine(), hdr ), out_file )

    return out_files


def sanitize( in_reference, in_distorted, in_mask, out_file=None ):
    import nibabel as nib
    import numpy as np
    import os.path as op
    import scipy.ndimage as ndimage

    inc = 7e-2
    msk_data = nib.load( in_mask ).get_data()
    msk_data[msk_data<(1.0-inc)] = 0
    msk_data[msk_data>(1.0+inc)] = 0
    msk_data[msk_data!=0] = 1.0
    msk_data = ndimage.binary_opening( msk_data ).astype( 'uint8' )
    im = nib.load( in_distorted )
    im_data = im.get_data()
    ref_data = nib.load( in_reference ).get_data()
        
    for i in range(0,np.shape(im_data)[-1]):
        im_data[:,:,:,i] = ref_data[:,:,:,i] * msk_data + im_data[:,:,:,i] * (1-msk_data) 
       
    if out_file is None:
        fname, fext = op.splitext(op.basename(in_distorted))
        if fext == '.gz':
            fname, _ = op.splitext(fname)
        out_file = op.abspath('./%s_fixed.nii.gz' % fname)
    nib.save( nib.Nifti1Image( im_data, im.get_affine(), im.get_header() ), out_file )
    return out_file


def generate_siemens_phmap( in_file, intensity, sigma, w1=-0.5, w2=1.0, out_file=None ):
    import nibabel as nib
    from scipy.ndimage import (binary_erosion,binary_dilation)
    from scipy.ndimage.filters import gaussian_filter
    import numpy as np
    import os.path as op
    import math

    if out_file is None:
        fname, fext = op.splitext(op.basename(in_file))
        if fext == '.gz':
            fname, _ = op.splitext(fname)
        out_file = op.abspath('./%s_siemens.nii.gz' % fname)


    msk = nib.load( in_file )
    mskdata = msk.get_data()
    imshape = msk.get_shape()
    boundary = binary_erosion( mskdata ).astype(np.dtype('u1'))
    boundary = (( mskdata - boundary )).astype(float)
    
    x = ( int(0.65*imshape[0]),int(0.35*imshape[0]) )
    y = ( int(0.4*imshape[1]), int(0.6*imshape[1]) )
    z = ( int(0.4*imshape[2]), int( imshape[2] ) )

    slice1 = np.zeros( shape=imshape )
    slice1[x[0],y[0]:y[1],z[0]:z[1]]  = 1
    slice1 = binary_dilation( slice1 * boundary ).astype( np.float32 )

    slice2 = np.zeros( shape=imshape )
    slice2[x[1],y[0]:y[1],z[0]:z[1]]  = 1
    slice2 = binary_dilation( slice2 * boundary ).astype( np.float32 )
    distortionfront = intensity * ( slice1 * w1 + slice2 * w2 )

    phasefield = gaussian_filter( distortionfront, sigma=sigma )
   # phasefield = np.ma.array( phasefield, mask=1-mskdata )
   # median = np.ma.median( phasefield )
   # phasefield = phasefield - median

   # maxvalue = np.ma.max( phasefield.reshape(-1) )
   # if np.fabs( np.ma.min(phasefield.reshape(-1) ) )>maxvalue:
   #     maxvalue =np.fabs( np.ma.min(phasefield.reshape(-1) ) )

   # normalizer = 0.001 / maxvalue
    normalizer = 1.0

   # noisesrc = 2.0 * np.random.random_sample( size=np.shape(phasefield) ) - 1.0
   # noisesrc[mskdata>0] = 0.0
   # phasefield[mskdata==0] = 0.0
   # phasefield= phasefield * normalizer + noisesrc
    phasefield = phasefield * 2047.5

    hdr = msk.get_header()
    hdr['data_type'] = 16
    hdr['qform_code'] = 2
    hdr.set_data_dtype( np.float32 )
    nib.save( nib.Nifti1Image( phasefield.astype( np.float32 ), msk.get_affine(), hdr ), out_file ) 
    return out_file


def generate_phmap( in_file, out_file=None ):
    import nibabel as nib
    from scipy.ndimage import binary_erosion
    from scipy.ndimage.filters import gaussian_filter
    import numpy as np
    import os.path as op
    import math

    if out_file is None:
        fname, fext = op.splitext(op.basename(in_file))
        if fext == '.gz':
            fname, _ = op.splitext(fname)
        out_file = op.abspath('./%s_ph_unwrap_rads.nii.gz' % fname)

    msk = nib.load( in_file )
    data = msk.get_data().astype(float)
    data = np.pi * data * (1.0/2048)
    hdr = msk.get_header()
    hdr.set_data_dtype( 16 )
    nib.save( nib.Nifti1Image( data, msk.get_affine(), hdr ), out_file ) 
    return out_file
    
def rad2radsec( in_file, in_mask, te_incr=2.46e-3, out_file=None ):
    import nibabel as nib
    import numpy as np
    import os.path as op
    
    im = nib.load( in_file )
    msk = nib.load( in_mask ).get_data()
    data = np.ma.array( im.get_data(), mask = 1-msk )
    data[msk==1] = data[msk==1] / ( te_incr )
    mval = np.ma.median( data )
    
    data1 = np.zeros( shape=im.get_shape() )
    data2 = np.zeros( shape=im.get_shape() )
    
    data1[msk==1] = data[msk==1] - mval
    
    data = np.rollaxis ( np.array( [ data1, data2 ] ), 0, 4)    
    hdr = im.get_header()
    hdr['dim'][0]=4
    hdr['dim'][4]=2
    
    if out_file is None:
        fname, fext = op.splitext(op.basename(in_file))
        if fext == '.gz':
            fname, _ = op.splitext(fname)
        out_file = op.abspath('./%s_ph_unwrap_radsec.nii.gz' % fname)
        
    nib.save( nib.Nifti1Image( data, im.get_affine(), hdr ), out_file )
    
    return out_file

def genmask(in_file, out_file=None):
    import numpy as np
    import nibabel as nib
    import os.path as op
    
    im = nib.load(in_file)
    
    data = np.ones( shape=im.get_shape() )
    
    if out_file is None:
        fname, fext = op.splitext(op.basename(in_file))
        if fext == '.gz':
            fname, _ = op.splitext(fname)
        out_file = op.abspath('./%s_mask.nii.gz' % fname)
        
    nib.save( nib.Nifti1Image( data, im.get_affine(), im.get_header() ), out_file )
    
    return out_file

def demean( in_file, in_mask, out_file=None ):
    import numpy as np
    import nibabel as nib
    import os.path as op
    
    im = nib.load( in_file )
    msk = nib.load( in_mask ).get_data()
    
    data = np.ma.array( im.get_data(), mask = 1-msk )
    mval = np.ma.median( data )
    data[msk==1] = data[msk==1] - mval
    
    if out_file is None:
        fname, fext = op.splitext(op.basename(in_file))
        if fext == '.gz':
            fname, _ = op.splitext(fname)
        out_file = op.abspath('./%s_demeaned.nii.gz' % fname)
        
    nib.save( nib.Nifti1Image( data, im.get_affine(), im.get_header() ), out_file )
    
    return out_file

def _ms2sec(val):
    return val*1e-3

def _sec2ms(val):
    return val*1e3

def _aslist( val ):
    return [ val ]

def _pickupWarp( val ):
    return val[0]


def _split_dwi(in_file):
    import nibabel as nib
    import os
    out_files = []
    frames = nib.funcs.four_to_three(nib.load(in_file))
    name, fext = os.path.splitext(os.path.basename(in_file))
    if fext == '.gz':
        name, _ = os.path.splitext(name)
    for i, frame in enumerate(frames):
        out_file = os.path.abspath('./%s_%03d.nii.gz' % (name, i))
        nib.save(frame, out_file)
        out_files.append(out_file)
    return out_files
