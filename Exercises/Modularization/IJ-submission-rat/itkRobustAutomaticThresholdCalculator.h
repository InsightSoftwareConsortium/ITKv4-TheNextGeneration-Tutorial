/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRobustAutomaticThresholdCalculator.h,v $
  Language:  C++
  Date:      $Date: 2004/04/25 23:59:26 $
  Version:   $Revision: 1.37 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkRobustAutomaticThresholdCalculator_h
#define __itkRobustAutomaticThresholdCalculator_h

#include "itkMacro.h"
#include "itkImage.h"

namespace itk
{

/** \class RobustAutomaticThresholdCalculator
 * \brief Compute moments of an n-dimensional image.
 *
 *
 * \ingroup Operators
 *
 * \todo It's not yet clear how multi-echo images should be handled here.
 */
template < class TInputImage, class TGradientImage, class TMaskImage >
class ITK_EXPORT RobustAutomaticThresholdCalculator : public Object
{
public:
  /** Standard class typedefs. */
  typedef RobustAutomaticThresholdCalculator Self;
  typedef Object Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(RobustAutomaticThresholdCalculator, Object);

  /** Extract the dimension of the image. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** Standard image type within this class. */
  typedef TInputImage InputImageType;
  typedef TGradientImage GradientImageType;
  typedef TMaskImage MaskImageType;

  /** Standard image type pointer within this class. */
  typedef typename InputImageType::Pointer InputImagePointer;
  typedef typename InputImageType::ConstPointer InputImageConstPointer;
  typedef typename GradientImageType::Pointer GradientImagePointer;
  typedef typename GradientImageType::ConstPointer GradientImageConstPointer;
  typedef typename MaskImageType::Pointer MaskImagePointer;
  typedef typename MaskImageType::ConstPointer MaskImageConstPointer;

  typedef typename InputImageType::PixelType InputPixelType;
  typedef typename GradientImageType::PixelType GradientPixelType;
  typedef typename MaskImageType::PixelType MaskPixelType;

  /** Set the input image. */
  virtual void SetInput( const InputImageType * image )
    {
    if ( m_Input != image )
      {
      m_Input = image;
      this->Modified();
      m_Valid = false;
      }
    }

  virtual void SetGradient( const GradientImageType * image )
    {
    if ( m_Gradient != image )
      {
      m_Gradient = image;
      this->Modified();
      m_Valid = false;
      }
    }

  virtual void SetMask( const MaskImageType * image )
    {
    if ( m_Mask != image )
      {
      m_Mask = image;
      this->Modified();
      m_Valid = false;
      }
    }

  itkSetMacro(MaskValue, MaskPixelType);
  itkGetMacro(MaskValue, MaskPixelType);

  itkSetMacro(Pow, double);
  itkGetMacro(Pow, double);

  /** Compute moments of a new or modified image.
   * This method computes the moments of the image given as a
   * parameter and stores them in the object.  The values of these
   * moments and related parameters can then be retrieved by using
   * other methods of this object. */
  void Compute( void );
  
  const InputPixelType & GetOutput() const;

protected:
  RobustAutomaticThresholdCalculator();
  virtual ~RobustAutomaticThresholdCalculator() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  RobustAutomaticThresholdCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool m_Valid;                      // Have moments been computed yet?
  MaskPixelType m_MaskValue;
  double m_Pow;
  InputPixelType m_Output;

  InputImageConstPointer m_Input;
  GradientImageConstPointer m_Gradient;
  MaskImageConstPointer m_Mask;

};  // class RobustAutomaticThresholdCalculator

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRobustAutomaticThresholdCalculator.txx"
#endif

#endif /* __itkRobustAutomaticThresholdCalculator_h */
