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
#ifndef __itkOptimizerParameterScaleEstimator_h
#define __itkOptimizerParameterScaleEstimator_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkObjectToObjectOptimizerBase.h"

namespace itk
{

/** \class OptimizerParameterScaleEstimator
 *  \brief OptimizerParameterScaleEstimator is the base class offering a
 * empty method of estimating the parameter scales for optimizers.
 *
 * Its subclass RegistrationParameterScaleEstimator estimates scales for
 * registration optimizers.
 *
 * \ingroup ITKHighDimensionalOptimizers
 */
class ITK_EXPORT OptimizerParameterScaleEstimator : public Object
{
public:
  /** Standard class typedefs. */
  typedef OptimizerParameterScaleEstimator      Self;
  typedef Object                                Superclass;
  typedef SmartPointer<Self>                    Pointer;
  typedef SmartPointer<const Self>              ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( OptimizerParameterScaleEstimator, Object );

  /** Type of scales */
  typedef ObjectToObjectOptimizerBase::ScalesType        ScalesType;
  /** Type of paramters of the optimizer */
  typedef ObjectToObjectOptimizerBase::ParametersType    ParametersType;
  /** Type of float */
  typedef double                                         FloatType;

  /** Estimate parameter scales */
  virtual void EstimateScales(ScalesType &scales) = 0;

protected:
  OptimizerParameterScaleEstimator(){};
  ~OptimizerParameterScaleEstimator(){};

  void PrintSelf(std::ostream &os, Indent indent) const
    {
    Superclass::PrintSelf(os,indent);
    }

private:
  OptimizerParameterScaleEstimator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

}; //class OptimizerParameterScaleEstimator

}  // namespace itk

#endif /* __itkOptimizerParameterScaleEstimator_h */
