/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkRegistrationParameterScaleEstimator_hxx
#define __itkRegistrationParameterScaleEstimator_hxx

#include "itkRegistrationParameterScaleEstimator.h"

namespace itk
{

template< class TMetric >
RegistrationParameterScaleEstimator< TMetric >
::RegistrationParameterScaleEstimator()
{
  // estimate paramter scales of the moving transform
  m_TransformForward = true;

  // number for random sampling
  m_NumberOfRandomSamples = 0;

  // the metric object must be set before EstimateScales()
  this->m_Metric = NULL;
  this->m_FixedImage   = NULL;
  this->m_MovingImage  = NULL;
  this->m_VirtualImage = NULL;

  this->m_MovingTransform = NULL;
  this->m_FixedTransform  = NULL;
}

template< class TMetric >
bool
RegistrationParameterScaleEstimator< TMetric >
::CheckAndSetInputs()
{
if (m_Metric == (MetricPointer)NULL)
    {
    itkExceptionMacro("RegistrationParameterScaleEstimator: the metric is NULL");
    }

  this->m_FixedImage = m_Metric->GetFixedImage();
  this->m_MovingImage = m_Metric->GetMovingImage();
  this->m_VirtualImage = m_Metric->GetVirtualDomainImage();

  if (this->m_FixedImage.GetPointer() == NULL
    || this->m_MovingImage.GetPointer() == NULL
    || this->m_VirtualImage.GetPointer() == NULL)
    {
    itkExceptionMacro("RegistrationParameterScaleEstimator: the image(s) in the metric is NULL");
    }

  this->m_MovingTransform = m_Metric->GetMovingTransform();
  this->m_FixedTransform = m_Metric->GetFixedTransform();

  if (this->m_MovingTransform.GetPointer() == NULL
    || this->m_FixedTransform.GetPointer() == NULL)
    {
    itkExceptionMacro("RegistrationParameterScaleEstimator: the transform(s) in the metric is NULL.");
    }

  const ParametersType &parameters = this->GetTransform()->GetParameters();
  for (SizeValueType p = 0; p < parameters.size(); p++)
    {
    // check for NaN
    if (parameters[p] != parameters[p])
      {
      itkExceptionMacro("RegistrationParameterScaleEstimator: a transform parameter is not a number.");
      }
    }

  return true;
}

/** Get the transform being estimated scales for. */
template< class TMetric >
const TransformBase *
RegistrationParameterScaleEstimator< TMetric >
::GetTransform()
{
  if (m_TransformForward)
    {
    return m_MovingTransform.GetPointer();
    }
  else
    {
    return m_FixedTransform.GetPointer();
    }
}

/** Get the dimension of the target image transformed to. */
template< class TMetric >
itk::SizeValueType
RegistrationParameterScaleEstimator< TMetric >
::GetImageDimension()
{
  if (m_TransformForward)
    {
    return MovingImageDimension;
    }
  else
    {
    return FixedImageDimension;
    }
}

/** Transform a physical point to its continuous index */
template< class TMetric >
template< class TContinuousIndexType >
void
RegistrationParameterScaleEstimator< TMetric >
::TransformPointToContinuousIndex(const VirtualPointType &point,
                                  TContinuousIndexType &mappedIndex)
{
  if (this->GetTransformForward())
    {
    MovingPointType mappedPoint;
    mappedPoint = this->GetMovingTransform()->TransformPoint(point);
    this->GetMovingImage()->TransformPhysicalPointToContinuousIndex(mappedPoint, mappedIndex);
    }
  else
    {
    FixedPointType mappedPoint;
    mappedPoint = this->GetFixedTransform()->TransformPoint(point);
    this->GetFixedImage()->TransformPhysicalPointToContinuousIndex(mappedPoint, mappedIndex);
    }
}

/** Get the squared norms of the transform Jacobians w.r.t parameters at a point */
template< class TMetric >
template< class TJacobianType >
void
RegistrationParameterScaleEstimator< TMetric >
::ComputeSquaredJacobianNorms( const VirtualPointType  & point,
                                     ParametersType & squareNorms )
{
  TJacobianType jacobian;
  SizeValueType numPara = this->GetTransform()->GetNumberOfParameters();
  SizeValueType dim = this->GetImageDimension();

  if (this->GetTransformForward())
    {
    this->GetMovingTransform()->ComputeJacobianWithRespectToParameters(point, jacobian);

    for (SizeValueType p=0; p<numPara; p++)
      {
      squareNorms[p] = 0;
      for (SizeValueType d=0; d<dim; d++)
        {
        squareNorms[p] += jacobian[d][p] * jacobian[d][p];
        }
      }
    }
  else
    {
    this->GetFixedTransform()->ComputeJacobianWithRespectToParameters(point, jacobian);

    for (SizeValueType p=0; p<numPara; p++)
      {
      squareNorms[p] = 0;
      for (SizeValueType d=0; d<dim; d++)
        {
        squareNorms[p] += jacobian[d][p] * jacobian[d][p];
        }
      }
    }
}

/** Sample the virtual image domain with phyical points
 *  and store the results into m_ImageSamples.
 */
template< class TMetric >
void
RegistrationParameterScaleEstimator< TMetric >
::SampleImageDomain()
{
  if (m_SamplingStrategy == SamplingWithCorners)
    {
    this->SampleImageDomainWithCorners();
    }
  else if (m_SamplingStrategy == SamplingWithRandom)
    {
    this->SampleImageDomainRandomly();
    }
  else
    {
    this->SampleImageDomainFully();
    }

}

/**
 *  Sample the virtual domain with the points at image corners.
 *  and store the results into m_ImageSamples.
 */
template< class TMetric >
void
RegistrationParameterScaleEstimator< TMetric >
::SampleImageDomainWithCorners()
{
  VirtualImagePointer image = this->m_VirtualImage;
  m_ImageSamples.clear();

  VirtualRegionType region = image->GetLargestPossibleRegion();
  VirtualIndexType firstCorner = region.GetIndex();
  VirtualIndexType corner;
  VirtualPointType point;

  VirtualSizeType size = region.GetSize();
  int cornerNumber = 1 << VirtualImageDimension; // 2^ImageDimension

  for(int i=0; i<cornerNumber; i++)
    {
    int bit;
    for (int d=0; d<VirtualImageDimension; d++)
      {
      bit = (int) (( i & (1 << d) ) != 0); // 0 or 1
      corner[d] = firstCorner[d] + bit * (size[d] - 1);
      }

    image->TransformIndexToPhysicalPoint(corner, point);
    m_ImageSamples.push_back(point);
    }
}

/**
 * Sample the physical points of the virtual image in a uniform random distribution.
 */
template< class TMetric >
void
RegistrationParameterScaleEstimator< TMetric >
::SampleImageDomainRandomly()
{
  VirtualImagePointer image = this->GetVirtualImage();

  if (m_NumberOfRandomSamples == 0)
    {
    SizeValueType total = image->GetLargestPossibleRegion().GetNumberOfPixels();
    if (total <= SizeOfSmallDomain)
      {
      m_NumberOfRandomSamples = total;
      }
    else
      {
      FloatType ratio = vcl_log((FloatType)total);
      if (ratio < 1)
        {
        ratio = 1;
        }
      m_NumberOfRandomSamples = SizeOfSmallDomain * ratio;
      }
    }

  m_ImageSamples.clear();

  // Set up a random interator within the user specified fixed image region.
  typedef ImageRandomConstIteratorWithIndex<VirtualImageType> RandomIterator;
  RandomIterator randIter( image, image->GetLargestPossibleRegion() );

  VirtualPointType point;

  randIter.SetNumberOfSamples( m_NumberOfRandomSamples );
  randIter.GoToBegin();
  for (SizeValueType i=0; i<m_NumberOfRandomSamples; i++)
    {
    image->TransformIndexToPhysicalPoint( randIter.GetIndex(), point );
    m_ImageSamples.push_back(point);
    ++randIter;
    }
}

/**
 * Sample the virtual image domain fully with all pixels.
 */
template< class TMetric >
void
RegistrationParameterScaleEstimator< TMetric >
::SampleImageDomainFully()
{

  VirtualImagePointer image = this->m_VirtualImage;

  m_ImageSamples.clear();

  // Set up a random interator within the user specified fixed image region.
  typedef ImageRegionConstIteratorWithIndex<VirtualImageType> RegionIterator;
  RegionIterator regionIter( image, image->GetLargestPossibleRegion() );

  VirtualPointType point;

  /* Iterate over the image */
  regionIter.GoToBegin();
  while( !regionIter.IsAtEnd() )
    {
    image->TransformIndexToPhysicalPoint( regionIter.GetIndex(), point );
    m_ImageSamples.push_back(point);
    ++regionIter;
    }
}

/** Print the information about this class */
template< class TMetric >
void
RegistrationParameterScaleEstimator< TMetric >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "MetricType   = " << std::endl;
  os << indent << typeid(MetricType).name()  << std::endl;

  os << indent << "FixedTransformType   = " << std::endl;
  os << indent << typeid(FixedTransformType).name()  << std::endl;

  os << indent << "MovingTransformType  = " << std::endl;
  os << indent << typeid(MovingTransformType).name()  << std::endl;

  os << indent << "FixedImageType    = " << std::endl;
  os << indent << typeid(FixedImageType).name()  << std::endl;

  os << indent << "MovingImageType   = " << std::endl;
  os << indent << typeid(MovingImageType).name()  << std::endl;

  os << indent << "VirtualImageType  = " << std::endl;
  os << indent << typeid(VirtualImageType).name()  << std::endl;

  os << indent << "m_ImageSamples.size = " << std::endl;
  os << indent << this->m_ImageSamples.size()  << std::endl;

  os << indent << "m_TransformForward = " << this->m_TransformForward << std::endl;
  os << indent << "m_SamplingStrategy = " << this->m_SamplingStrategy << std::endl;

}

}  // namespace itk

#endif /* __itkRegistrationParameterScaleEstimator_txx */
