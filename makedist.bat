# this batch file takes the binary files and builds a dist folder containing all
# that should usually be included in an incalang distribution

rmdir dist
mkdir dist
copy development\projects\windows_vc\Inca\Release\Inca.exe dist\Inca.exe
mkdir dist\Data
copy development\projects\windows_vc\IncaVM\Release\IncaVM.exe dist\Data\IncaVM.exe
copy development\projects\windows_vc\IncaVM\IncaVMLibrary\system.txt dist\Data\system.txt
xcopy /E development\samples dist\Samples\
copy development\manual\manual.pdf dist\Manual.pdf