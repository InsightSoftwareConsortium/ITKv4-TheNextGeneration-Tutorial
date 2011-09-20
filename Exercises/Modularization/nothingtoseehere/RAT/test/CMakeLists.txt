itk_module_test()
set(ITKRATTests
itkRobustAutomaticThresholdImageFilterTest.cxx
)

CreateTestDriver(ITKRAT  "${ITKRAT-Test_LIBRARIES}" "${ITKRATTests}")

itk_add_test(NAME itkRobustAutomaticThresholdImageFilterTest
      COMMAND ITKRATTestDriver
      --compare ${CMAKE_CURRENT_SOURCE_DIR}/Baseline/itkRobustAutomaticThresholdImageFilterTest.png
                ${ITK_TEST_OUTPUT_DIR}/itkRobustAutomaticThresholdImageFilterTestOutput.png
     itkRobustAutomaticThresholdImageFilterTest
              ${CMAKE_CURRENT_SOURCE_DIR}/Input/itkRobustAutomaticThresholdImageFilterInput.png
              ${ITK_TEST_OUTPUT_DIR}/itkRobustAutomaticThresholdImageFilterTestOutput.png 2
               )
