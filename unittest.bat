
::@echo off
@setlocal EnableDelayedExpansion

@cls

@Set /a UNITTESTNBR=0

@set currentdir=%cd%
@REM @echo %currentdir%
@REM @echo.


@set convm="G:\Collector\_Apple IIgs\_DiskCrackBand_\tools\convm\x64\Debug\convm.exe"
@if not exist %convm% ( 
	@set convm="G:\Collector\_Apple IIgs\_DiskCrackBand_\tools\convm\x64\Release\convm.exe"
	@if not exist %convm% ( 
		@set convm="%currentdir%\..\convm.exe"
	)
)

@REM TEST : not enough parameters
@%convm%
@if %errorlevel% == 0 (
  @goto error
)
@set /a UNITTESTNBR=%UNITTESTNBR%+1

@REM TEST : illegal convmspec parameter
@echo.
@%convm% -toto "%currentdir%\tartemolle.scr"
@if %errorlevel% == 0 (
  @goto error
)
@set /a UNITTESTNBR=%UNITTESTNBR%+1

@REM TEST : illegal option parameter
@echo.
@%convm% -RlEd +upper "%currentdir%\tartemolle.shr"
@if %errorlevel% == 0 (
  @goto error
)
@set /a UNITTESTNBR=%UNITTESTNBR%+1

@REM TEST : input file does not exist
@echo.
@%convm% -RlEc "%currentdir%\tartemolle.pic"
@if %errorlevel% == 0 (
  @goto error
)
@set /a UNITTESTNBR=%UNITTESTNBR%+1

@REM TEST : size equal to 0 for file
@echo.
@type NUL > "%currentdir%\tartemolle.pnt"
@%convm% -RlEc "%currentdir%\tartemolle.pnt"
@if %errorlevel% == 0 (
  @goto error
)
@del "%currentdir%\tartemolle.pnt"
@set /a UNITTESTNBR=%UNITTESTNBR%+1

@REM TEST : file without extension
@echo.
@copy /Y "G:\Collector\_Apple IIgs\_DiskCrackBand_\Iron_Lord\dessin\TAVERNE.pic" /B "%currentdir%\TAVERNE" /B >nul
@%convm% -RlEc "%currentdir%\TAVERNE"
@if %errorlevel% == 0 (
  @goto error
)
@del "%currentdir%\TAVERNE"
@set /a UNITTESTNBR=%UNITTESTNBR%+1

@REM TEST : file extension not supported
@echo.
@copy /Y "G:\Collector\_Apple IIgs\_DiskCrackBand_\Iron_Lord\dessin\TAVERNE.pic" /B "%currentdir%\TAVERNE.tst" /B >nul
@%convm% -RlEc "%currentdir%\TAVERNE.tst"
@if %errorlevel% == 0 (
  @goto error
)
@del "%currentdir%\TAVERNE.tst"
@set /a UNITTESTNBR=%UNITTESTNBR%+1

@REM TEST : file extension not supported
@echo.
@copy /Y "G:\Collector\_Apple IIgs\_DiskCrackBand_\Iron_Lord\dessin\SORCIER.shr" /B "%currentdir%\SORCIER.abc" /B >nul
@%convm% -RlEc "SORCIER.abc"
@if %errorlevel% == 0 (
  @goto error
)
@del "%currentdir%\SORCIER.abc"
@set /a UNITTESTNBR=%UNITTESTNBR%+1

@echo.
@copy /Y "G:\Collector\_Apple IIgs\_DiskCrackBand_\Iron_Lord\dessin\CHATEAU.shr" /B "%currentdir%\CHATEAU.shr" /B >nul
@%convm% -RlEc "%currentdir%\CHATEAU.shr"
@if %errorlevel% == 0 (
  @goto error
)
@del "%currentdir%\CHATEAU.shr"
@set /a UNITTESTNBR=%UNITTESTNBR%+1

@echo.
@copy /Y "G:\Collector\_Apple IIgs\_DiskCrackBand_\Iron_Lord\dessin\carte.pic" /B "%currentdir%\carte.pic" /B >nul
@%convm% -RlEd "%currentdir%\carte.pic"
@if %errorlevel% == 0 (
  @goto error
)
@del "%currentdir%\carte.pic"
@set /a UNITTESTNBR=%UNITTESTNBR%+1


@echo.
@echo.
@echo   %UNITTESTNBR% Unit tests from batch script are on SUCCESS !
@set errorlevel=0
@goto cleaner

:error
@echo.
@echo.
@echo Unit test failed at test %UNITTESTNBR%

:cleaner

@Endlocal
:: end of script
@echo.
@REM @echo Done.

@REM @pause

@exit /b %ERRORLEVEL%