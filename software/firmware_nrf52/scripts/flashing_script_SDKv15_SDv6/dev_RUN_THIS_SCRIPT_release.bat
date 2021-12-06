@echo off

:: Define app configuration ("debug" or "release")
set APP_CONFIG=release
echo ------ APP_CONFIG = %APP_CONFIG% ------

:: Define file paths
::set SOFTDEVICE="..\..\nRF5_SDK_15.0.0_a53641a\components\softdevice\s132\hex\s132_nrf52_6.0.0_softdevice.hex"
set SOFTDEVICE="..\..\nRF5_SDK_15.0.0_a53641a\components\softdevice\s132\hex\s132_nrf52_6.1.0_softdevice.hex"
set BL_RELEASE="..\..\nRF5_SDK_15.0.0_a53641a\examples\dfu\secure_bootloader\pca10040_ble\ses\Output\Release\Exe\secure_bootloader_ble_s132_pca10040.hex"
set APP_RELEASE="..\..\nRF5_SDK_15.0.0_a53641a\examples\ble_peripheral\ellie-fw-v2_0\pca10040\s132\ses\Output\Release\Exe\ellie-fw-v2_0.hex"
set APP_DEBUG="..\..\nRF5_SDK_15.0.0_a53641a\examples\ble_peripheral\ellie-fw-v2_0\pca10040\s132\ses\Output\Debug\Exe\ellie-fw-v2_0.hex"

:: Copy latest build hex files to local directory
xcopy /y %SOFTDEVICE% softdevice.hex
xcopy /y %BL_RELEASE% bootloader.hex
IF /i "%APP_CONFIG%"=="release" xcopy /y %APP_RELEASE% app.hex
IF /i "%APP_CONFIG%"=="debug" xcopy /y %APP_DEBUG% app.hex

:: Recover BMD-300
nrfjprog -f NRF52 --recover
IF %ERRORLEVEL% NEQ 0 (
  GOTO FLASH_FAIL
)

:: Generate bootloader settings
nrfutil settings generate --family NRF52 --application app.hex --application-version 3 --bootloader-version 1 --bl-settings-version 1 bootloader_setting.hex
IF %ERRORLEVEL% NEQ 0 (
  GOTO FLASH_FAIL
)

:: Merge SoftDevice, Bootloader, and Application .hex files
mergehex -m softdevice.hex bootloader_setting.hex bootloader.hex -o merged_SD_BL_app.hex
mergehex -m merged_SD_BL_app.hex app.hex -o merged_SD_BL_app.hex
IF %ERRORLEVEL% NEQ 0 (
  GOTO FLASH_FAIL
)

:: Flash merged SoftDevice, Bootloader settings, Bootloader, and Application firmware
nrfjprog -f NRF52 --program merged_SD_BL_app.hex
IF %ERRORLEVEL% NEQ 0 (
  GOTO FLASH_FAIL
)

:: Cleanup generated files (merged hex, BL settings, etc.)
del "merged_SD_BL_app.hex"
del "bootloader_setting.hex"

:: Enable readback protection?
::nrfjprog -f NRF52 --rbp ALL

:: Pin-reset to disable the debug interface (saves ~1mA power)
::nrfjprog --pinreset
::nrfjprog -p
::IF %ERRORLEVEL% NEQ 0 (
::  GOTO FLASH_FAIL
::)

:: Display prompt in a message box
IF %ERRORLEVEL% NEQ 0 (
  GOTO FLASH_FAIL
)
:FLASH_SUCCESS
cscript MessageBox.vbs "EllieGrid firmware flashing SUCCEEDED!"
exit /b

:FLASH_FAIL
cscript MessageBox.vbs "EllieGrid firmware flashing FAILED! Please try again."
