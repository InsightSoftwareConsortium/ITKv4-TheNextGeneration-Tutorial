/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRobustAutomaticThresholdCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2005/08/10 16:32:57 $
  Version:   $Revision: 1.52 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkRobustAutomaticThresholdCalculator_txx
#define _itkRobustAutomaticThresholdCalculator_txx
#include "itkRobustAutomaticThresholdCalculator.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{ 



template < class TInputImage, class TGradientImage, class TMaskImage >
RobustAutomaticThresholdCalculator<TInputImage, TGradientImage, TMaskImage>
::RobustAutomaticThresholdCalculator(void) 
{
  m_Valid = false;
  m_Input = NULL;
  m_Gradient = NULL;
  m_Mask = NULL;
  m_MaskValue = NumericTraits< MaskPixelType >::max();
  m_Output = NumericTraits< InputPixelType >::Zero;
  m_Pow = 1;
}


template < class TInputImage, class TGradientImage, class TMaskImage >
void
RobustAutomaticThresholdCalculator<TInputImage, TGradientImage, TMaskImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Input: " << m_Input.GetPointer() << std::endl;
  os << indent << "Gradient: " << m_Gradient.GetPointer() << std::endl;
  os << indent << "Mask: " << m_Mask.GetPointer() << std::endl;
  os << indent << "Valid: " << m_Valid << std::endl;
  os << indent << "MaskValue: " << m_MaskValue << std::endl;
  os << indent << "Pow: " << m_Pow << std::endl;
  os << indent << "Output: " << m_Output << std::endl;
}


template < class TInputImage, class TGradientImage, class TMaskImage >
void
RobustAutomaticThresholdCalculator<TInputImage, TGradientImage, TMaskImage>
::Compute()
{

  typedef typename InputImageType::IndexType IndexType;

  if( !m_Input || !m_Gradient ) 
    {
    return;
    }

  ImageRegionConstIteratorWithIndex< InputImageType > iIt( m_Input,
                                                     m_Input->GetRequestedRegion() ); 
  iIt.GoToBegin();
  ImageRegionConstIteratorWithIndex< GradientImageType > gIt( m_Gradient,
                                                     m_Gradient->GetRequestedRegion() ); 
  gIt.GoToBegin();

  // init the values
  double n = 0;
  double d = 0;
  
  while( !iIt.IsAtEnd() )
    {
    if( !m_Mask || m_Mask->GetPixel( iIt.GetIndex() ) == m_MaskValue )
      {
      double g = pow( gIt.Get(), m_Pow );
      n += iIt.Get() * g;
      d += g;
      }
    ++iIt;
    ++gIt;
    }
    
//   std::cout << "n: " << n << "  d: " << d << std::endl;
  m_Output = static_cast< InputPixelType >( n / d );
  m_Valid = true;

}



template < class TInputImage, class TGradientImage, class TMaskImage >
const typename RobustAutomaticThresholdCalculator<TInputImage, TGradientImage, TMaskImage>::InputPixelType &
RobustAutomaticThresholdCalculator<TInputImage, TGradientImage, TMaskImage>
::GetOutput() const
{
  if (!m_Valid)
    {
    itkExceptionMacro( << "GetOutput() invoked, but the output have not been computed. Call Compute() first.");
    }
  return m_Output;
}

} // end namespace itk



#endif
