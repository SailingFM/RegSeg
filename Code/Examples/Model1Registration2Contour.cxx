// --------------------------------------------------------------------------
// File:             Model1Registration2Contour.cxx
// Date:             12/12/2012
// Author:           code@oscaresteban.es (Oscar Esteban, OE)
// Version:          0.1
// License:          BSD
// --------------------------------------------------------------------------
//
// Copyright (c) 2012, code@oscaresteban.es (Oscar Esteban)
// with Signal Processing Lab 5, EPFL (LTS5-EPFL)
// and Biomedical Image Technology, UPM (BIT-UPM)
// All rights reserved.
// 
// This file is part of RegSeg
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the names of the LTS5-EFPL and the BIT-UPM, nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Oscar Esteban ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL OSCAR ESTEBAN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



#ifndef DATA_DIR
#define DATA_DIR "../Data/ModelGeneration/Model1/"
#endif

#include <itkVector.h>
#include <itkVectorImage.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkQuadEdgeMesh.h>
#include <itkVTKPolyDataReader.h>
#include <itkVTKPolyDataWriter.h>
#include <itkVectorImageToImageAdaptor.h>
#include <itkComposeImageFilter.h>
#include <itkVectorResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkDisplacementFieldTransform.h>
#include <itkResampleImageFilter.h>
#include "MahalanobisFunctional.h"
#include "GradientDescentFunctionalOptimizer.h"

using namespace rstk;

