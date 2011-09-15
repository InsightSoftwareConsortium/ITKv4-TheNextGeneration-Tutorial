/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRobustAutomaticThresholdImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/05 13:59:37 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkRobustAutomaticThresholdImageFilter_h
#define __itkRobustAutomaticThresholdImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkRobustAutomaticThresholdCalculator.h"

namespace itk {

/** \class RobustAutomaticThresholdImageFilter 
 * \brief Threshold an image using multiple Otsu Thresholds.
 *
 * This filter creates a labeled image that separates the input
 * image into various classes. The filter
 * computes the thresholds using the MaximumEntropyThresholdCalculator and
 * applies those thesholds to the input image using the
 * ThresholdLabelerImageFilter. The NumberOfHistogramBins and
 * NumberOfThresholds can be set
 * for the Calculator. The LabelOffset can be set
 * for the ThresholdLabelerImageFilter.
 *
 * \sa ScalarImageToHistogramGenerator
 * \sa MaximumEntropyThresholdCalculator
 * \sa ThresholdLabelerImageFilter
 * \ingroup IntensityImageFilters  Multithreaded
 */

template<class TInputImage, class TGradientImage=TInputImage, class TMaskImage=Image<unsigned char, TInputImage::ImageDimension>, class TOutputImage=TInputImage>
class ITK_EXPORT RobustAutomaticThresholdImageFilter : 
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard Self typedef */
  typedef RobustAutomaticThresholdImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(RobustAutomaticThresholdImageFilter, ImageToImageFilter);
  
  /** Standard image type within this class. */
  typedef TInputImage InputImageType;
  typedef TGradientImage GradientImageType;
  typedef TMaskImage MaskImageType;

  /** Image pixel value typedef. */
  typedef typename TInputImage::PixelType   InputPixelType;
  typedef typename TOutputImage::PixelType   OutputPixelType;
  typedef typename TGradientImage::PixelType   GradientPixelType;
  typedef typename TMaskImage::PixelType   MaskPixelType;
  
  /** Image related typedefs. */
  typedef typename TInputImage::Pointer InputImagePointer;
  typedef typename TOutputImage::Pointer OutputImagePointer;
  typedef typename TGradientImage::Pointer GradientImagePointer;
  typedef typename TMaskImage::Pointer MaskImagePointer;

  typedef typename TInputImage::SizeType  InputSizeType;
  typedef typename TInputImage::IndexType  InputIndexType;
  typedef typename TInputImage::RegionType InputImageRegionType;
  typedef typename TOutputImage::SizeType  OutputSizeType;
  typedef typename TOutputImage::IndexType  OutputIndexType;
  typedef typename TOutputImage::RegionType OutputImageRegionType;

  typedef RobustAutomaticThresholdCalculator< TInputImage, TGradientImage, TMaskImage > CalculatorType;
  
  /** Image related typedefs. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension ) ;
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension ) ;

  /** Set the "outside" pixel value. The default value
   * NumericTraits<OutputPixelType>::Zero. */
  itkSetMacro(OutsideValue,OutputPixelType);

  /** Get the "outside" pixel value. */
  itkGetMacro(OutsideValue,OutputPixelType);

  /** Set the "inside" pixel value. The default value
   * NumericTraits<OutputPixelType>::max() */
  itkSetMacro(InsideValue,OutputPixelType);

  /** Get the "inside" pixel value. */
  itkGetMacro(InsideValue,OutputPixelType);

  /** Get the computed threshold. */
  itkGetMacro(Threshold,InputPixelType);

  itkSetMacro(MaskValue, MaskPixelType);
  itkGetMacro(MaskValue, MaskPixelType);

  itkSetMacro(Pow, double);
  itkGetMacro(Pow, double);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(OutputComparableCheck,
    (Concept::Comparable<OutputPixelType>));
  itkConceptMacro(OutputOStreamWritableCheck,
    (Concept::OStreamWritable<OutputPixelType>));
  /** End concept checking */
#endif

   /** Set the mask image */
  void SetMaskImage(MaskImageType *input)
     {
     // Process object is not const-correct so the const casting is required.
     this->SetNthInput( 1, const_cast<MaskImageType *>(input) );
     }

  /** Get the mask image */
  MaskImageType * GetMaskImage()
    {
    return static_cast<MaskImageType*>(const_cast<DataObject *>(this->ProcessObject::GetInput(2)));
    }


   /** Set the gradient image */
  void SetGradientImage(GradientImageType *input)
     {
     // Process object is not const-correct so the const casting is required.
     this->SetNthInput( 1, const_cast<GradientImageType *>(input) );
     }

  /** Get the gradient image */
  GradientImageType * GetGradientImage()
    {
    return static_cast<GradientImageType*>(const_cast<DataObject *>(this->ProcessObject::GetInput(1)));
    }



   /** Set the input image */
  void SetInput1(TInputImage *input)
     {
     this->SetInput( input );
     }

   /** Set the gradient image */
  void SetInput2(GradientImageType *input)
     {
     this->SetGradientImage( input );
     }

   /** Set the marker image */
  void SetInput3(MaskImageType *input)
     {
     this->SetMaskImage( input );
     }

protected:
  RobustAutomaticThresholdImageFilter();
  ~RobustAutomaticThresholdImageFilter(){};
  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateInputRequestedRegion();
  void GenerateData ();

private:
  RobustAutomaticThresholdImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MaskPixelType m_MaskValue;
  double m_Pow;
  InputPixelType      m_Threshold;
  OutputPixelType     m_InsideValue;
  OutputPixelType     m_OutsideValue;


} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRobustAutomaticThresholdImageFilter.txx"
#endif

#endif
