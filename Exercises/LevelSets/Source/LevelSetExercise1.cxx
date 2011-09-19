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

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkLevelSetDomainMapImageFilter.h"
#include "itkLevelSetContainerBase.h"
#include "itkLevelSetEquationChanAndVeseInternalTerm.h"
#include "itkLevelSetEquationChanAndVeseExternalTerm.h"
#include "itkLevelSetEquationTermContainerBase.h"
#include "itkLevelSetEquationContainerBase.h"
#include "itkSinRegularizedHeavisideStepFunction.h"
#include "itkLevelSetSparseEvolutionBase.h"
#include "itkBinaryImageToWhitakerSparseLevelSetAdaptor.h"
#include "itkLevelSetEvolutionNumberOfIterationsStoppingCriterion.h"
#include "itkNumericTraits.h"

#include "itkWhitakerCommandIterationUpdate.h"
#include "itkShiCommandIterationUpdate.h"

// ------------------------------------------------------------------------
//
// Exercise: Add a curvature term for regularization
// coefficient for the new term will be provided as argument to the 
// executable
//
// ------------------------------------------------------------------------


int main( int argc, char* argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Missing Arguments" << std::endl;
    std::cerr << "./LevelSetExercise1 " <<std::endl;
    std::cerr << "1- Input Image" <<std::endl;
    std::cerr << "2- Number of Iterations" <<std::endl;
    std::cerr << "3- Curvature Term coefficient" <<std::endl;
    std::cerr << "4- Visualization (0 or 1)" <<std::endl;
    std::cerr << "5- Output" <<std::endl;

    return EXIT_FAILURE;
    }

  // Image Dimension
  const unsigned int Dimension = 2;

  typedef unsigned char                                     InputPixelType;
  typedef itk::Image< InputPixelType, Dimension >           InputImageType;

  // Read input image (to be processed).
  typedef itk::ImageFileReader< InputImageType >            ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();
  InputImageType::Pointer input = reader->GetOutput();

  // Generate a binary mask that will be used as initialization
  // of the level set evolution.
  InputImageType::Pointer binary = InputImageType::New();
  binary->SetRegions( input->GetLargestPossibleRegion() );
  binary->CopyInformation( input );
  binary->Allocate();
  binary->FillBuffer( itk::NumericTraits<InputPixelType>::Zero );

  InputImageType::RegionType region;
  InputImageType::IndexType index;
  InputImageType::SizeType size;

  index.Fill( 5 );
  size.Fill( 120 );

  region.SetIndex( index );
  region.SetSize( size );

  typedef itk::ImageRegionIteratorWithIndex< InputImageType > InputIteratorType;
  InputIteratorType iIt( binary, region );
  iIt.GoToBegin();
  while( !iIt.IsAtEnd() )
    {
    iIt.Set( itk::NumericTraits<InputPixelType>::One );
    ++iIt;
    }

  // Convert the binary mask into a level set function.
  // Here the output level-set will be a "Whitaker" sparse level-set;
  // i.e. only few layers {-2, -1, 0, +1, +2 } around the zero-set are
  // maintained, the rest of the domain is either -3 or +3.
  typedef float PixelType;

  typedef itk::BinaryImageToWhitakerSparseLevelSetAdaptor< InputImageType,
    PixelType > BinaryToSparseAdaptorType;

  BinaryToSparseAdaptorType::Pointer adaptor = BinaryToSparseAdaptorType::New();
  adaptor->SetInputImage( binary );
  adaptor->Initialize();
  std::cout << "Finished converting to sparse format" << std::endl;

  // Here get the resulting level-set function
  typedef BinaryToSparseAdaptorType::LevelSetType SparseLevelSetType;

  SparseLevelSetType::Pointer level_set = adaptor->GetSparseLevelSet();

  // Create here the bounds in which this level-set can evolved.

  // There is only one level-set, so we fill 1 list with only one element which
  // correspondongs to the level-set identifier.
  typedef itk::IdentifierType         IdentifierType;
  typedef std::list< IdentifierType > IdListType;

  IdListType list_ids;
  list_ids.push_back( 1 );

  // We create one image where for each pixel we provide which level-set exists.
  // In this example the first level-set is defined on the whole image.
  typedef itk::Image< IdListType, Dimension >               IdListImageType;
  IdListImageType::Pointer id_image = IdListImageType::New();
  id_image->SetRegions( input->GetLargestPossibleRegion() );
  id_image->Allocate();
  id_image->FillBuffer( list_ids );

  typedef itk::Image< short, Dimension >                     CacheImageType;
  typedef itk::LevelSetDomainMapImageFilter< IdListImageType, CacheImageType >
                                                            DomainMapImageFilterType;
  DomainMapImageFilterType::Pointer domainMapFilter = DomainMapImageFilterType::New();
  domainMapFilter->SetInput( id_image );
  domainMapFilter->Update();
  std::cout << "Domain map computed" << std::endl;

  // Define the Heaviside function
  typedef SparseLevelSetType::OutputRealType LevelSetOutputRealType;

  typedef itk::SinRegularizedHeavisideStepFunction< LevelSetOutputRealType,
      LevelSetOutputRealType > HeavisideFunctionBaseType;
  HeavisideFunctionBaseType::Pointer heaviside = HeavisideFunctionBaseType::New();
  heaviside->SetEpsilon( 1.0 );

  // Insert the levelsets in a levelset container
  typedef itk::LevelSetContainerBase< IdentifierType, SparseLevelSetType >
      LevelSetContainerType;

  LevelSetContainerType::Pointer lscontainer = LevelSetContainerType::New();
  lscontainer->SetHeaviside( heaviside );
  lscontainer->SetDomainMapFilter( domainMapFilter );

  lscontainer->AddLevelSet( 0, level_set );

  std::cout << "Level set container created" << std::endl;

  // **************** CREATE ALL TERMS ****************

  // Create ChanAndVese internal term for phi
  typedef itk::LevelSetEquationChanAndVeseInternalTerm< InputImageType,
      LevelSetContainerType > ChanAndVeseInternalTermType;

  ChanAndVeseInternalTermType::Pointer cvInternalTerm0 = ChanAndVeseInternalTermType::New();
  cvInternalTerm0->SetInput( input );
  cvInternalTerm0->SetCoefficient( 1.0 );
  cvInternalTerm0->SetCurrentLevelSetId( 0 );
  cvInternalTerm0->SetLevelSetContainer( lscontainer );
  std::cout << "Chan and Vese internal term created" << std::endl;

  // Create ChanAndVese external term for phi
  typedef itk::LevelSetEquationChanAndVeseExternalTerm< InputImageType,
      LevelSetContainerType > ChanAndVeseExternalTermType;

  ChanAndVeseExternalTermType::Pointer cvExternalTerm0 = ChanAndVeseExternalTermType::New();
  cvExternalTerm0->SetInput( input );
  cvExternalTerm0->SetCoefficient( 1.0 );
  cvExternalTerm0->SetCurrentLevelSetId( 0 );
  cvExternalTerm0->SetLevelSetContainer( lscontainer );
  std::cout << "Chan and Vese external term created" << std::endl;

  double CurvatureTermCoefficient = atof( argv[3] );
  std::cout <<"CurvatureTermCoefficient : " 
            << CurvatureTermCoefficient <<std::endl;

  // here are the curvature term!

  // **************** CREATE ALL EQUATIONS ****************

  // Create Term Container which corresponds to the combination of terms in the PDE.
  typedef itk::LevelSetEquationTermContainerBase< InputImageType, LevelSetContainerType >
                                                            TermContainerType;
  TermContainerType::Pointer termContainer0 = TermContainerType::New();
  termContainer0->SetInput( input );

  termContainer0->AddTerm( 0, cvInternalTerm0 );
  termContainer0->AddTerm( 1, cvExternalTerm0 );

  std::cout << "Term container 0 created" << std::endl;

  typedef itk::LevelSetEquationContainerBase< TermContainerType >
                                                            EquationContainerType;
  EquationContainerType::Pointer equationContainer = EquationContainerType::New();
  equationContainer->AddEquation( 0, termContainer0 );

  typedef itk::LevelSetEvolutionNumberOfIterationsStoppingCriterion< LevelSetContainerType >
      StoppingCriterionType;
  StoppingCriterionType::Pointer criterion = StoppingCriterionType::New();
  criterion->SetNumberOfIterations( atoi( argv[2]) );

  typedef itk::LevelSetSparseEvolutionBase< EquationContainerType >
                                                            LevelSetEvolutionType;

  LevelSetEvolutionType::Pointer evolution = LevelSetEvolutionType::New();

  typedef itk::WhitakerCommandIterationUpdate< LevelSetEvolutionType > CommandType;

  CommandType::Pointer observer = CommandType::New();

  if( atoi( argv[4] ) == 1 )
    {
    evolution->AddObserver( itk::IterationEvent(), observer );
    }

  evolution->SetEquationContainer( equationContainer );
  evolution->SetStoppingCriterion( criterion );
  evolution->SetLevelSetContainer( lscontainer );

  try
    {
    evolution->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image< char, Dimension > OutputImageType;
  OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage->SetRegions( input->GetLargestPossibleRegion() );
  outputImage->CopyInformation( input );
  outputImage->Allocate();
  outputImage->FillBuffer( 0 );

  typedef itk::ImageRegionIteratorWithIndex< OutputImageType > OutputIteratorType;
  OutputIteratorType oIt( outputImage, outputImage->GetLargestPossibleRegion() );
  oIt.GoToBegin();

  OutputImageType::IndexType idx;

  while( !oIt.IsAtEnd() )
    {
    idx = oIt.GetIndex();
    oIt.Set( level_set->GetLabelMap()->GetPixel(idx) );
    ++oIt;
    }

  typedef itk::ImageFileWriter< OutputImageType >     OutputWriterType;
  OutputWriterType::Pointer writer = OutputWriterType::New();
  writer->SetFileName( argv[5] );
  writer->SetInput( outputImage );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    }

  return EXIT_SUCCESS;
}
