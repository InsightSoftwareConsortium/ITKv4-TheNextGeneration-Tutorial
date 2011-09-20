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
#ifndef __itkRegistrationParameterScalesFromShift_hxx
#define __itkRegistrationParameterScalesFromShift_hxx

#include "itkRegistrationParameterScalesFromShift.h"

namespace itk
{

template< class TMetric >
RegistrationParameterScalesFromShift< TMetric >
::RegistrationParameterScalesFromShift()
{
  this->SetSamplingStrategy(Superclass::SamplingWithFullDomain);
}

/** Compute parameter scales */
template< class TMetric >
void
RegistrationParameterScalesFromShift< TMetric >
::EstimateScales(ScalesType &parameterScales)
{
  this->CheckAndSetInputs();
  this->SampleImageDomain();
  parameterScales.Fill(0);
  SizeValueType numPara = this->GetTransform()->GetNumberOfParameters();
  parameterScales.SetSize(numPara);

  FloatType maxShift;

  ParametersType deltaParameters(numPara);
  deltaParameters.Fill(0.0);

  // minNonZeroShift: the minimum non-zero shift.
  FloatType minNonZeroShift = NumericTraits<FloatType>::max();

  // compute voxel shift generated from each transform parameter
  for (SizeValueType i=0; i<numPara; i++)
    {
    deltaParameters[i] = 1.0;
    maxShift = this->ComputeMaximumVoxelShift(deltaParameters);
    deltaParameters[i] = 0.0;

    parameterScales[i] = maxShift;
    if ( maxShift > NumericTraits<FloatType>::epsilon() && maxShift < minNonZeroShift )
      {
      minNonZeroShift = maxShift;
      }
    std::cout << " i " << i << " shift " << maxShift << " param " << deltaParameters << " scales " << parameterScales << std::endl;
    }

  if (minNonZeroShift == NumericTraits<FloatType>::max())
    {
    itkWarningMacro(  << "Variation in any parameter won't change a voxel position."
                      << std::endl
                      << "The default scales (1.0) are used to avoid division-by-zero."
                   );
    parameterScales.fill(1.0);
    }
  else
    {
    for (SizeValueType i=0; i<numPara; i++)
      {
      if (parameterScales[i] <= NumericTraits<FloatType>::epsilon())
        {
        // To avoid division-by-zero in optimizers, assign a small value for a zero scale.
        parameterScales[i] = minNonZeroShift * minNonZeroShift;
        }
      else
        {
        parameterScales[i] *= parameterScales[i];
        }
      }
    }

}

/**
 * Compute the maximum shift when a transform is changed with deltaParameters
 */
template< class TMetric >
typename RegistrationParameterScalesFromShift< TMetric >::FloatType
RegistrationParameterScalesFromShift< TMetric >
::ComputeMaximumVoxelShift(const ParametersType &deltaParameters)
{
  //when ComputeMaximumVoxelShift is called without EstimateScales being called,
  //we need to make sure SampleImageDomain is called
  this->SampleImageDomain();

  FloatType shift;
  if (this->GetTransformForward())
    {
    shift = this->ComputeMaximumVoxelShiftTemplated<MovingTransformType>(deltaParameters);
    }
  else
    {
    shift = this->ComputeMaximumVoxelShiftTemplated<FixedTransformType>(deltaParameters);
    }
  return shift;
}

/** The templated version of ComputeMaximumVoxelShift.
 *  The template argument TTransform may be either
 *  MovingTransformType or FixedTransformType.
 */
template< class TMetric >
template< class TTransform >
typename RegistrationParameterScalesFromShift< TMetric >::FloatType
RegistrationParameterScalesFromShift< TMetric >
::ComputeMaximumVoxelShiftTemplated(const ParametersType &deltaParameters)
{
  FloatType voxelShift = 0.0;

  //We're purposely copying the parameters,
  //for temporary use of the transform to calculate the voxel shift.
  //After it is done, we will reset to the old parameters.
  TransformBase *transform = const_cast<TransformBase *>(this->GetTransform());
  const ParametersType oldParameters = transform->GetParameters();
  ParametersType newParameters(oldParameters.size());
  for (SizeValueType p=0; p<oldParameters.size(); p++)
    {
    newParameters[p] = oldParameters[p] + deltaParameters[p]*1.e-2;
    }
  std::cout << " newParams " << newParameters << std::endl;
  FloatType distance;
  SizeValueType numSamples = this->m_ImageSamples.size();

  VirtualPointType point;

  typedef ContinuousIndex<FloatType, TTransform::OutputSpaceDimension> ContinuousIndexType;

  ContinuousIndexType newMappedIndex;
  ContinuousIndexType diffIndex;

  //store the old mapped indices to reduce calls to Transform::SetParameters()
  std::vector<ContinuousIndexType> oldMappedIndices(numSamples);

  // compute the indices mapped by the old transform
  for (SizeValueType c=0; c<numSamples; c++)
    {
    point = this->m_ImageSamples[c];
    this->template TransformPointToContinuousIndex<ContinuousIndexType>(point, oldMappedIndices[c]);
    } // end for numSamples

  // set the new parameters in the transform
  transform->SetParameters(newParameters);

  // compute the indices mapped by the new transform
  for (SizeValueType c=0; c<numSamples; c++)
    {
    point = this->m_ImageSamples[c];
    this->template TransformPointToContinuousIndex<ContinuousIndexType>(point, newMappedIndex);

    // find max shift by checking each sample point
    distance = newMappedIndex.EuclideanDistanceTo(oldMappedIndices[c]);
    if ( voxelShift < distance )
      {
      voxelShift = distance;
      }
  } // end for numSamples

  // restore the parameters in the transform
  transform->SetParameters(oldParameters);

  return voxelShift;
}

/** Print the information about this class */
template< class TMetric >
void
RegistrationParameterScalesFromShift< TMetric >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

}  // namespace itk

#endif /* __itkRegistrationParameterScalesFromShift_txx */
