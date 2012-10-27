// --------------------------------------------------------------------------
// File:             MahalanobisLevelSets.h
// Date:             27/10/2012
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
// This file is part of ACWERegistration-Debug@Debug
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

#ifndef MAHALANOBISLEVELSETS_H_
#define MAHALANOBISLEVELSETS_H_

// Include headers
#include <itkObject.h>
#include <itkArray.h>
#include "LevelSetsBase.h"

// Namespace declaration
namespace rstk {
/** \class MahalanobisLevelSets
 *  \brief This class implements a LevelSets function for registration based on
 *         Mahalanobis distance
 *
 *  MahalanobisLevelSets provides the implementation for using the Mahalanobis
 *  distance as distance function in the LevelSets environment
 *
 *  \ingroup
 */
template <class TTargetImage, class TDeformationField, class TContourDeformation, int NCONTOURS=2 >
class MahalanobisLevelSets: public LevelSetsBase< TDeformationField, TContourDeformation > {
public:
	typedef MahalanobisLevelSets                         Self;
	typedef LevelSetsBase
			< TDeformationField, TContourDeformation >   Superclass;
	typedef itk::SmartPointer<Self>                      Pointer;
	typedef itk::SmartPointer<const Self>                ConstPointer;

	/** Run-time type information (and related methods). */
	itkTypeMacro( MahalanobisLevelSets, LevelSetsBase );
	itkNewMacro( Self );

	typedef typename Superclass::ValueType                 ValueType;
	typedef typename Superclass::DeformationFieldType      DeformationFieldType;
	typedef typename Superclass::DeformationFieldPointer   DeformationFieldPointer;
	typedef typename Superclass::ContourDeformationType    ContourDeformationType;
	typedef typename Superclass::ContourDeformationPointer ContourDeformationPointer;

	typedef TTargetImage                                   ImageType;
	typedef typename ImageType::Pointer                    ImagePointer;
	typedef typename ImageType::ConstPointer               ImageConstPointer;

	typedef itk::Array< double >                           MeansType;
	typedef itk::Matrix< double >                          CovarianceType;

	ValueType GetValue() const;
	void GetLevelSetsMap( ContourDeformationType & contourDeformation, DeformationFieldType & targetDeformation) const;

	itkSetObjectMacro(Image, ImageType);
	itkGetConstObjectMacro(Image, ImageType);

protected:
	MahalanobisLevelSets();
	~MahalanobisLevelSets() {}

	void PrintSelf( std::ostream& os, itk::Indent indent) const;

	ImageConstPointer m_Image;

private:
	MahalanobisLevelSets( const Self &); // purposely not implemented
	void operator=(const Self &); // purposely not implemented
}; // End of class MahalanobisLevelSets
} // End of namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "MahalanobisLevelSets.hxx"
#endif

#endif /* MAHALANOBISLEVELSETS_H_ */
