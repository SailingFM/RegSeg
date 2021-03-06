// --------------------------------------------------------------------------------------
// File:          UniformMembershipFunction.h
// Date:          Jan 10, 2015
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
// This file is part of ACWEReg
//
// ACWEReg is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ACWEReg is distributed in the hope that it will be useful,
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

#ifndef SOURCE_DIRECTORY__MODULES_FUNCTIONAL_INCLUDE_UNIFORMMEMBERSHIPFUNCTION_H_
#define SOURCE_DIRECTORY__MODULES_FUNCTIONAL_INCLUDE_UNIFORMMEMBERSHIPFUNCTION_H_

#include <itkVariableSizeMatrix.h>
#include <itkMembershipFunctionBase.h>

namespace rstk {

template< typename TVector >
class UniformMembershipFunction:
  public itk::Statistics::MembershipFunctionBase< TVector >
{
public:
  /** Standard class typedefs */
  typedef UniformMembershipFunction Self;
  typedef itk::Statistics::MembershipFunctionBase< TVector >     Superclass;
  typedef itk::SmartPointer< Self >                              Pointer;
  typedef itk::SmartPointer< const Self >                        ConstPointer;

  /** Strandard macros */
  itkTypeMacro(UniformMembershipFunction, itk::Statistics::MembershipFunctionBase);
  itkNewMacro(Self);

  /** SmartPointer class for superclass */
  typedef typename Superclass::Pointer MembershipFunctionPointer;

  /** Typedef alias for the measurement vectors */
  typedef TVector MeasurementVectorType;

  /** Typedef to represent the length of measurement vectors */
  typedef typename Superclass::MeasurementVectorSizeType MeasurementVectorSizeType;

  /** Type of the mean vector. RealType on a vector-type is the same
   * vector-type but with a real element type.  */
  typedef typename itk::NumericTraits< MeasurementVectorType >::RealType MeasurementVectorRealType;
  typedef MeasurementVectorRealType  MeanVectorType;
  typedef typename itk::NumericTraits<MeasurementVectorRealType>::ScalarRealType MeasurementValueType;


  /**
   * Evaluate the Mahalanobis distance of a measurement using the
   * prescribed mean and covariance. Note that the Mahalanobis
   * distance is not a probability density. The square of the
   * distance is returned. */
  double Evaluate(const MeasurementVectorType & measurement) const { return m_Value; }

  itkSetMacro(Value, double);
  itkGetMacro(Value, double);

  double GetOffsetTerm() const { return 0.0; }

protected:
  UniformMembershipFunction(void): m_Value(0.0), Superclass() {}
  virtual ~UniformMembershipFunction(void) {}
  void PrintSelf(std::ostream & os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

private:
  double  m_Value;

};
} // end namespace itk


#endif /* SOURCE_DIRECTORY__MODULES_FUNCTIONAL_INCLUDE_UNIFORMMEMBERSHIPFUNCTION_H_ */
