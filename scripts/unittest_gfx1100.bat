cd ..\UnitTest\bitcodes
call generate_bitcodes_gfx1100.bat
cd ..\..\scripts
..\dist\bin\Release\Unittest64 --gtest_filter=-*getErrorString* --gtest_output=xml:../result.xml
