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

#ifndef __vtkVisualize2DWhitakerLevelSetLayers_h
#define __vtkVisualize2DWhitakerLevelSetLayers_h

#include "itkLightObject.h"

#include "itkWhitakerSparseLevelSetImage.h"

#include "itkImageToRGBVTKImageFilter.h"
#include "itkWhitakerLevelSetTovtkImageData.h"

#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkMarchingSquares.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageActor.h"
#include "vtkScalarBarActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkImageShiftScale.h"

#include "vtkCaptureScreen.h"
#include "vtkPNGWriter.h"

template< class TInputImage, typename TOutput, unsigned int VDimension >
class vtkVisualize2DWhitakerLevelSetLayers : public itk::LightObject
{
public:
  typedef vtkVisualize2DWhitakerLevelSetLayers  Self;
  typedef LightObject                           Superclass;
  typedef itk::SmartPointer< Self >             Pointer;
  typedef itk::SmartPointer< const Self >       ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(vtkVisualize2DWhitakerLevelSetLayers, LightObject);

  typedef TInputImage                         InputImageType;
  typedef typename InputImageType::PixelType  InputPixelType;

  typedef itk::WhitakerSparseLevelSetImage< TOutput, VDimension > LevelSetType;
  typedef typename LevelSetType::Pointer                          LevelSetPointer;

  typedef itk::ImageToRGBVTKImageFilter< InputImageType >  ConverterType;
  typedef typename ConverterType::Pointer                  ConverterPointer;

  void SetInputImage( const InputImageType * iImage )
    {
    m_ImageConverter->SetInput( iImage );
    try
      {
      m_ImageConverter->Update();
      }
    catch( itk::ExceptionObject& e )
      {
      std::cout << e << std::endl;
      return;
      }

    m_Count = 0;
    }

  void SetLevelSet( LevelSetType *f )
    {
    m_LevelSet = f;
    m_Count = 0;
    }

  void SetScreenCapture( const bool& iCapture )
    {
    m_ScreenCapture = iCapture;
    }

