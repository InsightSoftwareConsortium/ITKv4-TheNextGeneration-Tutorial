#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"

#include "itkRobustAutomaticThresholdImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkMorphologicalGradientImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

int main(int argc, char * argv[])
{

  if( argc != 4 )
    {
    std::cerr << "usage: " << argv[0] << " inputImage outputImage pow" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
  typedef unsigned short PType;
  typedef itk::Image< PType, dim > IType;

  typedef float RPType;
  typedef itk::Image< RPType, dim > RIType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
/*  typedef itk::BinaryBallStructuringElement< bool, dim > KernelType;
  KernelType kernel;
  KernelType::RadiusType rad;
  rad.Fill(2);
  kernel.SetRadius( rad );
  kernel.CreateStructuringElement();
  
  typedef itk::MorphologicalGradientImageFilter< IType, RIType, KernelType > GradientType;
  GradientType::Pointer gradient = GradientType::New();
  gradient->SetInput( reader->GetOutput() );
  gradient->SetKernel( kernel );*/
//  gradient->Update();
//  std::cout << gradient->GetOutput()->GetSpacing() << std::endl;
  
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< IType, RIType > GradientType;
  GradientType::Pointer gradient = GradientType::New();
  gradient->SetInput( reader->GetOutput() );
  gradient->SetSigma( 10 );
  gradient->Update();

  typedef itk::RobustAutomaticThresholdImageFilter< IType, RIType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetGradientImage( gradient->GetOutput() );
  filter->SetPow( atof(argv[3]) );

  itk::SimpleFilterWatcher watcher(filter, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
//   writer->SetInput( gradient->GetOutput() );
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();

  return 0;
}

