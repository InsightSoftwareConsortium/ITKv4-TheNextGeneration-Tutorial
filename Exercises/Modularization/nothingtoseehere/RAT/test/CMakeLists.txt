itk_module_test()
set(ITKRATTests
itkRobustAutomaticThresholdImageFilterTest.cxx
)

CreateTestDriver(ITKRAT  "${ITKRAT-Test_LIBRARIES}" "${ITKRATTests}")

itk_add_test(NAME itkRobustAutomaticThresholdImageFilterTest
      COMMAND ITKRATTestDriver
     --compare Baseline/itkRobustAutomaticThresholdImageFilterTest.png
               ${ITK_TEST_OUTPUT_DIR}/itkRobustAutomaticThresholdImageFilterTestOutput.png
     itkRobustAutomaticThresholdImageFilterTest
              Input/itkRobustAutomaticThresholdImageFilterInput.png
              ${ITK_TEST_OUTPUT_DIR}/itkRobustAutomaticThresholdImageFilterTestOutput.png 2
               )
