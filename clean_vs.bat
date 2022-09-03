
::@echo off
@setlocal EnableDelayedExpansion

@cls

@set currentdir=%cd%
@echo %currentdir%
@REM @echo.
@REM display the file name from a pathname
@REM cd /d "%~dp0"

@REM get the last folder of a pathname
@for %%f in (%currentdir%) do @set myfolder=%%~nxf
@REM @echo %myfolder%

@if exist "%currentdir%\%myfolder%.sln" ( 
	@echo.
	@echo Deleting directories
	@if exist "%currentdir%\.vs\%myfolder%\v16" (
		@rmdir /s /q "%currentdir%\.vs\%myfolder%\v16"
	)
	@if exist "%currentdir%\%myfolder%\Debug" (
		@rmdir /s /q "%currentdir%\%myfolder%\Debug"
	)
	@if exist "%currentdir%\%myfolder%\Release" (
		@rmdir /s /q "%currentdir%\%myfolder%\Release"
	)
	@if exist "%currentdir%\%myfolder%\x64" (
		@rmdir /s /q "%currentdir%\%myfolder%\x64"
	)
	@if exist "%currentdir%\Debug" (
		@rmdir /s /q "%currentdir%\Debug"
	)
	@if exist "%currentdir%\Release" (
		@rmdir /s /q "%currentdir%\Release"
	)
	@echo.
	@if exist "%currentdir%\x64\Release\%myfolder%.exe" (	
		@echo Copy release to up directory
		@copy /Y "%currentdir%\x64\Release\%myfolder%.exe" /B "%currentdir%\..\%myfolder%.exe" /B >nul
	)
	@echo.
	@if exist "%currentdir%\x64" (
		@echo Delete directory
		@rmdir /s /q "%currentdir%\x64"
	)
)

:cleaner

@Endlocal
:: end of script
@echo.
@echo Done.

@REM @pause

@exit /b