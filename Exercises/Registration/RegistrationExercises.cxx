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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkImageMomentsCalculator.h"

#include "itkJointHistogramMutualInformationImageToImageMetricv4.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkEuler2DTransform.h"
#include "itkGradientDescentOptimizerv4.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkRegistrationParameterScalesFromShift.h"

#include "itkQuasiNewtonOptimizerv4.h"

template<class TOptimizer>
class CommandIterationUpdate : public itk::Command
{
public:
  typedef CommandIterationUpdate   Self;
  typedef itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};

public:

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    TOptimizer * optimizer =
      dynamic_cast< TOptimizer * >( caller );

    if( typeid( event ) != typeid( itk::IterationEvent ) )
      {
      return;
      }

    typename TOptimizer::DerivativeType gradient = optimizer->GetGradient();

    std::cout << "   Learning rate:          " << optimizer->GetLearningRate() << std::endl;
    std::cout << "   Metric value:           " << optimizer->GetValue() << std::endl;
    std::cout << "   Optimizer scales:       " << optimizer->GetScales() << std::endl;
    std::cout << "   Metric gradient:        " << gradient << std::endl;
    std::cout << "   Transform parameters:   " << optimizer->GetCurrentPosition() << std::endl;
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    itkExceptionMacro( "Should not be here." );
    }
};

int main( int argc, char *argv[] )
{
  if( argc < 4 )
    {
    std::cout << argv[0] << " fixedImage movingImage outputImage numberIterations " << std::endl;
    exit( 1 );
    }

  // Define what image dimension we will be working with and what type we
  // want to use for the pixel.
  static const unsigned int ImageDimension = 2;

  typedef float                                   PixelType;
  typedef itk::Image< PixelType, ImageDimension > FixedImageType;
  typedef itk::Image< PixelType, ImageDimension > MovingImageType;

  // Read in the two images we want to register -- called the "Fixed" and "Moving"
  // image by convention.
  typedef itk::ImageFileReader< FixedImageType >  ImageReaderType;

  ImageReaderType::Pointer fixedImageReader = ImageReaderType::New();
  fixedImageReader->SetFileName( argv[1] );
  fixedImageReader->Update();

  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();
  fixedImage->Update();
  fixedImage->DisconnectPipeline();

  ImageReaderType::Pointer movingImageReader = ImageReaderType::New();
  movingImageReader->SetFileName( argv[2] );
  movingImageReader->Update();

  MovingImageType::Pointer movingImage = movingImageReader->GetOutput();
  movingImage->Update();
  movingImage->DisconnectPipeline();

  // The Transform is a parameterized model of motion.
  typedef itk::Euler2DTransform< double > TransformType;
  TransformType::Pointer transform = TransformType::New();
  // The Euler transform is a rotation and translation about a center, so we
  // used the moments of the moving image to find the center.
  typedef itk::ImageMomentsCalculator< MovingImageType > ImageMomentsCalculatorType;
  ImageMomentsCalculatorType::Pointer imageMomentsCalculator = ImageMomentsCalculatorType::New();
  imageMomentsCalculator->SetImage( fixedImage );
  imageMomentsCalculator->Compute();
  const ImageMomentsCalculatorType::VectorType centerOfGravity = imageMomentsCalculator->GetCenterOfGravity();
  MovingImageType::PointType center;
  center[0] = centerOfGravity[0];
  center[1] = centerOfGravity[1];
  transform->SetCenter( center );
  std::cout << "Transform: " << transform << std::endl;
  transform->SetIdentity();

  // The metric is the objective function for the optimization problem.
  //typedef itk::JointHistogramMutualInformationImageToImageMetricv4< FixedImageType, MovingImageType > MetricType;
  typedef itk::MeanSquaresImageToImageMetricv4< FixedImageType, MovingImageType >     MetricType;
  MetricType::Pointer metric = MetricType::New();

  // The optimizer adjusts the parameters of the transform to improve the
  // metric.
  typedef itk::GradientDescentOptimizerv4 OptimizerType;
  //typedef itk::QuasiNewtonOptimizerv4 OptimizerType;
  OptimizerType::Pointer optimizer = OptimizerType::New();
  optimizer->SetNumberOfIterations( atoi( argv[4] ) );
  optimizer->SetDoEstimateLearningRateOnce( false ); //true by default
  optimizer->SetDoEstimateLearningRateAtEachIteration( true );
  optimizer->SetMinimumConvergenceValue( 1e-5 );
  optimizer->SetMaximumStepSizeInPhysicalUnits( 0.5 );
  //optimizer->SetMaximumNewtonStepSizeInPhysicalUnits( 1.5 );

  // The optimizer assumes that the metric is equally sensitive to all transform
  // parameters.  However, that is not true.  This classe determines what good
  // scales should be for the given transform.
  typedef itk::RegistrationParameterScalesFromShift< MetricType > ScalesEstimatorType;
  ScalesEstimatorType::Pointer scalesEstimator = ScalesEstimatorType::New();
  scalesEstimator->SetMetric( metric );
  scalesEstimator->SetTransformForward( true );
  optimizer->SetScalesEstimator( scalesEstimator );
  optimizer->SetDoEstimateScales( true );

  // Print out the optimizer status at every iteration.
  typedef  CommandIterationUpdate< OptimizerType > CommandType;
  CommandType::Pointer observer = CommandType::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  // The RegistrationMethod class coordinates the registration operation.
  // It needs all the pieces that come together to perform the registration
  // operation.
  typedef  itk::ImageRegistrationMethodv4< FixedImageType, MovingImageType, TransformType > RegistrationMethodType;
  RegistrationMethodType::Pointer registrationMethod = RegistrationMethodType::New();
  registrationMethod->SetOptimizer( optimizer );
  registrationMethod->SetFixedImage( fixedImage );
  registrationMethod->SetMovingImage( movingImage );
  registrationMethod->SetMovingInitialTransform( transform );
  registrationMethod->SetNumberOfLevels( 1 );
  registrationMethod->SetMetric( metric );

  try
    {
    std::cout << "Starting registration..." << std::endl;
    registrationMethod->Update();
    std::cout << "Affine parameters after registration: " << std::endl
              << optimizer->GetCurrentPosition() << std::endl;
    }
  catch( itk::ExceptionObject &e )
    {
    std::cerr << "Exception caught: " << e << std::endl;
    return EXIT_FAILURE;
    }

  // Get the moving image after resampling with the transform.
  typedef itk::ResampleImageFilter< MovingImageType, FixedImageType > ResampleFilterType;
  ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  transform->SetParameters( optimizer->GetCurrentPosition() );
  resampler->SetTransform( transform );
  resampler->SetInput( movingImage );
  resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
  resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
  resampler->SetOutputSpacing( fixedImage->GetSpacing() );
  resampler->SetOutputDirection( fixedImage->GetDirection() );
  resampler->SetDefaultPixelValue( 0 );
  resampler->Update();

  // Cast to unsigned char so we save as a PNG.
  typedef unsigned char                                           OutputPixelType;
  typedef itk::Image< OutputPixelType, ImageDimension >           OutputImageType;
  typedef itk::CastImageFilter< FixedImageType, OutputImageType > CastFilterType;
  CastFilterType::Pointer caster = CastFilterType::New();
  caster->SetInput( resampler->GetOutput() );

  typedef itk::ImageFileWriter< OutputImageType >     WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( caster->GetOutput() );

  try
    {
    std::cout << "Writing registered image..." << std::endl;
    writer->Update();
    }
  catch( itk::ExceptionObject &e )
    {
    std::cerr << "Exception caught: " << e << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
