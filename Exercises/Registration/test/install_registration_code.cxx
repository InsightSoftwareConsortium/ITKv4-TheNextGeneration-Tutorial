#!/bin/bash 
# this is just a guide ... should fix paths for your own system
#
# first link the registration directory to the external itk modules directory 
ln -s ITKv4-TheNextGeneration-Tutorial/Exercises/Registration  ITK/Modules/External/ 
# call cmake from the itk binary directory 
cmake ../ITK/
# make the registration examples/tests 
make -j 2 ITKRegistrationRefactoringTestDriver