int main(int argc, char *argv[]) {
	typedef itk::Image<float, 3u>                                ChannelType;
	typedef itk::Vector<float, 2u>                               VectorPixelType;
	typedef itk::Image<VectorPixelType, 3u>                      ImageType;
	typedef itk::ComposeImageFilter< ChannelType,ImageType >     InputToVectorFilterType;

	typedef MahalanobisFunctional<ImageType>                      FunctionalType;
	typedef FunctionalType::ContourType                ContourType;
	typedef ContourType::Pointer                      ContourDisplacementFieldPointer;
	typedef FunctionalType::VectorType                            VectorType;
	typedef FunctionalType::MeanType                              MeanType;
	typedef FunctionalType::CovarianceType                        CovarianceType;
	typedef FunctionalType::DeformationFieldType                  DeformationFieldType;

	typedef GradientDescentFunctionalOptimizer< FunctionalType >   Optimizer;
	typedef typename Optimizer::Pointer                          OptimizerPointer;

	typedef itk::VTKPolyDataReader< ContourType >     ReaderType;
	typedef itk::VTKPolyDataWriter< ContourType >     WriterType;
	typedef itk::ImageFileReader<ChannelType>                      ImageReader;
	typedef itk::ImageFileWriter<ChannelType>                      ImageWriter;
	typedef itk::ImageFileWriter<DeformationFieldType>           DeformationWriter;

	typedef itk::VectorImageToImageAdaptor<double,3u>            VectorToImage;
	typedef itk::VectorResampleImageFilter
			<DeformationFieldType,DeformationFieldType,float>   DisplacementResamplerType;
	typedef itk::BSplineInterpolateImageFunction
			                <DeformationFieldType>               InterpolatorFunction;
	typedef itk::DisplacementFieldTransform<float, 3u>           TransformType;

	typedef itk::ResampleImageFilter<ChannelType,ChannelType,float>    ResamplerType;




	InputToVectorFilterType::Pointer comb = InputToVectorFilterType::New();

	ImageReader::Pointer r = ImageReader::New();
	r->SetFileName( std::string( DATA_DIR ) + "deformed2_FA.nii.gz" );
	r->Update();
	comb->SetInput(0,r->GetOutput());

	ImageReader::Pointer r2 = ImageReader::New();
	r2->SetFileName( std::string( DATA_DIR ) + "deformed2_MD.nii.gz" );
	r2->Update();
	comb->SetInput(1,r2->GetOutput());
	comb->Update();

	ChannelType::Pointer im = r->GetOutput();


	VectorType zero = itk::NumericTraits<VectorType>::Zero;

	ReaderType::Pointer polyDataReader1 = ReaderType::New();
	polyDataReader1->SetFileName( std::string( DATA_DIR ) + "wm_moved.vtk" );
	polyDataReader1->Update();
	ContourDisplacementFieldPointer initialContour1 = polyDataReader1->GetOutput();

	typename ContourType::PointsContainerPointer points1 = initialContour1->GetPoints();
	typename ContourType::PointsContainerIterator u_it1 = points1->Begin();

	while( u_it1 != points1->End() ) {
		initialContour1->SetPointData( u_it1.Index(),zero);
		++u_it1;
	}

	ReaderType::Pointer polyDataReader2 = ReaderType::New();
	polyDataReader2->SetFileName( std::string( DATA_DIR ) + "csf_moved.vtk" );
	polyDataReader2->Update();
	ContourDisplacementFieldPointer initialContour2 = polyDataReader2->GetOutput();

	typename ContourType::PointsContainerPointer points2 = initialContour2->GetPoints();
	typename ContourType::PointsContainerIterator u_it2 = points2->Begin();

	while( u_it2 != points2->End() ) {
		initialContour2->SetPointData( u_it2.Index(),zero);
		++u_it2;
	}

	// Initialize tissue signatures
	MeanType mean1; // This is GM
	mean1[0] = 0.11941234;
	mean1[2] = 0.00089523;
	CovarianceType cov1;
	cov1(0,0) =  5.90117156e-04;
	cov1(0,1) = -1.43226633e-06;
	cov1(1,0) = -1.43226633e-06;
	cov1(1,1) =  1.03718252e-08;
	MeanType mean2; // This is WM
	mean2[0] = 7.77335644e-01;
	mean2[1] = 6.94673450e-04;
	CovarianceType cov2;
	cov2(0,0) =  4.85065832e-03;
	cov2(0,1) = -6.89610616e-06;
	cov2(1,0) = -6.89610616e-06;
	cov2(1,1) =  1.02706528e-08;

	MeanType mean3; // This is CSF
	mean3[0] = 0.10283652;
	mean3[1] = 0.00298646;
	CovarianceType cov3;
	cov3(0,0) =  1.19084185e-03;
	cov3(0,1) =  2.22414814e-07;
	cov3(1,0) =  2.22414814e-07;
	cov3(1,1) =  1.56537816e-08;

	typename FunctionalType::ParametersType params1;
	params1.mean[0] = mean2;
	params1.mean[1] = mean1;
	params1.iCovariance[0] = cov2;
	params1.iCovariance[1] = cov1;

	typename FunctionalType::ParametersType params2;
	params2.mean[0] = mean3;
	params2.mean[1] = mean2;
	params2.iCovariance[0] = cov3;
	params2.iCovariance[1] = cov2;

	// Initialize deformation field
	DeformationFieldType::Pointer df = DeformationFieldType::New();
	DeformationFieldType::SizeType imSize = im->GetLargestPossibleRegion().GetSize();
	DeformationFieldType::SizeType size; size.Fill(32);
	DeformationFieldType::SpacingType spacing = im->GetSpacing();
	for( size_t i = 0; i<3; i++) spacing[i] = 1.0*imSize[i]/size[i];
	df->SetRegions( size );
	df->SetSpacing( spacing );
	df->SetDirection( im->GetDirection() );
	df->Allocate();
	df->FillBuffer( itk::NumericTraits<DeformationFieldType::PixelType>::Zero );

	// Initialize LevelSet function
	FunctionalType::Pointer ls = FunctionalType::New();
	ls->SetReferenceImage( comb->GetOutput() );
	ls->AddShapePrior( initialContour1, params1 );
	ls->AddShapePrior( initialContour2, params2 );

	// Connect Optimizer
	OptimizerPointer opt = Optimizer::New();
	opt->SetFunctionalFunction( ls );
	opt->SetNumberOfIterations(5000);
	//opt->SetAlpha( 1e-3 );
	//opt->SetBeta( 1e-3 );
	opt->SetStepSize( 1 );

	// Start
	opt->Start();

	// Write final result out
	WriterType::Pointer polyDataWriter = WriterType::New();
	polyDataWriter->SetInput( ls->GetCurrentContourPosition()[0] );
	polyDataWriter->SetFileName( "deformed2-wm.vtk" );
	polyDataWriter->Update();

	// Write final result out
	WriterType::Pointer polyDataWriter2 = WriterType::New();
	polyDataWriter2->SetInput( ls->GetCurrentContourPosition()[1] );
	polyDataWriter2->SetFileName( "deformed2-csf.vtk" );
	polyDataWriter2->Update();

//	DeformationWriter::Pointer w = DeformationWriter::New();
//	w->SetInput( opt->GetDeformationField() );
//	w->SetFileName( "deformed2_field.nii.gz" );
//	w->Update();

//	DisplacementResamplerType::Pointer p = DisplacementResamplerType::New();
//	p->SetInput( opt->GetDeformationField() );
//	p->SetOutputOrigin( im->GetOrigin() );
//	p->SetOutputSpacing( im->GetSpacing() );
//	p->SetOutputDirection( im->GetDirection() );
//	p->SetSize( im->GetLargestPossibleRegion().GetSize() );
//	//p->SetInterpolator( InterpolatorFunction::New() );
//	p->Update();
//	DeformationWriter::Pointer w2 = DeformationWriter::New();
//	w2->SetInput( p->GetOutput() );
//	w2->SetFileName( "deformed2_fieldHD.nii.gz" );
//	w2->Update();
/*
	DeformationFieldType::Pointer dfield = DeformationFieldType::New();
	TransformType::Pointer tf = TransformType::New();
	tf->SetDisplacementField( dfield );

	ResamplerType::Pointer res = ResamplerType::New();
	res->SetInput( im );
	res->SetReferenceImage( im );
	res->SetTransform( tf );
	res->UseReferenceImageOn();
	res->Update();

	ImageWriter::Pointer w3 = ImageWriter::New();
	w3->SetInput( res->GetOutput() );
	w3->SetFileName( "FAunwarped.nii.gz" );
	w3->Update();*/
}