  void Update()
    {
    vtkSmartPointer< vtkImageData > VTKImage = m_ImageConverter->GetOutput();

    typedef typename LevelSetType::LayerType          LayerType;
    typedef typename LevelSetType::LayerConstIterator LayerConstIterator;

    LayerType layer = m_LevelSet->GetLayer( LevelSetType::MinusTwoLayer() );

    LayerConstIterator it = layer.begin();

    while( it != layer.end() )
      {
      typename InputImageType::IndexType idx = it->first;
      InputPixelType* vtkpixel =
          static_cast< InputPixelType* >( VTKImage->GetScalarPointer( idx[0], idx[1], 0 ) );
      vtkpixel[0] = 0;
      vtkpixel[1] = 255;
      vtkpixel[2] = 0;
      ++it;
      }

    layer = m_LevelSet->GetLayer( LevelSetType::MinusOneLayer() );

    it = layer.begin();

    while( it != layer.end() )
      {
      typename InputImageType::IndexType idx = it->first;
      InputPixelType* vtkpixel =
          static_cast< InputPixelType* >( VTKImage->GetScalarPointer( idx[0], idx[1], 0 ) );
      vtkpixel[0] = 255;
      vtkpixel[1] = 255;
      vtkpixel[2] = 0;
      ++it;
      }

    layer = m_LevelSet->GetLayer( LevelSetType::ZeroLayer() );

    it = layer.begin();

    while( it != layer.end() )
      {
      typename InputImageType::IndexType idx = it->first;
      InputPixelType* vtkpixel =
          static_cast< InputPixelType* >( VTKImage->GetScalarPointer( idx[0], idx[1], 0 ) );
      vtkpixel[0] = 255;
      vtkpixel[1] = 0;
      vtkpixel[2] = 0;
      ++it;
      }

    layer = m_LevelSet->GetLayer( LevelSetType::PlusOneLayer() );

    it = layer.begin();

    while( it != layer.end() )
      {
      typename InputImageType::IndexType idx = it->first;
      InputPixelType* vtkpixel =
          static_cast< InputPixelType* >( VTKImage->GetScalarPointer( idx[0], idx[1], 0 ) );
      vtkpixel[0] = 0;
      vtkpixel[1] = 255;
      vtkpixel[2] = 255;
      ++it;
      }

    layer = m_LevelSet->GetLayer( LevelSetType::PlusTwoLayer() );

    it = layer.begin();

    while( it != layer.end() )
      {
      typename InputImageType::IndexType idx = it->first;
      InputPixelType* vtkpixel =
          static_cast< InputPixelType* >( VTKImage->GetScalarPointer( idx[0], idx[1], 0 ) );
      vtkpixel[0] = 0;
      vtkpixel[1] = 0;
      vtkpixel[2] = 255;
      ++it;
      }

//    vtkSmartPointer< vtkLookupTable > lut =
//        vtkSmartPointer< vtkLookupTable >::New();
//    lut->SetNumberOfTableValues( 5 );
//    lut->SetRange( -2., 2. );
//    lut->SetTableValue( 0, 0., 1., 0. );
//    lut->SetTableValue( 1, 1., 1., 0. );
//    lut->SetTableValue( 2, 1., 0., 0. );
//    lut->SetTableValue( 3, 1., 0., 1. );
//    lut->SetTableValue( 4, 0., 0., 1. );
//    lut->Build();


//    vtkSmartPointer< vtkScalarBarActor > scalarbar =
//        vtkSmartPointer< vtkScalarBarActor >::New();
//    scalarbar->SetTitle( "Layers" );
//    scalarbar->SetNumberOfLabels( 5 );
//    scalarbar->SetLookupTable( lut );

    vtkSmartPointer< vtkImageActor > input_Actor =
        vtkSmartPointer< vtkImageActor >::New();
    input_Actor->SetInput( VTKImage );
    input_Actor->InterpolateOff();

    vtkSmartPointer< vtkRenderer > ren =
        vtkSmartPointer< vtkRenderer >::New();
    ren->SetBackground( 0.5, 0.5, 0.5 );

    vtkSmartPointer< vtkRenderWindowInteractor > iren =
        vtkSmartPointer< vtkRenderWindowInteractor >::New();

    vtkSmartPointer< vtkRenderWindow > renWin =
        vtkSmartPointer< vtkRenderWindow >::New();

    ren->AddActor ( input_Actor );
//    ren->AddActor2D( scalarbar );

    iren->SetRenderWindow( renWin );

    renWin->AddRenderer( ren );
    renWin->Render();

    if( m_ScreenCapture )
      {
      std::string filename;
      std::stringstream yo;
      yo << "snapshot_" << m_Count;
      filename = yo.str();
      filename.append ( ".png" );

      vtkCaptureScreen< vtkPNGWriter > capture ( renWin );
      // begin mouse interaction
//      iren->Start();
      capture( filename );
      ++m_Count;
      }
    else
      {
      iren->Start();
      }
    }

protected:
  vtkVisualize2DWhitakerLevelSetLayers() : Superclass(),
    m_Count( 0 ),
    m_NumberOfLevels( 1 ),
    m_LevelLimit( 0 ),
    m_ScreenCapture( false )
    {
    m_ImageConverter = ConverterType::New();
    }

  ~vtkVisualize2DWhitakerLevelSetLayers()
    {}

private:
  vtkVisualize2DWhitakerLevelSetLayers ( const Self& );
  void operator = ( const Self& );

  ConverterPointer  m_ImageConverter;
  LevelSetPointer   m_LevelSet;

  itk::IdentifierType m_Count;
  unsigned int        m_NumberOfLevels;
  double              m_LevelLimit;
  bool                m_ScreenCapture;

};
#endif
