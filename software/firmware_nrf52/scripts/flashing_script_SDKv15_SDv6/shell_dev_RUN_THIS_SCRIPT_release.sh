#!/bin/sh

# Define app configuration ("debug" or "release")
APP_CONFIG="release"
echo "\n------ APP_CONFIG = $APP_CONFIG ------"

# Define file paths
SOFTDEVICE="../../nRF5_SDK_15.0.0_a53641a/components/softdevice/s132/hex/s132_nrf52_6.1.1_softdevice.hex"
BL_RELEASE="../../nRF5_SDK_15.0.0_a53641a/examples/dfu/secure_bootloader/pca10040_ble/ses/Output/Release/Exe/secure_bootloader_ble_s132_pca10040.hex"
APP_RELEASE="../../nRF5_SDK_15.0.0_a53641a/examples/ble_peripheral/stranger-things-wall/pca10040/s132/ses/Output/Release/Exe/stranger-things-wall.hex"
APP_DEBUG="../../nRF5_SDK_15.0.0_a53641a/examples/ble_peripheral/stranger-things-wall/pca10040/s132/ses/Output/Debug/Exe/stranger-things-wall.hex"

# Define return functions
print_error () {
    echo "\n\nFirmware flashing FAILED! Please try again.\n\n"
}

print_success () {
    echo "\n\nFirmware flashing SUCCEEDED!\n\n"
}

# Copy latest build hex files to local directory
cp $SOFTDEVICE softdevice.hex
cp $BL_RELEASE bootloader.hex
if [ $APP_CONFIG == 'release' ]; then
    cp $APP_RELEASE app.hex
elif [ $APP_CONFIG == 'debug' ]; then
    cp $APP_DEBUG app.hex
else
    echo "ERROR: APP_CONFIG = \"$APP_CONFIG\" not supported"
    print_error
    exit 1
fi

# Recover BMD-300
nrfjprog -f NRF52 --recover
if [ $? -ne 0 ]; then
    print_error
    exit 1
fi

# Generate bootloader settings
nrfutil settings generate --family NRF52 --application app.hex --application-version 3 --bootloader-version 1 --bl-settings-version 1 bootloader_setting.hex
if [ $? -ne 0 ]; then
    print_error
    exit 1
fi

# Merge SoftDevice, Bootloader, and Application .hex files
mergehex -m softdevice.hex bootloader_setting.hex bootloader.hex -o merged_SD_BL_app.hex
mergehex -m merged_SD_BL_app.hex app.hex -o merged_SD_BL_app.hex
if [ $? -ne 0 ]; then
    print_error
    exit 1
fi

# Flash merged SoftDevice, Bootloader settings, Bootloader, and Application firmware
nrfjprog -f NRF52 --program merged_SD_BL_app.hex
if [ $? -ne 0 ]; then
    print_error
    exit 1
fi

# Cleanup generated files (merged hex, BL settings, etc.)
rm "merged_SD_BL_app.hex"
rm "bootloader_setting.hex"

# Enable readback protection?
# nrfjprog -f NRF52 --rbp ALL

# Pin-reset to disable the debug interface (saves ~1mA power)
# nrfjprog --pinreset
# nrfjprog -p
# if [ $? -ne 0 ]; then
#     print_error
#     exit 1
# fi

# Done!
print_success
