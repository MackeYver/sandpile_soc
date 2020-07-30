@ECHO OFF
SETLOCAL ENABLEEXTENSIONS
SETLOCAL ENABLEDELAYEDEXPANSION
cls

SET Name="sandpile"
SET BuildMode="debug"

IF "%1"=="debug"    SET BuildMode="debug"
IF "%1"=="release"  SET BuildMode="release"
IF "%2"=="debug"    SET BuildMode="debug"
IF "%2"=="release"  SET BuildMode="release"

IF %errorlevel% NEQ 0 EXIT /b %errorlevel%



REM ----------------------------------------------------------------------------------
REM Compiler Options
REM https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=vs-2017

SET IgnoredWarnings=/wd4100 /wd4201 /wd4505
SET GeneralCompilerOptions=/nologo /DWIN32=1 /MP /fp:fast /fp:except- /EHsc /Gm- /Oi /WX /W4 /Zi !IgnoredWarnings!


IF %BuildMode%=="release" (
  ECHO Compiling in release configuration.
  SET CompilerOptions=/O2 /DRELEASE=1 !GeneralCompilerOptions!
)
IF  %BuildMode%=="debug" (
  ECHO Compiling in debug configuration.
  SET CompilerOptions=/Od /MTd /DDEBUG=1 !GeneralCompilerOptions!
)

REM MP         Build with Multiple Processes
REM Ox		 Code generation x E [d = Debug, 1 = small code, 2 = fast code]
REM fp:fast    Fast floating point code generated
REM fp:except- No floating point exceptions
REM EHsc       Catches C++ exceptions only
REM GM-		Enables minimal rebuild (- disables it, we want all files compiled all the time)
REM Zi		 Produces separate PDB file that contains all the symbolic debugging information for use with the debugger
REM Zo		 Generate enhanced debugging information for optimized code in non-debug builds (enabled with Zi)
REM Oi		 Generates intrinsic functions.
REM WX		 Treats all warnings as errors
REM W4		 All warnings
REM wx		 Except...
REM			  4100 'identifier': unreferenced formal parameter
REM			  4201 nonstandard extension used: nameless struct/union
REM			  4505 unreferenced local function


REM ----------------------------------------------------------------------------------
REM Linker Options
REM https://docs.microsoft.com/en-us/cpp/build/reference/linker-options?view=vs-2017

IF %BuildMode%=="debug" (
SET LinkerOptions=/DEBUG:FULL /OPT:NOREF /OPT:NOICF /PROFILE
) ELSE (
SET LinkerOptions=/OPT:REF /OPT:ICF
)

REM
REM We are setting /OPT for the linker explicitily just to be clear about what we are doing.
REM (OPT:REF and OPT:ICF is default for the linker and when using /DEBUG it changes to /OPT:NOREF and /OPT:NOICF)
REM
REM DEBUG:FULL     single PDB, also changes to opt:noref and opt:noicf
REM OPT:NOREF      keeps functions and data that are never referenced
REM OPT:NOICF      does not perform identical COMDAT folding


REM ----------------------------------------------------------------------------------
REM Build

IF NOT EXIST ..\build mkdir ..\build
PUSHD ..\build

ECHO Removing all old files...
del /Q *.*

ECHO Building...
cl %CompilerOptions% ../src/*.cpp /link /SUBSYSTEM:console %LinkerOptions% /out:%Name%.exe

IF %errorlevel% NEQ 0 (
  popd
  EXIT /b %errorlevel%
)

IF NOT EXIST ..\run mkdir ..\run
move %Name%.exe ../run

IF %errorlevel% NEQ 0 (
  popd
  EXIT /b %errorlevel%
)

ECHO All done.
POPD