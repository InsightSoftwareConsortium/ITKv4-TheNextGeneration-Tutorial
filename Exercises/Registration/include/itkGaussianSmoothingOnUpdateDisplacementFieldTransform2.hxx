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
#ifndef __itkGaussianSmoothingOnUpdateDisplacementFieldTransform2_hxx
#define __itkGaussianSmoothingOnUpdateDisplacementFieldTransform2_hxx

#include "itkGaussianSmoothingOnUpdateDisplacementFieldTransform2.h"

#include "itkAddImageFilter.h"
#include "itkGaussianOperator.h"
#include "itkImageDuplicator.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImportImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkVectorNeighborhoodOperatorImageFilter.h"

namespace itk
{

/**
 * Constructor
 */
template<class TScalar, unsigned int NDimensions>
GaussianSmoothingOnUpdateDisplacementFieldTransform2<TScalar, NDimensions>
::GaussianSmoothingOnUpdateDisplacementFieldTransform2()
{
  this->m_GaussianSmoothingVarianceForTheUpdateField = 3.0;
  this->m_GaussianSmoothingVarianceForTheTotalField = 0.5;
}

/**
 * Destructor
 */
template<class TScalar, unsigned int NDimensions>
GaussianSmoothingOnUpdateDisplacementFieldTransform2<TScalar, NDimensions>::
~GaussianSmoothingOnUpdateDisplacementFieldTransform2()
{
}

template<class TScalar, unsigned int NDimensions>
void
GaussianSmoothingOnUpdateDisplacementFieldTransform2<TScalar, NDimensions>
::UpdateTransformParameters( DerivativeType & update, ScalarType factor)
{
  DisplacementFieldPointer displacementField = this->GetDisplacementField();

  const typename DisplacementFieldType::RegionType & bufferedRegion = displacementField->GetBufferedRegion();
  const SizeValueType numberOfPixels = bufferedRegion.GetNumberOfPixels();

  typedef ImportImageFilter<DisplacementVectorType, NDimensions> ImporterType;
  const bool importFilterWillReleaseMemory = false;

  //
  // Smooth the update field
  //
  bool smoothUpdateField = true;
  if( this->m_GaussianSmoothingVarianceForTheUpdateField <= 0.0 )
    {
    itkDebugMacro( "Not smooothing the update field." );
    smoothUpdateField = false;
    }
  if( smoothUpdateField )
    {
    itkDebugMacro( "Smooothing the update field." );

    DisplacementVectorType *updateFieldPointer = reinterpret_cast<DisplacementVectorType *>( update.data_block() );

    typename ImporterType::Pointer importer = ImporterType::New();
    importer->SetImportPointer( updateFieldPointer, numberOfPixels, importFilterWillReleaseMemory );
    importer->SetRegion( displacementField->GetBufferedRegion() );
    importer->SetOrigin( displacementField->GetOrigin() );
    importer->SetSpacing( displacementField->GetSpacing() );
    importer->SetDirection( displacementField->GetDirection() );

    DisplacementFieldPointer updateField = importer->GetOutput();
    updateField->Update();
    updateField->DisconnectPipeline();

    DisplacementFieldPointer updateSmoothField = this->GaussianSmoothDisplacementField( updateField, this->m_GaussianSmoothingVarianceForTheUpdateField );

    DerivativeValueType *updatePointer = reinterpret_cast<DerivativeValueType *>( updateSmoothField->GetBufferPointer() );

    memcpy( update.data_block(), updatePointer, sizeof( DisplacementVectorType ) * numberOfPixels );
    }

  //
  // Add the update field to the current total field before (optionally)
  // smoothing the total field
  //
  Superclass::UpdateTransformParameters( update, factor );

  //
  // Smooth the total field
  //
  bool smoothTotalField = true;
  if( this->m_GaussianSmoothingVarianceForTheTotalField <= 0.0 )
    {
    itkDebugMacro( "Not smooothing the total field." );
    smoothTotalField = false;
    }
  if( smoothTotalField )
    {
    itkDebugMacro( "Smooothing the total field." );

    typename ImporterType::Pointer importer = ImporterType::New();
    importer->SetImportPointer( displacementField->GetBufferPointer(), numberOfPixels, importFilterWillReleaseMemory );
    importer->SetRegion( displacementField->GetBufferedRegion() );
    importer->SetOrigin( displacementField->GetOrigin() );
    importer->SetSpacing( displacementField->GetSpacing() );
    importer->SetDirection( displacementField->GetDirection() );

    DisplacementFieldPointer totalField = importer->GetOutput();
    totalField->Update();
    totalField->DisconnectPipeline();

    DisplacementFieldPointer totalSmoothField = this->GaussianSmoothDisplacementField( totalField, this->m_GaussianSmoothingVarianceForTheTotalField );

    memcpy( displacementField->GetBufferPointer(), totalSmoothField->GetBufferPointer(), sizeof( DisplacementVectorType ) * numberOfPixels );
    }
}

template<class TScalar, unsigned int NDimensions>
typename GaussianSmoothingOnUpdateDisplacementFieldTransform2<TScalar, NDimensions>::DisplacementFieldPointer
GaussianSmoothingOnUpdateDisplacementFieldTransform2<TScalar, NDimensions>
::GaussianSmoothDisplacementField( DisplacementFieldType *field, ScalarType variance )
{
  if( variance <= 0 )
    {
    return field;
    }

  /* Allocate temp field if new displacement field has been set.
   * We only want to allocate this field if this method is used */
  if( this->GetDisplacementFieldSetTime() >
      this->m_GaussianSmoothingTempFieldModifiedTime )
    {
    this->m_GaussianSmoothingTempFieldModifiedTime = this->GetMTime();
    m_GaussianSmoothingTempField = DisplacementFieldType::New();
    m_GaussianSmoothingTempField->SetSpacing( field->GetSpacing() );
    m_GaussianSmoothingTempField->SetOrigin( field->GetOrigin() );
    m_GaussianSmoothingTempField->SetDirection( field->GetDirection() );
    m_GaussianSmoothingTempField->SetLargestPossibleRegion(
                                          field->GetLargestPossibleRegion() );
    m_GaussianSmoothingTempField->SetRequestedRegion(
                                                field->GetRequestedRegion() );
    m_GaussianSmoothingTempField->SetBufferedRegion(
                                                field->GetBufferedRegion() );
    m_GaussianSmoothingTempField->Allocate();

    //This should only be allocated once as well, for efficiency.
    m_GaussianSmoothingSmoother = GaussianSmoothingSmootherType::New();
    }

  if( m_GaussianSmoothingTempField.IsNull() )
    {
    itkExceptionMacro("Expected m_GaussianSmoothingTempField to be allocated.");
    }

  typedef typename DisplacementFieldType::PixelType   VectorType;

  typedef typename DisplacementFieldType::PixelContainerPointer
                                                        PixelContainerPointer;
  // I think we need to keep this as SmartPointer type, to preserve the
  // reference counting so we can assign the swapPtr to the main field and
  // not have to do a memory copy - this happens when image dimensions are odd.
  PixelContainerPointer swapPtr;

  // graft the output field onto the mini-pipeline
  m_GaussianSmoothingSmoother->GraftOutput( m_GaussianSmoothingTempField );

  for( unsigned int j = 0; j < Superclass::Dimension; j++ )
    {
    // smooth along this dimension
    m_GaussianSmoothingOperator.SetDirection( j );
    m_GaussianSmoothingOperator.SetVariance( variance );
    m_GaussianSmoothingOperator.SetMaximumError(0.001 );
    m_GaussianSmoothingOperator.SetMaximumKernelWidth( 256 );
    m_GaussianSmoothingOperator.CreateDirectional();

    // todo: make sure we only smooth within the buffered region
    m_GaussianSmoothingSmoother->SetOperator( m_GaussianSmoothingOperator );
    m_GaussianSmoothingSmoother->SetInput( field );
    try
      {
      m_GaussianSmoothingSmoother->Update();
      }
    catch( ExceptionObject & exc )
      {
      std::string msg("Caught exception: ");
      msg += exc.what();
      itkExceptionMacro( << msg );
      }

    if( j < Superclass::Dimension - 1 )
      {
      // swap the containers
      swapPtr = m_GaussianSmoothingSmoother->GetOutput()->GetPixelContainer();
      m_GaussianSmoothingSmoother->GraftOutput( field );
      // SetPixelContainer does a smartpointer assignment, so the pixel
      // container won't be deleted if field  points to the
      // temporary field upon exiting this method.
      field->SetPixelContainer( swapPtr );
      m_GaussianSmoothingSmoother->Modified();
      }
    }

  if( Superclass::Dimension % 2 == 0 )
    {
    // For even number of dimensions, the final pass writes the output
    // into field's original pixel container, so we just point back to that.
    // And point the temporary field back to its original container for next
    // time through.
    m_GaussianSmoothingTempField->SetPixelContainer(
                                                  field->GetPixelContainer() );
    field->SetPixelContainer(
               m_GaussianSmoothingSmoother->GetOutput()->GetPixelContainer() );
    }

  //make sure boundary does not move
  ScalarType weight = 1.0;
  if (variance < 0.5)
    {
    weight=1.0 - 1.0 * ( variance / 0.5);
    }
  ScalarType weight2 = 1.0 - weight;
  typedef ImageRegionIteratorWithIndex<DisplacementFieldType> Iterator;
  typename DisplacementFieldType::SizeType size =
                                field->GetLargestPossibleRegion().GetSize();
  Iterator outIter( field, field->GetLargestPossibleRegion() );
  for( outIter.GoToBegin(); !outIter.IsAtEnd(); ++outIter )
  {
    bool onboundary=false;
    typename DisplacementFieldType::IndexType index= outIter.GetIndex();
    for (int i=0; i < Superclass::Dimension; i++)
      {
      if (index[i] < 1 || index[i] >= static_cast<int>( size[i] )-1 )
        {
        onboundary=true;
        }
      }
    if( onboundary )
      {
      VectorType vec;
      vec.Fill(0.0);
      outIter.Set(vec);
      }
    else
      {
      VectorType
          svec = m_GaussianSmoothingSmoother->GetOutput()->GetPixel( index );
      outIter.Set( svec * weight + outIter.Get() * weight2);
      }
  }

  itkDebugMacro("done gauss smooth ");
  return field;
}


template<class TScalar, unsigned int NDimensions>
typename GaussianSmoothingOnUpdateDisplacementFieldTransform2<TScalar, NDimensions>::DisplacementFieldPointer
GaussianSmoothingOnUpdateDisplacementFieldTransform2<TScalar, NDimensions>
::GaussianSmoothDisplacementFieldNick( DisplacementFieldType *inputField, ScalarType variance )
{
  typedef ImageDuplicator<DisplacementFieldType> DuplicatorType;
  typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage( inputField );
  duplicator->Update();
  DisplacementFieldPointer outputField = duplicator->GetOutput();

  const typename DisplacementFieldType::RegionType & bufferedRegion = outputField->GetBufferedRegion();

  typedef VectorNeighborhoodOperatorImageFilter<DisplacementFieldType, DisplacementFieldType> SmootherType;
  typename SmootherType::Pointer smoother = SmootherType::New();

  typedef GaussianOperator<typename DisplacementVectorType::ValueType, NDimensions> GaussianType;
  GaussianType gaussian;
  gaussian.SetVariance( variance );
  gaussian.SetMaximumError( 0.001 );

  for( unsigned int d = 0; d < NDimensions; d++ )
    {
    gaussian.SetDirection( d );
    gaussian.SetMaximumKernelWidth( bufferedRegion.GetSize()[d] );
    gaussian.CreateDirectional();

    smoother->SetOperator( gaussian );
    smoother->SetInput( outputField );

    outputField = smoother->GetOutput();
    outputField->Update();
    outputField->DisconnectPipeline();
    }

  // Ensure zero motion on the boundary

  typename DisplacementVectorType::ValueType weight1 = 1.0;
  if( variance < 0.5 )
    {
    weight1 = 1.0 - 1.0 * ( variance / 0.5 );
    }
  typename DisplacementVectorType::ValueType weight2 = 1.0 - weight1;

  typedef MultiplyImageFilter<DisplacementFieldType, DisplacementFieldType, DisplacementFieldType> MultiplierType;

  typename MultiplierType::Pointer multiplier1 = MultiplierType::New();
  multiplier1->SetInput1( outputField );
  multiplier1->SetConstant2( weight1 );

  typename MultiplierType::Pointer multiplier2 = MultiplierType::New();
  multiplier2->SetInput1( inputField );
  multiplier2->SetConstant2( weight2 );

  typedef AddImageFilter<DisplacementFieldType, DisplacementFieldType, DisplacementFieldType> AdderType;
  typename AdderType::Pointer adder = AdderType::New();
  adder->SetInput1( multiplier1->GetOutput() );
  adder->SetInput2( multiplier2->GetOutput() );

  outputField = adder->GetOutput();
  outputField->Update();
  outputField->DisconnectPipeline();

  DisplacementVectorType zeroVector( 0.0 );

  ImageLinearIteratorWithIndex<DisplacementFieldType> It( outputField, outputField->GetRequestedRegion() );
  for( unsigned int d = 0; d < NDimensions; d++ )
    {
    It.SetDirection( d );
    It.GoToBegin();
    while( !It.IsAtEnd() )
      {
      It.GoToBeginOfLine();
      It.Set( zeroVector );
      It.GoToEndOfLine();
      --It;
      It.Set( zeroVector );

      It.NextLine();
      }
    }

  return outputField;
}

template <class TScalar, unsigned int NDimensions>
void
GaussianSmoothingOnUpdateDisplacementFieldTransform2<TScalar, NDimensions>::
PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os,indent );

  os << indent << "Gaussian smoothing parameters: " << std::endl
     << indent << "m_GaussianSmoothingVarianceForTheUpdateField: " << this->m_GaussianSmoothingVarianceForTheUpdateField
     << std::endl
     << indent << "m_GaussianSmoothingVarianceForTheTotalField: " << this->m_GaussianSmoothingVarianceForTheTotalField
     << std::endl;
}
} // namespace itk

#endif
