@echo off

:: Recover BMD-300
nrfjprog -f NRF52 --recover
IF %ERRORLEVEL% NEQ 0 (
  GOTO FLASH_FAIL
)

:: Generate bootloader settings
nrfutil settings generate --family NRF52 --application app_eg.hex --application-version 3 --bootloader-version 1 --bl-settings-version 1 bootloader_setting.hex
IF %ERRORLEVEL% NEQ 0 (
  GOTO FLASH_FAIL
)

:: Merge SoftDevice, Bootloader, and Application .hex files
mergehex -m softdevice.hex bootloader_setting.hex bootloader.hex -o merged_SD_BL_app.hex
mergehex -m merged_SD_BL_app.hex app_eg.hex -o merged_SD_BL_app.hex
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
