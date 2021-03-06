// --------------------------------------------------------------------------------------
// File:          regseg_energytest.h
// Date:          Feb 12, 2015
// Author:        code@oscaresteban.es (Oscar Esteban)
// Version:       1.5.5
// License:       GPLv3 - 29 June 2007
// Short Summary:
// --------------------------------------------------------------------------------------
//
// Copyright (c) 2015, code@oscaresteban.es (Oscar Esteban)
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
// along with ACWEReg.  If not, see <http://www.gnu.org/licenses/>.
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

#ifndef SOURCE_DIRECTORY__APPLICATIONS_REGSEG_ENERGYTEST_HXX_
#define SOURCE_DIRECTORY__APPLICATIONS_REGSEG_ENERGYTEST_HXX_

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include <itkArray.h>
#include <itkVector.h>
#include <itkVectorImage.h>
#include <itkQuadEdgeMesh.h>
#include <itkVTKPolyDataReader.h>
#include <itkImageFileReader.h>
#include <itkOrientImageFilter.h>
#include <itkComposeImageFilter.h>

#include "DownsampleAveragingFilter.h"
#include "MultilabelBinarizeMeshFilter.h"
#include "rstkVTKPolyDataWriter.h"
#include "ComponentsFileWriter.h"
#include "EnergyCalculatorFilter.h"
#include "MahalanobisDistanceModel.h"

#include <jsoncpp/json/json.h>

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

const static unsigned int Dimension = 3u;

typedef double                                                    MeasureType;
typedef float                                                     ChannelPixelType;
typedef itk::Image<ChannelPixelType, Dimension>                   ChannelType;
typedef itk::VectorImage<ChannelPixelType, Dimension>             ReferenceImageType;
typedef typename ReferenceImageType::PixelType                    VectorPixelType;
typedef itk::ComposeImageFilter<ChannelType, ReferenceImageType>  InputToVectorFilterType;

typedef float                                                     PointValueType;
typedef itk::Vector< PointValueType, Dimension >                  VectorType;
typedef itk::QuadEdgeMesh< VectorType, Dimension >                VectorContourType;
typedef itk::VectorImage< PointValueType, Dimension >             ProbmapType;

typedef typename ReferenceImageType::Pointer                      ReferencePointer;
typedef typename ReferenceImageType::DirectionType                DirectionType;
typedef typename ReferenceImageType::SizeType                     SizeType;
typedef typename ReferenceImageType::PointType                    PointType;
typedef typename ReferenceImageType::SpacingType                  SpacingType;
typedef itk::ContinuousIndex<PointValueType, Dimension>           ContinuousIndex;


typedef rstk::MultilabelBinarizeMeshFilter< VectorContourType >   BinarizeMeshFilterType;
typedef typename BinarizeMeshFilterType::Pointer                  BinarizeMeshFilterPointer;
typedef typename BinarizeMeshFilterType::OutputImageType          BinarizationImageType;
typedef typename BinarizeMeshFilterType::InputMeshContainer       InputMeshContainer;
typedef typename BinarizeMeshFilterType::OutputComponentType      SegmentationType;

typedef rstk::DownsampleAveragingFilter
		< BinarizationImageType, ProbmapType >                    DownsampleFilter;
typedef typename DownsampleFilter::Pointer                        DownsamplePointer;

typedef itk::VTKPolyDataReader< VectorContourType >               ReaderType;
typedef rstk::VTKPolyDataWriter< VectorContourType >              WriterType;
typedef itk::ImageFileReader<ChannelType>                         ImageReader;
typedef itk::ImageFileWriter<SegmentationType>                    SegmentationWriter;
typedef rstk::ComponentsFileWriter<ProbmapType>                   ImageWriter;

typedef itk::OrientImageFilter< ReferenceImageType, ReferenceImageType >  Orienter;
typedef itk::OrientImageFilter< SegmentationType, SegmentationType >      SegmentationOrienter;
typedef itk::OrientImageFilter< ProbmapType, ProbmapType >                ProbmapsOrienter;
typedef itk::OrientImageFilter< ChannelType, ChannelType >                ChannelOrienter;

typedef rstk::MahalanobisDistanceModel< ReferenceImageType >      EnergyModelType;
typedef typename EnergyModelType::Pointer                         EnergyModelPointer;

typedef rstk::EnergyCalculatorFilter<ReferenceImageType>          EnergyFilter;
typedef typename EnergyFilter::Pointer                            EnergyFilterPointer;
typedef typename EnergyFilter::PriorsImageType                    PriorsImageType;
typedef itk::Array< MeasureType >                                 MeasureArray;
typedef typename PriorsImageType::Pointer                         PriorsImagePointer;
typedef typename PriorsImageType::PixelType                       PriorsPixelType;
typedef typename PriorsImageType::InternalPixelType               PriorsValueType;




int main(int argc, char *argv[]);


#endif /* SOURCE_DIRECTORY__APPLICATIONS_REGSEG_ENERGYTEST_HXX_ */
