// --------------------------------------------------------------------------------------
// File:          bspline_invert.h
// Date:          Apr 21, 2014
// Author:        code@oscaresteban.es (Oscar Esteban)
// Version:       1.5.5
// License:       GPLv3 - 29 June 2007
// Short Summary:
// --------------------------------------------------------------------------------------
//
// Copyright (c) 2014, code@oscaresteban.es (Oscar Esteban)
// with Signal Processing Lab 5, EPFL (LTS5-EPFL)
// and Biomedical Image Technology, UPM (BIT-UPM)
// All rights reserved.
//
// This file is part of RegSeg
//
// RegSeg is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RegSeg is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RegSeg.  If not, see <http://www.gnu.org/licenses/>.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef BSPLINE_INVERT_H_
#define BSPLINE_INVERT_H_


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkWarpImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>

#include <itkMaskImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkThresholdImageFilter.h>

#include <itkDisplacementFieldTransform.h>
#include <itkMesh.h>
#include <itkVTKPolyDataReader.h>
#include "rstkVTKPolyDataWriter.h"
#include "DisplacementFieldFileWriter.h"
#include "BSplineSparseMatrixTransform.h"

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

static const unsigned int DIMENSION = 3;
static const unsigned int NUM_THREADS = 4;

typedef itk::Image<float, DIMENSION>                         ChannelType;
typedef typename ChannelType::Pointer                        ChannelPointer;
typedef itk::ImageFileReader<ChannelType>                    ReaderType;
typedef typename ReaderType::Pointer                         ReaderPointer;
typedef itk::ImageFileWriter<ChannelType>                    WriterType;
typedef typename WriterType::Pointer                         WriterPointer;


typedef itk::Image< unsigned char, DIMENSION >               MaskType;
typedef typename MaskType::Pointer                           MaskPointer;
typedef itk::ImageFileWriter<MaskType>                       MaskWriter;
typedef itk::BinaryThresholdImageFilter
		                           < ChannelType, MaskType > Binarize;


typedef float                                                ScalarType;
typedef rstk::BSplineSparseMatrixTransform
		                          < ScalarType, DIMENSION>   Transform;
typedef Transform::Pointer                                   TPointer;
typedef typename Transform::CoefficientsImageType            CoefficientsType;
typedef typename Transform::CoefficientsImageArray           CoefficientsImageArray;
typedef typename Transform::FieldType                        FieldType;
typedef typename Transform::VectorType                       VectorType;

typedef itk::ImageFileReader< CoefficientsType >             CoeffReaderType;
typedef typename CoeffReaderType::Pointer				     CoeffReaderPointer;

typedef typename FieldType::Pointer              			 DisplacementFieldPointer;
typedef itk::ImageFileReader<FieldType>          			 DisplacementFieldReaderType;
typedef typename DisplacementFieldReaderType::Pointer        DisplacementFieldReaderPointer;

typedef itk::MaskImageFilter< ChannelType, MaskType, ChannelType > MaskFilter;

typedef itk::ResampleImageFilter< ChannelType, ChannelType, ScalarType >    ResampleFilter;
typedef typename ResampleFilter::Pointer                       ResamplePointer;
typedef itk::BSplineInterpolateImageFunction< ChannelType, ScalarType >    ResampleInterpolator;

typedef itk::ResampleImageFilter
		                 < MaskType, MaskType, ScalarType >    ResampleMaskFilter;
typedef typename ResampleMaskFilter::Pointer                   ResampleMaskPointer;
typedef itk::NearestNeighborInterpolateImageFunction< MaskType, ScalarType >    ResampleMaskInterpolator;

typedef itk::WarpImageFilter
	    < MaskType, MaskType, FieldType >                      WarpMaskFilter;
typedef typename WarpMaskFilter::Pointer                       WarpMaskFilterPointer;
typedef itk::NearestNeighborInterpolateImageFunction< MaskType >    WarpMaskInterpolator;

typedef itk::WarpImageFilter
	    < ChannelType, ChannelType, FieldType >                WarpFilter;
typedef typename WarpFilter::Pointer                           WarpFilterPointer;
typedef itk::BSplineInterpolateImageFunction< ChannelType >    WarpInterpolator;

typedef itk::ThresholdImageFilter< ChannelType >               ThresholdFilter;
typedef typename ThresholdFilter::Pointer                      ThresholdPointer;

typedef typename itk::DisplacementFieldTransform
		                           < ScalarType, DIMENSION>    TransformType;
typedef typename TransformType::Pointer                        TransformPointer;

typedef itk::Mesh< float, DIMENSION >                          MeshType;
typedef typename MeshType::Pointer                             MeshPointer;
typedef typename MeshType::PointType                           MeshPointType;
typedef typename MeshType::PointsContainer::Iterator           PointsIterator;
typedef itk::VTKPolyDataReader<MeshType>                       MeshReaderType;
typedef typename MeshReaderType::Pointer                       MeshReaderPointer;
typedef rstk::VTKPolyDataWriter<MeshType>                      MeshWriterType;
typedef typename MeshWriterType::Pointer                       MeshWriterPointer;


typedef rstk::DisplacementFieldFileWriter< FieldType >         FieldWriter;

int main(int argc, char *argv[]);


#endif /* BSPLINE_INVERT_H_ */
