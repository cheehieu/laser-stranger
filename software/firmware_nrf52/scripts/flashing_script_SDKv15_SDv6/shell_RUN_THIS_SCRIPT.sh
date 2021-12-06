#!/bin/sh

# Define return functions
print_error () {
    echo "\n\nEllieGrid firmware flashing FAILED! Please try again.\n\n"
}

print_success () {
    echo "\n\nEllieGrid firmware flashing SUCCEEDED!\n\n"
}

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
