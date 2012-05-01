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

#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkRigid2DTransform.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"


#include "itkCommand.h"
class CommandIterationUpdate : public itk::Command
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::RegularStepGradientDescentOptimizer  OptimizerType;
  typedef   const OptimizerType *                   OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    OptimizerPointer optimizer =
      dynamic_cast< OptimizerPointer >( object );
    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "   ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};

int main( int argc, char *argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " fixedImageFile  movingImageFile ";
    std::cerr << " outputImagefile  [differenceAfterRegistration] ";
    std::cerr << " [differenceBeforeRegistration] ";
    std::cerr << " [initialStepLength] "<< std::endl;
    return EXIT_FAILURE;
    }

  const    unsigned int    Dimension = 2;
  typedef  unsigned char   PixelType;

  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  typedef itk::Rigid2DTransform< double > TransformType;

  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

  typedef itk::MeanSquaresImageToImageMetric<
                                    FixedImageType,
                                    MovingImageType >    MetricType;

  typedef itk:: LinearInterpolateImageFunction<
                                    MovingImageType,
                                    double          >    InterpolatorType;

  typedef itk::ImageRegistrationMethod<
                                    FixedImageType,
                                    MovingImageType >    RegistrationType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );

  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );

  typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName(  argv[1] );
  movingImageReader->SetFileName( argv[2] );

  registration->SetFixedImage(    fixedImageReader->GetOutput()    );
  registration->SetMovingImage(   movingImageReader->GetOutput()   );

  fixedImageReader->Update();
  movingImageReader->Update();

  registration->SetFixedImageRegion( fixedImageReader->GetOutput()->GetBufferedRegion() );

  typedef FixedImageType::SpacingType    SpacingType;
  typedef FixedImageType::PointType      OriginType;
  typedef FixedImageType::RegionType     RegionType;
  typedef FixedImageType::SizeType       SizeType;


  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

  const SpacingType fixedSpacing = fixedImage->GetSpacing();
  const OriginType  fixedOrigin  = fixedImage->GetOrigin();
  const RegionType  fixedRegion  = fixedImage->GetLargestPossibleRegion();
  const SizeType    fixedSize    = fixedRegion.GetSize();

  TransformType::InputPointType centerFixed;

  centerFixed[0] = fixedOrigin[0] + fixedSpacing[0] * fixedSize[0] / 2.0;
  centerFixed[1] = fixedOrigin[1] + fixedSpacing[1] * fixedSize[1] / 2.0;

  MovingImageType::Pointer movingImage = movingImageReader->GetOutput();

  const SpacingType movingSpacing = movingImage->GetSpacing();
  const OriginType  movingOrigin  = movingImage->GetOrigin();
  const RegionType  movingRegion  = movingImage->GetLargestPossibleRegion();
  const SizeType    movingSize    = movingRegion.GetSize();

  TransformType::InputPointType centerMoving;

  centerMoving[0] = movingOrigin[0] + movingSpacing[0] * movingSize[0] / 2.0;
  centerMoving[1] = movingOrigin[1] + movingSpacing[1] * movingSize[1] / 2.0;

  transform->SetCenter( centerFixed );
  transform->SetTranslation( centerMoving - centerFixed );

  transform->SetAngle( 0.0 );

  registration->SetInitialTransformParameters( transform->GetParameters() );



  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );
  const double translationScale = 1.0 / 1000.0;

  optimizerScales[0] = 1.0;
  optimizerScales[1] = translationScale;
  optimizerScales[2] = translationScale;

  optimizer->SetScales( optimizerScales );


  double initialStepLength = 0.1;

  if( argc > 6 )
    {
    initialStepLength = atof( argv[6] );
    }

  optimizer->SetRelaxationFactor( 0.9 );
  optimizer->SetMaximumStepLength( initialStepLength );
  optimizer->SetMinimumStepLength( 0.0001 );
  optimizer->SetNumberOfIterations( 200 );


  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  try
    {
    registration->Update();
    std::cout << "Optimizer stop condition: "
              << registration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  OptimizerType::ParametersType finalParameters =
                    registration->GetLastTransformParameters();

  const double finalAngle           = finalParameters[0];
  const double finalTranslationX    = finalParameters[1];
  const double finalTranslationY    = finalParameters[2];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();


  // Print out results
  //
  const double finalAngleInDegrees = finalAngle * 180.0 / vnl_math::pi;

  std::cout << "Result = " << std::endl;
  std::cout << " Angle (radians)   = " << finalAngle  << std::endl;
  std::cout << " Angle (degrees)   = " << finalAngleInDegrees  << std::endl;
  std::cout << " Translation X = " << finalTranslationX  << std::endl;
  std::cout << " Translation Y = " << finalTranslationY  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;




  typedef itk::ResampleImageFilter<
                            MovingImageType,
                            FixedImageType >    ResampleFilterType;

  TransformType::Pointer finalTransform = TransformType::New();

  finalTransform->SetParameters( finalParameters );
  finalTransform->SetFixedParameters( transform->GetFixedParameters() );

  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  std::cout << "Final Transform" << std::endl;
  finalTransform->Print( std::cout );

  resample->SetTransform( finalTransform );
  resample->SetInput( movingImageReader->GetOutput() );

  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );
  resample->SetDefaultPixelValue( 100 );

  typedef itk::ImageFileWriter< FixedImageType >  WriterFixedType;

  WriterFixedType::Pointer      writer =  WriterFixedType::New();

  writer->SetFileName( argv[3] );

  writer->SetInput( resample->GetOutput()   );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "ExceptionObject while writing the resampled image !" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image< float, Dimension > DifferenceImageType;

  typedef itk::SubtractImageFilter<
                           FixedImageType,
                           FixedImageType,
                           DifferenceImageType > DifferenceFilterType;

  DifferenceFilterType::Pointer difference = DifferenceFilterType::New();

  typedef  unsigned char  OutputPixelType;

  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

  typedef itk::RescaleIntensityImageFilter<
                                  DifferenceImageType,
                                  OutputImageType >   RescalerType;

  RescalerType::Pointer intensityRescaler = RescalerType::New();

  intensityRescaler->SetOutputMinimum(   0 );
  intensityRescaler->SetOutputMaximum( 255 );

  difference->SetInput1( fixedImageReader->GetOutput() );
  difference->SetInput2( resample->GetOutput() );

  resample->SetDefaultPixelValue( 1 );

  intensityRescaler->SetInput( difference->GetOutput() );

  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  WriterType::Pointer      writer2 =  WriterType::New();

  writer2->SetInput( intensityRescaler->GetOutput() );


  try
    {
    // Compute the difference image between the
    // fixed and moving image after registration.
    if( argc > 4 )
      {
      writer2->SetFileName( argv[4] );
      writer2->Update();
      }

    // Compute the difference image between the
    // fixed and resampled moving image after registration.
    TransformType::Pointer identityTransform = TransformType::New();
    identityTransform->SetIdentity();
    resample->SetTransform( identityTransform );
    if( argc > 5 )
      {
      writer2->SetFileName( argv[5] );
      writer2->Update();
      }
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error while writing difference images" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }



  return EXIT_SUCCESS;
}
