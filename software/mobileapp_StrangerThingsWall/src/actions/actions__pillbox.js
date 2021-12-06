import { Alert, Platform } from 'react-native';
import BleManager from 'react-native-ble-manager';
// import AsyncStorage from '@react-native-community/async-storage';
import store from '../store';
import * as bleConst from '../static/static__bluetooth';
// import { setAsyncStoragePersistent } from './actions__resolve-processes';
// import { sendPillboxCompliance } from './actions__compliance';
// import { getSliderIntFromVolume } from '../components/pillbox-volume-slider';
import moment from 'moment';

import {
    BEGIN_INIT_PILLBOX,
    DELETE_ALARM_DATA,
    INIT_PILLBOX_FAIL,
    INIT_PILLBOX_SUCCESS,
    SET_ALARM_DATA,
    SELECT_COMPARTMENT,
    SET_PILLBOX_LOADING,
    UPDATE_BATTERY_LEVEL,
    UPDATE_DEVICE_INFO,
    UPDATE_LED_STATE,
    UPDATE_PILLBOX_STATE,
    UPDATE_PILLBOX,
    UPDATE_VOLUME_LEVEL,
} from './types';
// import { PILLBOX_ACTION_UNKNOWN, PILLBOX_ACTION_TAKEN, PILLBOX_ACTION_SKIPPED, PILLBOX_ACTION_MISSED } from '../static/static__compliance';
// import { fetchAlarms } from './actions__fetched-data';
// import { changeDosesTimeZone } from './actions__alarms';
// import { Navigation } from 'react-native-navigation';

export const updatePillbox = ({ prop, value }) => {
    return {
        type: UPDATE_PILLBOX,
        payload: { prop, value }
    };
};

export const beginInitPillbox = () => {
    return {
        type: BEGIN_INIT_PILLBOX
    };
};

export const initializePillboxSuccess = (payload) => {
    return {
        type: INIT_PILLBOX_SUCCESS,
        payload
    };
};

export const initializePillboxFail = () => {
    return {
        type: INIT_PILLBOX_FAIL
    };
};

export const updateBatteryLevel = ({ batteryLevel, batteryIcon }) => {
    return {
        type: UPDATE_BATTERY_LEVEL,
        payload: { batteryLevel, batteryIcon }
    };
};

export const updatePillboxState = ({ chargingState, buttonState, lidState, batteryIcon }) => {
    return {
        type: UPDATE_PILLBOX_STATE,
        payload: { chargingState, buttonState, lidState, batteryIcon }
    };
};

export const updateDeviceInfo = ({ deviceName, manufName, serialNumber, hwRevision, fwRevision, swRevision }) => {
    return {
        type: UPDATE_DEVICE_INFO,
        payload: { deviceName, manufName, serialNumber, hwRevision, fwRevision, swRevision }
    };
};

export const setPillboxLoading = (payload) => {
    return {
        type: SET_PILLBOX_LOADING,
        payload
    };
};

export const selectCompartment = (payload) => {
    return {
        type: SELECT_COMPARTMENT,
        payload
    };
};

export const updateLedState = ({ ledState, selectedCompartment }) => {
    return {
        type: UPDATE_LED_STATE,
        payload: { ledState, selectedCompartment }
    };
};

export const updateVolumeLevel = (payload) => {
    return {
        type: UPDATE_VOLUME_LEVEL,
        payload
    };
};

export const actionSetAlarmData = (payload) => {
    return {
        type: SET_ALARM_DATA,
        payload
    };
};

export const actionDeleteAlarmData = (payload) => {
    return {
        type: DELETE_ALARM_DATA,
        payload
    };
};

// API FOR BLE CHARACTERISTICS
export const writeEllieChar = (rawCommand, peripheralId, serviceUUID, characteristicUUID) => {
    BleManager.write(
        peripheralId,
        serviceUUID,
        characteristicUUID,
        rawCommand,
        20
    )
        .then(() => {
            console.log('--- writeEllieChar rawCommand: ', rawCommand);
            store.dispatch(updatePillbox({ prop: 'loading', value: false }));
        })
        .catch(error => {
            console.log(error);
            store.dispatch(updatePillbox({ prop: 'loading', value: false }));
            throwUartNotificationError('There was a problem with sending data. Please try again.');
        });
};

export const getStringFromBleChar = (peripheralId, serviceUUID, characteristicUUID) => {
    return new Promise((resolve, reject) => {
        BleManager.read(peripheralId, serviceUUID, characteristicUUID)
            .then((result) => {
                let resultString = '';
                for (let ii = 0; ii < result.length; ii++) {
                    resultString += String.fromCharCode(result[ii]);
                }
                resolve(resultString);
            })
            .catch((error) => {
                console.log(error);
                reject(error);
            });
    });
};

/**
 * Initialize the pill box on every connection
 * @param {*} peripheralId
 */
export const initializePillbox = async (peripheralId, dispatch, getState) => {
    dispatch(beginInitPillbox());

    // Retrieve services and start notifications
    await BleManager.retrieveServices(peripheralId); // const peripheralDetails
    await BleManager.startNotification(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_TX_CHAR_UUID);
    await BleManager.startNotification(peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_STATUS_CHAR_UUID);
    await BleManager.startNotification(peripheralId, bleConst.BATTERY_SERVICE_UUID, bleConst.BATTERY_LEVEL_CHAR_UUID);

    // Get pillbox state, battery level
    await requestPillboxState(peripheralId);
    const { batteryLevel, batteryIcon } = await getBatteryLevel(peripheralId, dispatch, getState);
    return ({ batteryLevel, batteryIcon });
};

export const uninitializePillbox = async (peripheralId) => {
    await BleManager.stopNotification(peripheralId, bleConst.BATTERY_SERVICE_UUID, bleConst.BATTERY_LEVEL_CHAR_UUID);
    await BleManager.stopNotification(peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_STATUS_CHAR_UUID)
    await BleManager.stopNotification(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_TX_CHAR_UUID);
};

export const runPostConnection = async (peripheralId) => {

    // Get pillbox prefs
    await requestUserPref(peripheralId, bleConst.KEY_LED_B);
    await requestUserPref(peripheralId, bleConst.KEY_LED_R);
    await requestUserPref(peripheralId, bleConst.KEY_LED_G);
    await requestUserPref(peripheralId, bleConst.KEY_CHAR_DURATION_MS);
    await requestUserPref(peripheralId, bleConst.KEY_INTER_CHAR_DURATION_MS);
    await requestUserPref(peripheralId, bleConst.KEY_SPACE_DURATION_MS);

    console.log('--- READY!!!');
};

export const throwUartNotificationError = (errorMessage) => {
    Alert.alert(
        'Communication Error!',
        `${errorMessage}`,
        [{ text: 'OK' }],
        { cancelable: true }
    );
};

export const handleUartNotification = (data, dispatch, getState) => {
    const header = data.splice(0, 2);
    const [header0, header1] = header;

    if (header0 === bleConst.HEADER_CURRENT_TIME[0] && header1 === bleConst.HEADER_CURRENT_TIME[1]) {
        // Verify pill box time
        handleUartCurrentTimeNotification(data, dispatch, getState);
    } else if (header0 === bleConst.HEADER_USER_PREFS[0] && header1 === bleConst.HEADER_USER_PREFS[1]) {
        // Verify user prefs setting
        handleUartUserPrefsNotification(data, dispatch, getState);
    } else if (header0 === bleConst.HEADER_ACK[0] && header1 === bleConst.HEADER_ACK[1]) {
        // Verify alarm data set
        // handle HEADER_ALARM for failed alarm write?
        handleUartAlarmAckNotification(data, dispatch, getState);
    } else if (header0 === bleConst.HEADER_COMPL[0] && header1 === bleConst.HEADER_COMPL[1]) {
        // Receive current compliance data
        handleUartComplNotification(data, dispatch, getState);
    } else if (header0 === bleConst.HEADER_PAST_COMPL[0] && header1 === bleConst.HEADER_PAST_COMPL[1]) {
        // Receive past compliance data
        handleUartPastComplNotification(data, dispatch, getState);
    } else if (header0 === bleConst.HEADER_ALARM_REQUEST[0] && header1 === bleConst.HEADER_ALARM_REQUEST[1]) {
        // Requested alarm data
        handleUartAlarmRequestNotification(data, dispatch, getState);
    } else if (header0 === bleConst.HEADER_ALARM_CHECKSUM[0] && header1 === bleConst.HEADER_ALARM_CHECKSUM[1]) {
        // Requested alarm checksum
        handleUartAlarmChecksumNotification(data, dispatch, getState);
    } else if (header0 === bleConst.HEADER_DEBUG[0] && header1 === bleConst.HEADER_DEBUG[1]) {
        // Requested factory reset
        handleUartDebug(data, dispatch, getState);
    } else if (header0 === bleConst.HEADER_FACTORY_RESET[0] && header1 === bleConst.HEADER_FACTORY_RESET[1]) {
        // Requested factory reset
        handleUartFactoryResetNotification(data, dispatch, getState);
    } else {
        console.log('--- handleUartNotification: ', header, data);
    }
};

export const handleUartCurrentTimeNotification = (data, dispatch, getState) => {
    // Check if recovering from a reset
    if (data[0] == 0 && data[1] == 1 && data[2] == 2 && data[3] == 3) {
        console.log('Your EllieGrid pill box has successfully recovered from an error reset state.');
        // Alert.alert(
        //     'Success!',
        //     `Your EllieGrid pill box has successfully recovered from an error reset state.`,
        //     [{ text: 'OK' }],
        //     { cancelable: true }
        // );
    } else {
        // Convert data to concatenated hex string (with zero padding), then convert to epoch seconds
        const hexString = data.map(function(x) {
            x = x.toString(16);
            if (x.length < 2) x = '0' + x;
            return x;
        }).join('');
        const receivedSeconds = parseInt(hexString, 16);
        const receivedDate = new Date(receivedSeconds * 1000);
        // console.log('--- handleUartCurrentTimeNotification: PILL BOX TIME SET TO', receivedDate.toDateString(), receivedDate.toTimeString());

        // Check if pill box time is close to current time on phone (+/- 10 seconds), alert message if time sync fail
        // const timeWindowSeconds = 10;
        // const utcCurrentTime = new Date();
        // const year = utcCurrentTime.getFullYear();
        // const month = utcCurrentTime.getMonth();
        // const day = utcCurrentTime.getDate();
        // const hour = utcCurrentTime.getHours();
        // const minutes = utcCurrentTime.getMinutes();
        // const seconds = utcCurrentTime.getSeconds();
        // const timeUTC = new Date(
        //     Date.UTC(year, month, day, hour, minutes, seconds)
        // );
        // const utcSeconds = (timeUTC.getTime() / 1000);

        // if ((receivedSeconds < utcSeconds + timeWindowSeconds) && (receivedSeconds > utcSeconds - timeWindowSeconds)) {
        //     console.log('--- handleUartCurrentTimeNotification: PILL BOX TIME IS WITHIN VALID RANGE', receivedSeconds, utcSeconds);
        // } else {
        //     console.log('--- handleUartCurrentTimeNotification: INVALID PILL BOX TIME', receivedSeconds);
        //     throwUartNotificationError(`Error setting pill box time. App vs. Pill box: ${timeUTC.toTimeString()} vs. ${receivedDate.toTimeString()}`);
        // }
        const state = getState()
        const timeConnected = state.pillbox.timeConnected;
        const dfuState = state.fw_update.dfuState;
        if (timeConnected !== null && dfuState  !== 'DFU_COMPLETED') {
            const isoStr = receivedDate.toISOString();
            const timeComponent = moment(isoStr).utc().format('HH:mm:ss A');
            Alert.alert(
                'Pill Box Time',
                `Your pill box's current time is: ${timeComponent}\n\n` +
                `EPOCH: ${receivedSeconds}\n` +
                `ISO: ${isoStr}\n` +
                `LONG: ${receivedDate.toString()}\n`,
                [{ text: 'OK' }],
                { cancelable: true }
            );
        }
    }
};

export const handleUartUserPrefsNotification = (data, dispatch, getState) => {
    const state = getState()
    // const UID = state.persistent.UID;
    const CONNECTED_PERIPHERAL = state.persistent.CONNECTED_PERIPHERAL;

    switch(data[0]) {
        case bleConst.KEY_LED_R:
            if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                console.log('--- Request LED_R returned:', data[2]);
                dispatch(updatePillbox({ prop: 'ledR', value: data[2].toString() }));
            } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                if (data[2] === state.pillbox.userPrefsValue) {
                    console.log('--- User Preference Setting SUCCESS', data[2]);
                    dispatch(updatePillbox({ prop: 'ledR', value: data[2].toString() }));
                } else {
                    console.log('--- User Preference Setting FAIL', data[2]);
                    // throwUartNotificationError(`Error setting LED_R. Read back: ${data[2]}`);
                }
            }
            break;

        case bleConst.KEY_LED_G:
            if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                console.log('--- Request LED_G returned:', data[2]);
                dispatch(updatePillbox({ prop: 'ledG', value: data[2].toString() }));
            } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                if (data[2] === state.pillbox.userPrefsValue) {
                    console.log('--- User Preference Setting SUCCESS', data[2]);
                    dispatch(updatePillbox({ prop: 'ledG', value: data[2].toString() }));
                } else {
                    console.log('--- User Preference Setting FAIL', data[2]);
                    // throwUartNotificationError(`Error setting LED_G. Read back: ${data[2]}`);
                }
            }
            break;

        case bleConst.KEY_LED_B:
            if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                console.log('--- Request LED_B returned:', data[2]);
                dispatch(updatePillbox({ prop: 'ledB', value: data[2].toString() }));
            } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                if (data[2] === state.pillbox.userPrefsValue) {
                    console.log('--- User Preference Setting SUCCESS', data[2]);
                    dispatch(updatePillbox({ prop: 'ledB', value: data[2].toString() }));
                } else {
                    console.log('--- User Preference Setting FAIL', data[2]);
                    // throwUartNotificationError(`Error setting LED_B. Read back: ${data[2]}`);
                }
            }
            break;

        case bleConst.KEY_VOLUME:
            if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                console.log('--- Request Volume Level returned:', data[2]);

                // Save volume in pillbox and persistent
                dispatch(updatePillbox({ prop: 'volumeLevel', value: data[2] }));
                // const volumeSliderInt = getSliderIntFromVolume(data[2]);
                // setAsyncStoragePersistent('VOLUME', volumeSliderInt, true, dispatch, getState);

            } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                if (data[2] === state.pillbox.userPrefsValue) {
                    const volumeLevel = data[2];
                    console.log('--- Volume User Preference Setting SUCCESS', volumeLevel);

                    // Play sample alarm at selected volume
                    if (volumeLevel > bleConst.VOLUME_LEVEL_OFF && volumeLevel <= 100) {
                        playAudioSound(CONNECTED_PERIPHERAL.id, state.pillbox.alarmPingTone);
                    }
                    
                    // Save volume in pillbox and persistent
                    dispatch(updatePillbox({ prop: 'volumeLevel', value: volumeLevel }));
                    // const volumeSliderInt = getSliderIntFromVolume(volumeLevel);
                    // setAsyncStoragePersistent('VOLUME', volumeSliderInt, true, dispatch, getState);
                } else {
                    console.log('--- User Preference Setting FAIL', data[2]);
                    // throwUartNotificationError(`Error setting pill box volume. Read back: ${data[2]}`);
                }
            }
            break;

            case bleConst.KEY_CHAR_DURATION_MS:
                if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                    console.log('--- Request charDuration returned:', data[2]);
                    dispatch(updatePillbox({ prop: 'charDuration', value: ((data[2]<<8) | data[3]).toString() }));
                } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                    if ((data[2]<<8) | data[3] === state.pillbox.userPrefsValue) {
                        console.log('--- User Preference Setting SUCCESS', (data[2]<<8) | data[3]);
                        dispatch(updatePillbox({ prop: 'charDuration', value: ((data[2]<<8) | data[3]).toString() }));
                    } else {
                        console.log('--- User Preference Setting FAIL', (data[2]<<8) | data[3]);
                        // throwUartNotificationError(`Error setting charDuration. Read back: ${(data[2]<<8) | data[3]}`);
                    }
                }
                break;

            case bleConst.KEY_INTER_CHAR_DURATION_MS:
                if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                    console.log('--- Request interCharDuration returned:', data[2]);
                    dispatch(updatePillbox({ prop: 'interCharDuration', value: ((data[2]<<8) | data[3]).toString() }));
                } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                    if ((data[2]<<8) | data[3] === state.pillbox.userPrefsValue) {
                        console.log('--- User Preference Setting SUCCESS', (data[2]<<8) | data[3]);
                        dispatch(updatePillbox({ prop: 'interCharDuration', value: ((data[2]<<8) | data[3]).toString() }));
                    } else {
                        console.log('--- User Preference Setting FAIL', (data[2]<<8) | data[3]);
                        // throwUartNotificationError(`Error setting interCharDuration. Read back: ${(data[2]<<8) | data[3]}`);
                    }
                }
                break;

            case bleConst.KEY_SPACE_DURATION_MS:
                if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                    console.log('--- Request spaceDuration returned:', data[2]);
                    dispatch(updatePillbox({ prop: 'spaceDuration', value: ((data[2]<<8) | data[3]).toString() }));
                } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                    if ((data[2]<<8) | data[3] === state.pillbox.userPrefsValue) {
                        console.log('--- User Preference Setting SUCCESS', (data[2]<<8) | data[3]);
                        dispatch(updatePillbox({ prop: 'spaceDuration', value: ((data[2]<<8) | data[3]).toString() }));
                    } else {
                        console.log('--- User Preference Setting FAIL', (data[2]<<8) | data[3]);
                        // throwUartNotificationError(`Error setting spaceDuration. Read back: ${(data[2]<<8) | data[3]}`);
                    }
                }
                break;

        case bleConst.KEY_PING_TONE_SEL:
            if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                console.log('--- Request KEY_PING_TONE_SEL returned:', data[2]);
                dispatch(updatePillbox({ prop: 'alarmPingTone', value: data[2] }));
            } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                if (data[2] === state.pillbox.userPrefsValue) {
                    console.log('--- User Preference Setting SUCCESS', data[2]);
                    dispatch(updatePillbox({ prop: 'alarmPingTone', value: data[2] }));
                } else {
                    console.log('--- User Preference Setting FAIL', data[2]);
                    // throwUartNotificationError(`Error setting KEY_PING_TONE_SEL. Read back: ${data[2]}`);
                }
            }
            break;

        case bleConst.KEY_ALARM_BETWEEN_HB_DURATION:
            if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                console.log('--- Request KEY_ALARM_BETWEEN_HB_DURATION returned:', data[2]);
                dispatch(updatePillbox({ prop: 'alarmBetweenHbDur', value: data[2] }));
            } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                if (data[2] === state.pillbox.userPrefsValue) {  // may fail when calling subsequent setBetweenPingsDuration since userPrefsValue will change in redux
                    console.log('--- User Preference Setting SUCCESS', data[2]);
                    dispatch(updatePillbox({ prop: 'alarmBetweenHbDur', value: data[2] }));
                } else {
                    console.log('--- User Preference Setting FAIL', data[2]);
                    // throwUartNotificationError(`Error setting KEY_ALARM_BETWEEN_HB_DURATION. Read back: ${data[2]}`);
                }
            }
            break;

        case bleConst.KEY_ALARM_BETWEEN_PINGS_DURATION:
            if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                console.log('--- Request KEY_ALARM_BETWEEN_PINGS_DURATION returned:', data[2]);
                dispatch(updatePillbox({ prop: 'alarmBetweenPingDur', value: data[2] }));
            } else if (data[1] === bleConst.KEY_USER_PREFS_SET) {
                if (data[2] ===state.pillbox.userPrefsValue) {
                    console.log('--- User Preference Setting SUCCESS', data[2]);
                    dispatch(updatePillbox({ prop: 'alarmBetweenPingDur', value: data[2] }));
                } else {
                    console.log('--- User Preference Setting FAIL', data[2]);
                    // throwUartNotificationError(`Error setting KEY_ALARM_BETWEEN_PINGS_DURATION. Read back: ${data[2]}`);
                }
            }
            break;

        case bleConst.KEY_NUS_MAX_DATA_LEN:
            if (data[1] === bleConst.KEY_USER_PREFS_GET) {
                let nusMaxDataLen = data.slice(2).map(function(x) {
                    x = x.toString(16);
                    if (x.length < 2) x = '0' + x;
                    return x;
                }).join('');
                nusMaxDataLen = parseInt(nusMaxDataLen, 16);
                console.log('--- Request NUS_MAX_DATA_LEN returned:', nusMaxDataLen);
                dispatch(updatePillbox({ prop: 'nusMaxDataLen', value: nusMaxDataLen }));
            }
            break;

        // TODO: add other user prefs

        default:
            console.log('--- User Preference KEY not supported.', data[0]);
    }
};

export const handleUartAlarmAckNotification = (data) => {
    console.log('--- Alarm Data Response Received: ', data);
    // const compartment = data[0];
    // const numPills = data[1];
    // let nextAlarmEpoch = data.slice(2).map(function(x) {
    //     x = x.toString(16);
    //     if (x.length < 2) x = '0' + x;
    //     return x;
    // }).join('');
    // nextAlarmEpoch = parseInt(nextAlarmEpoch, 16);

    // const adjNextAlarmEpoch_ms = (nextAlarmEpoch + (6 * 60 * 60)) * 1000;
    // const nextAlarmEpochLocaleString = (new Date(adjNextAlarmEpoch_ms)).toLocaleString();
    // const deletedFlag = (numPills == 0 && nextAlarmEpoch == 0) ? true : false;

    // Add support for alarmEXT (only last ACK message is valid for regimin)
    // Get alarm data instead and verify flash contents

    // if (numPills === 0) {
    //     Alert.alert(
    //         'Success!',
    //         `Deleted alarms from compartment ${compartment} [${nextAlarmEpoch}]`,
    //         [{ text: 'OK' }],
    //         { cancelable: true }
    //     );
    // }

    // } else {
    //     Alert.alert(
    //         'Success!',
    //         `Set alarms for compartment ${compartment}. The next upcoming alarm for this compartment is ${numPills} pills on ${nextAlarmEpochLocaleString}`,
    //         [{ text: 'OK' }],
    //         { cancelable: true }
    //     );

    //     // TODO: handle reconcileProcessResults() here instead of in actions__medication.js
    //     // reconcileProcessResults(dispatch, ({
    //     //     alert: {
    //     //         key: 'SUCCESS__MED_EDITED',
    //     //         var: name
    //     //     },
    //     //     route: 'drawer',
    //     //     clearForm: true
    //     // }));
    // }
};

export const handleUartComplNotification = (data, dispatch, getState) => {
    console.log('--- Compliance Data Received: ', data);
    const complNotifyState = data[0];
    const complLastRsoc = data[1];
    const compartments = data[2];
    const alarmStatus = data[3];

    if (alarmStatus === bleConst.ALARM_DATA_INVALID) {
        Alert.alert(
            'Compliance Check',
            'The previous alarm\'s compliance data is not available. Please try again after an alarm event has occurred.',
            [{ text: 'OK' }],
            { cancelable: true }
        );
    } else {
        // Parse individual compartments from one-hot byte representation
        let compNumArray = [];
        for (let ii = 0; ii < 8; ii++) {
            if ((compartments >> ii) & 0x01) {
                compNumArray.push((ii+1));
            }
        }
        // const compNumStr = compNumArray.join(', ');
        // console.log('compNumStr = ', compNumStr, 'size =', compNumArray.length);

        // Format timestamps from hex string value to epoch seconds
        let alarmTimestamp = data.slice(4, 8).map(function(x) {
            x = x.toString(16);
            if (x.length < 2) x = '0' + x;
            return x;
        }).join('');
        alarmTimestamp = parseInt(alarmTimestamp, 16);

        let eventTimestamp = data.slice(8).map(function(x) {
            x = x.toString(16);
            if (x.length < 2) x = '0' + x;
            return x;
        }).join('');
        eventTimestamp = parseInt(eventTimestamp, 16);

        // Translate alarm status to human readable string
        // let alarmStatusString = '';
        // let alarmAction = PILLBOX_ACTION_UNKNOWN;
        // switch(alarmStatus) {
        //     case bleConst.ALARM_DATA_TAKEN:
        //         // alarmStatusString = 'TAKEN';
        //         alarmAction = PILLBOX_ACTION_TAKEN;
        //         break;
        //     case bleConst.ALARM_DATA_SKIPPED:
        //         // alarmStatusString = 'SKIPPED';
        //         alarmAction = PILLBOX_ACTION_SKIPPED;
        //         break;
        //     case bleConst.ALARM_DATA_MISSED:
        //         // alarmStatusString = 'MISSED';
        //         alarmAction = PILLBOX_ACTION_MISSED;
        //         break;
        //     default:
        //         // alarmStatusString = 'INVALID';
        //         alarmAction = PILLBOX_ACTION_UNKNOWN;
        // }

        // Compute local timezone offset
        // const adjAlarmTimestamp_ms = (alarmTimestamp + (new Date().getTimezoneOffset() * 60)) * 1000;
        // const adjEventTimestamp_ms = (eventTimestamp + (new Date().getTimezoneOffset() * 60)) * 1000;
        // const alarmLocaleString = (new Date(adjAlarmTimestamp_ms)).toLocaleString();
        // const complianceLocaleString = (new Date(adjEventTimestamp_ms)).toLocaleString();
        // Alert.alert(
        //     'Compliance Check',
        //     `The alarm for compartments ${compNumStr} on ${alarmLocaleString} was logged as ${alarmStatusString} at ${complianceLocaleString}`,
        //     [{ text: 'OK' }],
        //     { cancelable: true }
        // );
        // Alert.alert(
        //     'Compliance Check',
        //     `The alarm for compartments ${compNumStr} was logged as ${alarmStatusString}`,
        //     [{ text: 'OK' }],
        //     { cancelable: true }
        // );

        // If valid, send compliance data to Firebase
        // if (compartments > 0) {
        //     sendPillboxCompliance(
        //         (alarmTimestamp + (new Date().getTimezoneOffset() * 60)) * 1000,
        //         compNumArray,
        //         alarmAction,
        //         (eventTimestamp + (new Date().getTimezoneOffset() * 60)) * 1000,
        //         complNotifyState,
        //         complLastRsoc
        //     );
        // }
    }
};

export const handleUartPastComplNotification = (data, dispatch, getState) => {
    console.log('--- Past Compliance Data Received: ', data);
    let message = '';
    let msg = '';

    // Handler for v2.x.x firmware
    const compl_code = data[0];
    let compl_count = 0;
    if (compl_code == bleConst.REQUEST_COMPL_DATA) {
        // const complIdx = (data[1] << 0x8) | data[2];
        const complNotifyState = data[3];
        const complLastRsoc = data[4];
        const compartments = data[5];
        const alarmStatus = data[6];

        // Parse individual compartments from one-hot byte representation
        let compNumArray = [];
        for (let ii = 0; ii < 8; ii++) {
            if ((compartments >> ii) & 0x01) {
                compNumArray.push((ii+1));
            }
        }
        // const compNumStr = compNumArray.join(', ');

        // Format timestamps from hex string value to epoch seconds
        let alarmTimestamp = data.slice(7, 11).map(function(x) {
            x = x.toString(16);
            if (x.length < 2) x = '0' + x;
            return x;
        }).join('');
        alarmTimestamp = parseInt(alarmTimestamp, 16);

        let eventTimestamp = data.slice(11).map(function(x) {
            x = x.toString(16);
            if (x.length < 2) x = '0' + x;
            return x;
        }).join('');
        eventTimestamp = parseInt(eventTimestamp, 16);

        // Translate alarm status to human readable string
        // let alarmStatusString = '';
        // let alarmAction = PILLBOX_ACTION_UNKNOWN;
        // switch(alarmStatus) {
        //     case bleConst.ALARM_DATA_TAKEN:
        //         // alarmStatusString = 'TAKEN';
        //         alarmAction = PILLBOX_ACTION_TAKEN;
        //         break;
        //     case bleConst.ALARM_DATA_SKIPPED:
        //         // alarmStatusString = 'SKIPPED';
        //         alarmAction = PILLBOX_ACTION_SKIPPED;
        //         break;
        //     case bleConst.ALARM_DATA_MISSED:
        //         // alarmStatusString = 'MISSED';
        //         alarmAction = PILLBOX_ACTION_MISSED;
        //         break;
        //     default:
        //         // alarmStatusString = 'INVALID';
        // }

        // Compute local timezone offset
        // const adjAlarmTimestamp_ms = (alarmTimestamp + (new Date().getTimezoneOffset() * 60)) * 1000;
        // const adjEventTimestamp_ms = (eventTimestamp + (new Date().getTimezoneOffset() * 60)) * 1000;
        // const alarmLocaleString = (new Date(adjAlarmTimestamp_ms)).toLocaleString();
        // const complianceLocaleString = (new Date(adjEventTimestamp_ms)).toLocaleString();
        // msg =   `index: ${complIdx}\n` +
        //         `complNotifyState: ${complNotifyState}\n` +
        //         `complLastRsoc:  ${complLastRsoc}\n` +
        //         `compartments:  ${compNumStr}\n` +
        //         `alarm_status:  ${alarmStatusString}\n` +
        //         `alarm_ts:  ${alarmLocaleString}\n` +
        //         `alarm_event_ts:  ${complianceLocaleString}`;
        // console.log('--- fetchComplianceData:', msg);

        // sendPillboxCompliance(
        //     (alarmTimestamp + (new Date().getTimezoneOffset() * 60)) * 1000,
        //     compNumArray,
        //     alarmAction,
        //     (eventTimestamp + (new Date().getTimezoneOffset() * 60)) * 1000,
        //     complNotifyState,
        //     complLastRsoc
        // );
        const state = getState()
        const complianceCount = state.pillbox.complianceCount;
        const CONNECTED_PERIPHERAL = state.persistent.CONNECTED_PERIPHERAL;
        deletePastComplianceData(CONNECTED_PERIPHERAL.id, complianceCount);

    } else if (compl_code == bleConst.REQUEST_COMPL_COUNT) {
        compl_count = (data[1] << 0x8) | data[2];
        dispatch(updatePillbox({ prop: 'complianceCount', value: compl_count }));
        message = '';   //`There are ${compl_count} compl_data available.`;

        // Prompt user to fetch compliance data
        if (compl_count > 0) {
            // Alert.alert(
            //     'Past Compliance Data',
            //     `Your pill box contains past compliance data that can be fetched. Would you like to synchronize?`,
            //     [
            //         { text: 'Cancel', style: 'cancel' },
            //         { text: 'OK', onPress: () => fetchComplianceData() }
            //     ],
            //     { cancelable: true }
            // );
            fetchComplianceData(dispatch,getState);
        }

    } else if (compl_code == bleConst.CLEAR_COMPL_DATA) {
        compl_count = (data[1] << 0x8) | data[2];
        message = '';   //`Cleared compliance data. Count = ${compl_count}`;

    } else if (compl_code == bleConst.DELETE_PAST_COMPL_DATA) {
        const complIdx = (data[1] << 0x8) | data[2];
        compl_count = (data[3] << 0x8) | data[4];
        msg = `Deleted compliance data at index ${complIdx}. Count = ${compl_count}`;
        console.log('--- fetchComplianceData:', msg);
        dispatch(updatePillbox({ prop: 'complianceCount', value: compl_count }));
        if (compl_count > 0) {
            fetchComplianceData(dispatch, getState);
        }

    } else if (compl_code == bleConst.COMPL_DATA_INVALID) {
        compl_count = (data[1] << 0x8) | data[2];
        message = `Trying to access compliance data out of bounds... Count = ${compl_count}`;
    } else {
        message = `ERROR: compl_code is invalid`;
    }

    if (message !== '') {
        Alert.alert(
            'Past Compliance Data',
            `${message}`,
            [{ text: 'OK' }],
            { cancelable: true }
        );
    }
};

export const handleUartAlarmRequestNotification = (data, dispatch, getState) => {
    console.log('--- Alarm Data Received: ', data);
    const { compartment, startYear, startMonth, startDay, interval, numAlarms, doseTimes } = parseAlarmData(data);
    console.log('parseAlarmData() returns:', compartment, startYear, startMonth, startDay, interval, numAlarms, doseTimes);

    const state = getState()
    // Create new compartment alarm data string
    let alarmsDetailStr =state.pillbox.alarmsDetailStr;
    const line1 = `--- Compartment #${compartment} ---\n`;
    if (numAlarms !== 0) {
        const startDate = new Date(startYear+1900, startMonth, startDay);
        const line2 = `StartDate: ${startDate.toDateString()}\n`;
        const line3 = `Interval: Every ${interval} days\n`;
        const line4 = `DoseTimes (${numAlarms}):\n`;
        var ii;
        let line5 = '';
        for (ii = 0; ii < numAlarms; ii++) {
            const dosage = doseTimes[ii].dosage;
            const dosageStr = (dosage === 1) ? `${dosage} pill` : `${dosage} pills`;    // grammar
            const minute = doseTimes[ii].minute;
            const minuteStr = (minute < 10) ? `0${minute}` : `${minute}`;   // prepend 0 for human-readable minute
            line5 = line5.concat(`    ${dosageStr} at ${doseTimes[ii].hour}:${minuteStr}\n`);
        }
        
        alarmsDetailStr = alarmsDetailStr.concat(line1, line2, line3, line4, line5, '\n');

    } else {
        const line2 = `No alarms scheduled.\n\n`;
        alarmsDetailStr = alarmsDetailStr.concat(line1, line2);
    }

    // Append compartment alarm data to alarmsDetailStr
    dispatch(updatePillbox({ prop: 'alarmsDetailStr', value: alarmsDetailStr }));

    // Request data from next compartment
    if (compartment < 7) {
        const nextComp = compartment + 1;
        const peripheralId = state.persistent.CONNECTED_PERIPHERAL.id;
        requestAlarmData(peripheralId, nextComp);
    } else {
        // Display data in alert, clear alarmsDetailStr after viewing
        dispatch(updatePillbox({ prop: 'pendingTask', value: false }));
        Alert.alert(
            'Detailed Alarm Data',
            alarmsDetailStr,
            [{ text: 'OK', onPress: () => {
                dispatch(updatePillbox({ prop: 'alarmsDetailStr', value: '' }))
            }}],
            { cancelable: true }
        );
    }
};

export const handleUartAlarmChecksumNotification = (data) => {
    console.log('--- Alarm Checksum Received: ', data);
    const checksum = data.join('');
    const validCompartments = data[0];  // compartments that have alarms set
    const numAlarms1 = data[1];
    const numAlarms2 = data[2];
    const numAlarms3 = data[3];
    const numAlarms4 = data[4];
    const numAlarms5 = data[5];
    const numAlarms6 = data[6];
    const numAlarms7 = data[7];

    // Check if valid compartments are correct in app (and firebase) 0b0-c7-c6-c5-c4-c3-c2-c1
    const valid1 = (validCompartments & bleConst.COMP1_MASK) ? '(valid)' : '(empty)';
    const valid2 = (validCompartments & bleConst.COMP2_MASK) ? '(valid)' : '(empty)';
    const valid3 = (validCompartments & bleConst.COMP3_MASK) ? '(valid)' : '(empty)';
    const valid4 = (validCompartments & bleConst.COMP4_MASK) ? '(valid)' : '(empty)';
    const valid5 = (validCompartments & bleConst.COMP5_MASK) ? '(valid)' : '(empty)';
    const valid6 = (validCompartments & bleConst.COMP6_MASK) ? '(valid)' : '(empty)';
    const valid7 = (validCompartments & bleConst.COMP7_MASK) ? '(valid)' : '(empty)';

    // Check if number of alarms are correct (e.g. matches with compartment.doseTimes.length)
    Alert.alert(
        'Alarm Quickview',
        `The following info is an quickview of the alarms programmed on your pill box.\n\n` +
        `CHECKSUM: ${checksum}\n` +
        `Comp #1 has ${numAlarms1} alarms ${valid1}\n` +
        `Comp #2 has ${numAlarms2} alarms ${valid2}\n` +
        `Comp #3 has ${numAlarms3} alarms ${valid3}\n` +
        `Comp #4 has ${numAlarms4} alarms ${valid4}\n` +
        `Comp #5 has ${numAlarms5} alarms ${valid5}\n` +
        `Comp #6 has ${numAlarms6} alarms ${valid6}\n` +
        `Comp #7 has ${numAlarms7} alarms ${valid7}`,
        [{ text: 'OK' }],
        { cancelable: true }
    );
};


export const handleUartDebug = (data) => {
    console.log('--- handleUartDebug(): ', data);
};

export const handleUartFactoryResetNotification = (data, dispatch, getState) => {
    console.log('--- Factory Reset Received: ', data);
    Alert.alert(
        'EllieGrid Reset',
        `Your pill box has been reset to factory settings. To complete the system reset, press the pinhole reset button on the side of the lid, then connect to 'Ellie-XXXX'`,
        [{ text: 'OK' }],
        { cancelable: true }
    );

    // Disconnect from BLE client on reset (deleteAccount signout)
    const CONNECTED_PERIPHERAL = getState().persistent.CONNECTED_PERIPHERAL;
    clientDisconnect(CONNECTED_PERIPHERAL.id);
};

// export const handleUartNotification = (data) => {
// };

export const handleBatteryNotification = (data, dispatch, getState) => {
    const batteryLevel = data[0];
    const state = getState();
    const chargingState = state.pillbox.chargingState;
    const batteryIcon = getBatteryIcon(batteryLevel, chargingState);
    dispatch(updateBatteryLevel({ batteryLevel: batteryLevel, batteryIcon: batteryIcon }))

    // Alert user if low battery < 20%
    if ((batteryLevel < bleConst.ALERT_LOW_BATTERY_LEVEL) && (chargingState === false)) {
        // Prevent alert during report notification scene
        if (state.screen.screenName !== 'Notification') {
            Alert.alert(
                'Low Battery!',
                `Your pill box's battery is running low... be sure to charge your EllieGrid soon.`,
                [{ text: 'OK' }],
                { cancelable: true }
            );
        }
    }
};

export const handleStatusNotification = (data, dispatch, getState) => {
    const buttonPressed = (data & bleConst.STATUS_BUTTON_PRESSED_MASK) ? true : false;
    const lidOpen = (data & bleConst.STATUS_LID_OPEN_MASK) ? true : false;
    const charging = (data & bleConst.STATUS_CHARGING_MASK) ? true : false;
    const state = getState()
    const batteryIcon = getBatteryIcon(state.pillbox.batteryLevel, charging);
    dispatch(updatePillboxState({ chargingState: charging, buttonState: buttonPressed, lidState: lidOpen, batteryIcon: batteryIcon }));
    // console.log(data, ' --> ', 'button: ', buttonPressed, 'lid: ', lidOpen, 'charge: ', charging, 'batteryIcon: ', batteryIcon);
};

export const getDeviceName = (peripheral) => {
    if (peripheral == null) {
        return new Promise((reject) => {
            reject('null');
        });
    }

    // iOS doesn't allow access to the GAP service; access name from peripheral.name instead
    if (Platform.OS === 'android') {
        return getStringFromBleChar(peripheral.id, bleConst.GAP_SERVICE_UUID, bleConst.DEVICE_NAME_CHAR);
    } else {
        return new Promise((resolve) => {
            resolve(peripheral.name);
        });
    }
};

export const setDeviceName = (peripheralId, newDeviceName) => {
    return (dispatch) => {
        // Show spinner
        dispatch(updatePillbox({ prop: 'pendingTask', value: true }));

        // Prepend with HEADER 'dn' and (0x20+length)
        let data = [bleConst.HEADER_DEVICE_NAME[0], bleConst.HEADER_DEVICE_NAME[1], 0x20 + newDeviceName.length];
        const map = Array.prototype.map;
        const hexName = map.call(newDeviceName, (x) => {
            return x.charCodeAt(0);
        });
        data = data.concat(hexName);

        BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
            .then(() => {
                console.log('--- setDeviceName(): ', newDeviceName);
                // dispatch(updatePillbox({ prop: 'deviceName', value: newDeviceName }));
                // dispatch(updateDfu({ prop: 'dfuDeviceName', value: newDeviceName }));

                // Reset pillbox for changes to take effect
                softReset(peripheralId);

                dispatch(updatePillbox({ prop: 'pendingTask', value: false }));
            })
            .catch((error) => {
                console.log(error);
                dispatch(updatePillbox({ prop: 'pendingTask', value: false }));
                throwUartNotificationError('There was a problem with setting the device name. Please try again.');
            });
    };
};

export const getManufName = (peripheralId) => {
    return getStringFromBleChar(peripheralId, bleConst.DEVICE_INFO_SERVICE_UUID, bleConst.MANUF_NAME_CHAR);
};

export const getModelNumber = (peripheralId) => {
    return getStringFromBleChar(peripheralId, bleConst.DEVICE_INFO_SERVICE_UUID, bleConst.MODEL_NUM_CHAR);
};

export const getSerialNumber =(peripheralId) => {
    return getStringFromBleChar(peripheralId, bleConst.DEVICE_INFO_SERVICE_UUID, bleConst.SERIAL_NUM_CHAR);
};

export const setSerialNumber = (peripheralId, newSerialNumber) => {
    return (dispatch) => {
        // Show spinner
        dispatch(updatePillbox({ prop: 'loading', value: true }));

        // Prepend with HEADER 'sn'
        let data = bleConst.HEADER_SERIAL_NUM;
        const map = Array.prototype.map;
        const hexSerial = map.call(newSerialNumber, (x) => {
            return x.charCodeAt(0);
        });
        data = data.concat(hexSerial);

        BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
            .then(() => {
                console.log('--- setSerialNumber(): ', newSerialNumber);
                dispatch(updatePillbox({ prop: 'serialNumber', value: newSerialNumber }));
                dispatch(updatePillbox({ prop: 'loading', value: false }));
            })
            .catch((error) => {
                console.log(error);
                dispatch(updatePillbox({ prop: 'loading', value: false }));
                throwUartNotificationError('There was a problem with setting the serial number. Please try again.');
            });
    }
};

export const getHwRevision = (peripheralId) => {
    return getStringFromBleChar(peripheralId, bleConst.DEVICE_INFO_SERVICE_UUID, bleConst.HW_REV_CHAR);
};

export const getFwRevision = (peripheralId) => {
    return getStringFromBleChar(peripheralId, bleConst.DEVICE_INFO_SERVICE_UUID, bleConst.FW_REV_CHAR);
};

export const getSwRevision = (peripheralId) => {
    return getStringFromBleChar(peripheralId, bleConst.DEVICE_INFO_SERVICE_UUID, bleConst.SW_REV_CHAR);
};

export const getDeviceInfo = (peripheral) => {
    return (dispatch) => {
        // Show spinner
        dispatch(updatePillbox({ prop: 'loading', value: true }));

        const deviceInfo = { deviceName: '', manufName: '', serialNumber: '', hwRevision: '', fwRevision: '', swRevision: '' }
        getManufName(peripheral.id)
            .then((result) => deviceInfo.manufName = result)
            .then(() => getSerialNumber(peripheral.id))
            .then((result) => deviceInfo.serialNumber = result)
            .then(() => getHwRevision(peripheral.id))
            .then((result) => deviceInfo.hwRevision = result)
            .then(() => getFwRevision(peripheral.id))
            .then((result) => deviceInfo.fwRevision = result)
            .then(() => getSwRevision(peripheral.id))
            .then((result) => deviceInfo.swRevision = result)
            .then(() => getDeviceName(peripheral))
            .then((result) => deviceInfo.deviceName = result)
            .then(() => {
                console.log('--- getDeviceInfo(): ', deviceInfo);
                dispatch(updateDeviceInfo(deviceInfo));
                dispatch(updatePillbox({ prop: 'loading', value: false }));

                Alert.alert(
                    'Device Information',
                    `Device Name:  ${deviceInfo.deviceName}\n` +
                    `Manufacturer:  ${deviceInfo.manufName}\n` +
                    `Serial Number:  ${deviceInfo.serialNumber}\n` +
                    `Hardware Rev.:  ${deviceInfo.hwRevision}\n` +
                    `Firmware Rev.:  ${deviceInfo.fwRevision}\n` +
                    `Software Rev.:  ${deviceInfo.swRevision}\n`,
                    [{ text: 'OK' }],
                    { cancelable: true }
                );
            })
            .catch((error) => {
                console.log(error);
                dispatch(updatePillbox({ prop: 'loading', value: false }));
                throwUartNotificationError('There was a problem with retrieving device information. Please try again.');
            });
    };
};

export const requestPillboxState = (peripheralId) => {
    return new Promise((resolve, reject) => {
        const data = [(bleConst.COMMAND_PILLBOX_STATE << 4), 0, 0, 0].reverse();
        BleManager.write(peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID, data, 20)
            .then(() => {
                // console.log('--- requestPillboxState()');
                store.dispatch(updatePillbox({ prop: 'loading', value: false }));
                resolve(true);
            })
            .catch((error) => {
                console.log(error);
                store.dispatch(updatePillbox({ prop: 'loading', value: false }));
                throwUartNotificationError('There was a problem with requesting the pill box state. Please try again.');
                reject(error);
            });
    });
};

export const getBatteryLevel = (peripheralId, dispatch, getState) => {
    return new Promise((resolve, reject) => {
        BleManager.read(peripheralId, bleConst.BATTERY_SERVICE_UUID, bleConst.BATTERY_LEVEL_CHAR_UUID)
            .then((readData) => {
                const batteryIcon = getBatteryIcon(readData[0], getState().pillbox.chargingState);
                dispatch(updateBatteryLevel({ batteryLevel: readData[0], batteryIcon: batteryIcon }));
                resolve({ batteryLevel: readData[0], batteryIcon });
            })
            .catch((error) => {
                console.log(error);
                reject(error);
            });
    });
};

export const getBatteryIcon = (batteryLevel, chargingState) => {
    let batteryIcon = '';
    if (chargingState === true) {
        if (batteryLevel <= 7) {
            batteryIcon = 'battery-charging-outline';
        } else if (batteryLevel <= 15) {
            batteryIcon = 'battery-charging-10';
        } else if (batteryLevel <= 25) {
            batteryIcon = 'battery-charging-20';
        } else if (batteryLevel <= 35) {
            batteryIcon = 'battery-charging-30';
        } else if (batteryLevel <= 45) {
            batteryIcon = 'battery-charging-40';
        } else if (batteryLevel <= 55) {
            batteryIcon = 'battery-charging-50';
        } else if (batteryLevel <= 65) {
            batteryIcon = 'battery-charging-60';
        } else if (batteryLevel <= 75) {
            batteryIcon = 'battery-charging-70';
        } else if (batteryLevel <= 85) {
            batteryIcon = 'battery-charging-80';
        } else if (batteryLevel <= 95) {
            batteryIcon = 'battery-charging-90';
        } else if (batteryLevel <= 100) {
            batteryIcon = 'battery-charging-100';
        } else {
            batteryIcon = 'battery-unknown'
        }
    } else {
        if (batteryLevel <= 7) {
            batteryIcon = 'battery-outline';
        } else if (batteryLevel <= 15) {
            batteryIcon = 'battery-10';
        } else if (batteryLevel <= 25) {
            batteryIcon = 'battery-20';
        } else if (batteryLevel <= 35) {
            batteryIcon = 'battery-30';
        } else if (batteryLevel <= 45) {
            batteryIcon = 'battery-40';
        } else if (batteryLevel <= 55) {
            batteryIcon = 'battery-50';
        } else if (batteryLevel <= 65) {
            batteryIcon = 'battery-60';
        } else if (batteryLevel <= 75) {
            batteryIcon = 'battery-70';
        } else if (batteryLevel <= 85) {
            batteryIcon = 'battery-80';
        } else if (batteryLevel <= 95) {
            batteryIcon = 'battery-90';
        } else if (batteryLevel <= 100) {
            batteryIcon = 'battery';
        } else {
            batteryIcon = 'battery-unknown';
        }
    }
    // console.log('--- getBatteryIcon: ', batteryIcon);
    return batteryIcon;
};

// TODO:
export const getChargingState = (peripheralId) => {
    console.log('--- getChargingState: ', peripheralId);
};

// Send command to get the pillbox's time; recieve epoch seconds on TX char
export const getCurrentTime = (peripheralId) => {
    let data = [bleConst.HEADER_DEBUG[0], bleConst.HEADER_DEBUG[1], '0'.charCodeAt(0)];
    BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
        .then(() => {
            console.log('--- getCurrentTime: ');
        })
        .catch((error) => {
            console.log(error);
        });
};

// Set the pillbox's time to the device's current time
export const setCurrentTime = (peripheralId) => {
    let data      = bleConst.HEADER_CURRENT_TIME;
    const utcCurrentTime = new Date();
    const year    = utcCurrentTime.getFullYear();
    const month   = utcCurrentTime.getMonth();
    const day     = utcCurrentTime.getDate();
    const hour    = utcCurrentTime.getHours();
    const minutes = utcCurrentTime.getMinutes();
    const seconds = utcCurrentTime.getSeconds();
    const timeUTC = new Date(
        Date.UTC(year, month, day, hour, minutes, seconds)
    );
    const utcSeconds = (timeUTC.getTime() / 1000);
    const hexTime = utcSeconds.toString(16);
    const partsArr = hexTime.match(/[\s\S]{1,2}/g) || [];
    data = data.concat(partsArr.map(str => parseInt(str, 16)));
    
    return new Promise((resolve, reject) => {
        BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
            .then(() => {
                console.log('--- setCurrentTime(): ', utcCurrentTime.toDateString(), utcCurrentTime.toTimeString(), utcSeconds);
                resolve(utcSeconds);
            })
            .catch((error) => {
                console.log(error);
                reject(error);
            });
        });
};

export const requestUserPref = (peripheralId, userPref) => {
    return new Promise((resolve, reject) => {
        // Show spinner
        store.dispatch(updatePillbox({ prop: 'loading', value: true }));

        // Write command to UART char to retrieve pill box's userPref on notification
        const data = [bleConst.HEADER_USER_PREFS[0], bleConst.HEADER_USER_PREFS[1], userPref, bleConst.KEY_USER_PREFS_GET];
        BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
            .then(() => {
                // console.log('--- requestUserPref(', userPref.toString(), ')');
                store.dispatch(updatePillbox({ prop: 'loading', value: false }));
                resolve(true);
            })
            .catch((error) => {
                console.log(error);
                store.dispatch(updatePillbox({ prop: 'loading', value: false }));
                throwUartNotificationError('There was a problem with requesting a user preference. Please try again.');
                reject(error);
            });
    });
};

export const setUserPref = (peripheralId, userPref, value) => {
    // Show spinner
    store.dispatch(updatePillbox({ prop: 'loading', value: true }));

    // Set desired value into userPrefsValue for checking on receive UART notification
    store.dispatch(updatePillbox({ prop: 'userPrefsValue', value: value }));

    const data = [bleConst.HEADER_USER_PREFS[0], bleConst.HEADER_USER_PREFS[1], userPref, bleConst.KEY_USER_PREFS_SET, value];
    return new Promise((resolve, reject) => {
        BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
            .then(() => {
                console.log('--- setUserPref(', userPref.toString(), ') =', value);
                store.dispatch(updatePillbox({ prop: 'loading', value: false }));
                resolve(value);
            })
            .catch((error) => {
                console.log(error);
                store.dispatch(updatePillbox({ prop: 'loading', value: false }));
                throwUartNotificationError('There was a problem with setting a user preference. Please try again.');
                reject(error);
            });
        });
};

export const setUserPrefWord = (peripheralId, userPref, value) => {
    // Show spinner
    store.dispatch(updatePillbox({ prop: 'loading', value: true }));

    // Set desired value into userPrefsValue for checking on receive UART notification
    store.dispatch(updatePillbox({ prop: 'userPrefsValue', value: value }));

    const data = [bleConst.HEADER_USER_PREFS[0], bleConst.HEADER_USER_PREFS[1], userPref, bleConst.KEY_USER_PREFS_SET, (value & 0xFF000000)>>24, (value & 0x00FF0000)>>16, (value & 0x0000FF00)>>8, (value & 0x000000FF)];
    return new Promise((resolve, reject) => {
        BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
            .then(() => {
                console.log('--- setUserPrefWord(', userPref.toString(), ') =', value);
                store.dispatch(updatePillbox({ prop: 'loading', value: false }));
                resolve(value);
            })
            .catch((error) => {
                console.log(error);
                store.dispatch(updatePillbox({ prop: 'loading', value: false }));
                throwUartNotificationError('There was a problem with setting a user preference. Please try again.');
                reject(error);
            });
        });
};

export const setVolumeLevel = (peripheralId, volume) => {
    // Show spinner
    store.dispatch(updatePillbox({ prop: 'pendingTask', value: true }));

    // Set desired volumeLevel into userPrefsValue for checking on receive UART notification
    store.dispatch(updatePillbox({ prop: 'userPrefsValue', value: volume }));

    const data = [bleConst.HEADER_USER_PREFS[0], bleConst.HEADER_USER_PREFS[1], bleConst.KEY_VOLUME, bleConst.KEY_USER_PREFS_SET, volume];
    return new Promise((resolve, reject) => {
        BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
            .then(() => {
                console.log('--- setVolumeLevel(): ', volume, '%');
                // store.dispatch(updateVolumeLevel(volume));
                store.dispatch(updatePillbox({ prop: 'pendingTask', value: false }));
                resolve(volume);
            })
            .catch((error) => {
                console.log(error);
                store.dispatch(updatePillbox({ prop: 'pendingTask', value: false }));
                throwUartNotificationError('There was a problem with setting the volume. Please slow down and try again.');
                reject(error);
            });
        });
};

// export const increaseVolumeLevel = (peripheralId) => {
//     // Instead of nested if..else statements, use array logic on VOLUME_LEVELS?
//     // play beep at designated volume level command, SOUND_SINGLE_BEEP
//     const volume = state.pillbox.volumeLevel;
//     let newVolume = null;
//     if (volume < bleConst.VOLUME_LEVEL_HIGH) {
//         if (volume < bleConst.VOLUME_LEVEL_LOW) {
//             newVolume = bleConst.VOLUME_LEVEL_LOW;
//         } else if (volume < bleConst.VOLUME_LEVEL_MED_LOW) {
//             newVolume = bleConst.VOLUME_LEVEL_MED_LOW;
//         } else if (volume < bleConst.VOLUME_LEVEL_MED) {
//             newVolume = bleConst.VOLUME_LEVEL_MED;
//         } else if (volume < bleConst.VOLUME_LEVEL_MED_HIGH) {
//             newVolume = bleConst.VOLUME_LEVEL_MED_HIGH;
//         } else {
//             newVolume = bleConst.VOLUME_LEVEL_HIGH;
//         }
//         console.log('--- increaseVolumeLevel: ', volume, ' --> ', newVolume);
//         setVolumeLevel(peripheralId, newVolume);       //don't set? only change volume on press Save
//     } else {
//         newVolume = bleConst.VOLUME_LEVEL_HIGH;
//         console.log('Error increasing volume level: at maxVolume?', volume);
//     }
//     return newVolume;
// };

// export const decreaseVolumeLevel = (peripheralId) => {
//     const volume = state.pillbox.volumeLevel; 
//     let newVolume = null;
//     if (volume > bleConst.VOLUME_LEVEL_OFF) {
//         if (volume > bleConst.VOLUME_LEVEL_MED_HIGH) {
//             newVolume = bleConst.VOLUME_LEVEL_MED_HIGH;
//         } else if (volume > bleConst.VOLUME_LEVEL_MED) {
//             newVolume = bleConst.VOLUME_LEVEL_MED;
//         } else if (volume > bleConst.VOLUME_LEVEL_MED_LOW) {
//             newVolume = bleConst.VOLUME_LEVEL_MED_LOW;
//         } else if (volume > bleConst.VOLUME_LEVEL_LOW) {
//             newVolume = bleConst.VOLUME_LEVEL_LOW;
//         } else if (volume > bleConst.VOLUME_LEVEL_OFF) {
//             newVolume = bleConst.VOLUME_LEVEL_OFF;
//         }
//         console.log('--- decreaseVolumeLevel: ', volume, ' --> ', newVolume);
//         setVolumeLevel(peripheralId, newVolume);      //don't set? only change volume on press Save
//     } else {
//         newVolume = bleConst.VOLUME_LEVEL_LOW;
//         console.log('Error decreasing volume level: at minVolume?', volume);
//     }
//     return newVolume;
// };

export const getSoundName = (soundKey) => {
    if (soundKey === bleConst.SOUND_SINGLE_BEEP) {
        return 'SOUND_SINGLE_BEEP';
    } else if (soundKey === bleConst.SOUND_SIMPLE_BEEP) {
        return 'Beep';
    } else if (soundKey === bleConst.SOUND_SIMPLE_KNOCK) {
        return 'SOUND_SIMPLE_KNOCK';
    } else if (soundKey === bleConst.SOUND_MARIO_ONE_UP) {
        return 'SOUND_MARIO_ONE_UP';
    } else if (soundKey === bleConst.SOUND_MARIO_PAUSE) {
        return 'Pause';
    } else if (soundKey === bleConst.SOUND_MARIO_DEATH) {
        return 'SOUND_MARIO_DEATH';
    } else if (soundKey === bleConst.SOUND_MARIO_TIME_WARNING) {
        return 'SOUND_MARIO_TIME_WARNING';
    } else if (soundKey === bleConst.SOUND_MARIO_POWER_UP) {
        return 'SOUND_MARIO_POWER_UP';
    } else if (soundKey === bleConst.SOUND_MARIO_COIN) {
        return 'Coin';
    } else if (soundKey === bleConst.SOUND_MARIO_FLAGPOLE_FANFARE) {
        return 'SOUND_MARIO_FLAGPOLE_FANFARE';
    } else if (soundKey === bleConst.SOUND_MARIO_GAME_OVER) {
        return 'SOUND_MARIO_GAME_OVER';
    } else if (soundKey === bleConst.SOUND_LULLABY) {
        return 'SOUND_LULLABY';
    } else if (soundKey === bleConst.SOUND_NOTIFICATION) {
        return 'SOUND_NOTIFICATION';
    } else if (soundKey === bleConst.SOUND_NOTIFICATION2) {
        return 'SOUND_NOTIFICATION2';
    } else if (soundKey === bleConst.SOUND_DUTY_CYCLE_TEST) {
        return 'SOUND_DUTY_CYCLE_TEST';
    } else if (soundKey === bleConst.SOUND_CHROMATIC_TEST) {
        return 'SOUND_CHROMATIC_TEST';
    } else if (soundKey === bleConst.SOUND_NULL) {
        return 'SOUND_NULL';
    } else if (soundKey === bleConst.SOUND_POKEMON_HEAL) {
        return 'Heal';
    } else if (soundKey === bleConst.SOUND_SONIC_HEAL) {
        return 'Sonic';
    } else if (soundKey === bleConst.SOUND_ZELDA_SECRET) {
        return 'SOUND_ZELDA_SECRET';
    } else if (soundKey === bleConst.SOUND_ZELDA_ALERT) {
        return 'Alert';
    } else if (soundKey === bleConst.SOUND_ZELDA_KOROK) {
        return 'SOUND_ZELDA_KOROK';
    } else if (soundKey === bleConst.SOUND_POWER_RANGERS) {
        return 'SOUND_POWER_RANGERS';
    } else if (soundKey === bleConst.SOUND_KIMPOSSIBLE) {
        return 'Sitch';
    } else if (soundKey === bleConst.SOUND_PRICE_RIGHT) {
        return 'SOUND_PRICE_RIGHT';
    } else if (soundKey === bleConst.SOUND_ARPEGGIOS) {
        return 'Arpeggios 1';
    } else if (soundKey === bleConst.SOUND_ARPEGGIOS2) {
        return 'Arpeggios 2';
    } else if (soundKey === bleConst.SOUND_ARPEGGIOS3) {
        return 'Arpeggios 3';
    } else if (soundKey === bleConst.SOUND_MAJOR_SCALE) {
        return 'SOUND_MAJOR_SCALE';
    } else {
        return '';
    }
};

export const playAudioSound = (peripheralId, soundId, volume = 0) => {
    const data = [(volume !== bleConst.VOLUME_LEVEL_OFF) ? (bleConst.COMMAND_AUDIO << 4) : (bleConst.COMMAND_AUDIO_NO_VOL_CHG << 4), volume, 0, soundId].reverse();
    console.log('--- playAudioSound(): ', soundId, 'at volume: ', volume, data);
    writeEllieChar(data, peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID);
};

export const triggerDummyAlarm = (peripheralId, ledsC7, ledsC6, ledsC5, ledsC4, ledsC3, ledsC2, ledsC1) => {
    const data = [(bleConst.COMMAND_ALARM_TRIGGER << 4) | ledsC7, (ledsC6 << 4) | ledsC5, (ledsC4 << 4) | ledsC3, (ledsC2 << 4) | ledsC1].reverse();
    console.log('--- triggerDummyAlarm: ', ledsC7, ledsC6, ledsC5, ledsC4, ledsC3, ledsC2, ledsC1, data);
    writeEllieChar(data, peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID);
};

export const triggerAlarmTaken = (peripheralId) => {
    const data = [(bleConst.COMMAND_ALARM_TAKEN << 4), 0, 0, 0].reverse();
    console.log('--- triggerAlarmTaken: ');
    writeEllieChar(data, peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID);
};

export const triggerAlarmSkipped = (peripheralId) => {
    const data = [(bleConst.COMMAND_ALARM_SKIPPED << 4), 0, 0, 0].reverse();
    console.log('--- triggerAlarmSkipped: ');
    writeEllieChar(data, peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID);
};

export const triggerAlarmMissed = (peripheralId) => {
    const data = [(bleConst.COMMAND_ALARM_MISSED << 4), 0, 0, 0].reverse();
    console.log('--- triggerAlarmMissed: ');
    writeEllieChar(data, peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID);
};

// TODO:
export const findMyPillbox = () => {
    console.log('--- findMyPillbox: ');
    // flash lights, set volume to HIGH, play beacon sound
};

export const playEasterEgg = (peripheralId) => {
    console.log('--- playEasterEgg: ');
    writeEllieChar(bleConst.COMMAND_RICK, peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID);
};

export const writeAlarmData = (peripheralId, data) => {
    console.log('--- writeAlarmData: ', data);
    return new Promise((resolve, reject) => {
        BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
            .then(() => {
                resolve();
            })
            .catch((error) => {
                console.log(error);
                throwUartNotificationError('There was a problem with setting the alarm. Please try again.');
                reject(error);
            });
    });
};

export const formatAlarmData = (compartment, interval, doseTimes) => {
    // Add header
    let data = [bleConst.HEADER_ALARM[0], bleConst.HEADER_ALARM[1]];

    // Add compartment
    data.push(compartment);

    // Add start date
    const startDate = new Date(doseTimes[0].time);
    let year = startDate.getFullYear() - 1900;
    let month = startDate.getMonth();
    let day = startDate.getDate();
    data.push(year);
    data.push(month);
    data.push(day);

    // Add interval (# days)
    data.push(interval);

    // Add # alarms
    let numAlarms = doseTimes.length;
    data.push(numAlarms);

    // Add dose times
    const doseTimesCopy = doseTimes.slice();
    doseTimesCopy.forEach(dose => {
        const { time, num } = dose;
        const utcTime = new Date(time);
        const hours = utcTime.getHours();
        const minutes = utcTime.getMinutes();
        data.push((num << 6) | hours);
        data.push(minutes);
        console.log('--- doseTimes: ', utcTime.toDateString(), utcTime.toTimeString());
        // console.log('--- doseLimes: ', utcTime.toLocaleDateString(), utcTime.toLocaleTimeString());
    });
    return data;
};

export const parseAlarmData = (data) => {
    const header = [data[0], data[1]];
    const compartment = data[2];
    const startYear = data[3];
    const startMonth = data[4];
    const startDay = data[5];
    const interval = data[6];
    const numAlarms = data[7];
    let doseTimes = [];
    for (let ii = 0; ii < numAlarms; ii++) {
        doseTimes.push({
            dosage: data[(ii*2) + 8] >> 6,
            hour: data[(ii*2) + 8] & 0x3F,
            minute: data[(ii*2) + 9]
        });
    }

    return {
        header,
        compartment,
        startYear,
        startMonth,
        startDay,
        interval,
        numAlarms,
        doseTimes
    };
};

export const setAlarmData = async (peripheralId, compartment, interval, doseTimes) => {
    const data = formatAlarmData(compartment, interval, doseTimes);
    await writeAlarmData(peripheralId, data);

    // Turn off LEDs and unselect compartment
    disableLeds(peripheralId);
    store.dispatch(actionSetAlarmData());
};

export const deleteAlarmData = async (peripheralId, compartment) => {
    const data = [bleConst.HEADER_ALARM[0], bleConst.HEADER_ALARM[1], compartment, 0, 0, 0, 0, 0];
    await writeAlarmData(peripheralId, data);

    // Turn off LEDs and unselect compartment
    disableLeds(peripheralId);
    store.dispatch(actionDeleteAlarmData());
};

export const deleteAllAlarmData = async (peripheralId) => {
    await deleteAlarmData(peripheralId, bleConst.COMPARTMENT1);

    // TODO: Need to call next deleteAlarmData() from handleUartAlarmAckNotification

    // await deleteAlarmData(peripheralId, bleConst.COMPARTMENT2);
    // await deleteAlarmData(peripheralId, bleConst.COMPARTMENT3);
    // await deleteAlarmData(peripheralId, bleConst.COMPARTMENT4);
    // await deleteAlarmData(peripheralId, bleConst.COMPARTMENT5);
    // await deleteAlarmData(peripheralId, bleConst.COMPARTMENT6);
    // await deleteAlarmData(peripheralId, bleConst.COMPARTMENT7);
};

// show pendingTaskSpinner, progress bar with count
// get compl_count
// if compl_count > 0, requestPastComplianceData(compl_count)
// in handleUartPastComplData(), sendPillboxCompliance() and deletePastComplianceData(compl_count)
// in handleUartPastComplData(delete), set new compl_count
// if compl_count > 0, call fetchComplianceData
// else, stop pendingTaskSpinner, progress bar reaches 100%
export const fetchComplianceData = (dispatch, getState) => {
    const state = getState()
    const CONNECTED_PERIPHERAL = state.persistent.CONNECTED_PERIPHERAL;
    const complianceCount = state.pillbox.complianceCount;
    console.log('--- fetchComplianceData: ', complianceCount);

    if (complianceCount > 0) {
        requestPastComplianceData(CONNECTED_PERIPHERAL.id, complianceCount);
    } else {
        Alert.alert(
            'Fetch Compliance Data',
            `No data available to fetch. compl_count = ${complianceCount}`,
            [{ text: 'OK' }],
            { cancelable: true }
        );
    }
};

export const requestComplianceData = (peripheralId) => {
    const data = bleConst.HEADER_COMPL;
    BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
        .then(() => {
            console.log('--- requestComplianceData: ');
        })
        .catch((error) => {
            console.log(error);
            throwUartNotificationError('There was a problem with requesting the previous compliance data. Please try again.');
        });
};

export const requestPastComplianceData = (peripheralId, complIdx) => {
    const data = [bleConst.HEADER_PAST_COMPL[0], bleConst.HEADER_PAST_COMPL[1], bleConst.REQUEST_COMPL_DATA, (complIdx >> 8) & 0xFF, complIdx & 0xFF];
    BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
        .then(() => {
            console.log('--- requestPastComplianceData: ');
        })
        .catch((error) => {
            console.log(error);
            throwUartNotificationError(`There was a problem with requesting past compliance data[${complIdx}]. Please try again.`);
        });
};

export const requestComplianceCount = (peripheralId) => {
    const data = [bleConst.HEADER_PAST_COMPL[0], bleConst.HEADER_PAST_COMPL[1], bleConst.REQUEST_COMPL_COUNT];
    BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
        .then(() => {
            // console.log('--- requestComplianceCount(): ');
        })
        .catch((error) => {
            console.log('--- requestComplianceCount(): ', error);
            // throwUartNotificationError('There was a problem with requesting the compliance count. Please try again.');
        });
};

export const clearComplianceData = (peripheralId) => {
    const data = [bleConst.HEADER_PAST_COMPL[0], bleConst.HEADER_PAST_COMPL[1], bleConst.CLEAR_COMPL_DATA];
    BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
        .then(() => {
            console.log('--- clearComplianceData: ');
        })
        .catch((error) => {
            console.log(error);
            throwUartNotificationError('There was a problem with clearing past compliance data. Please try again.');
        });
};

export const deletePastComplianceData = (peripheralId, complIdx) => {
    const data = [bleConst.HEADER_PAST_COMPL[0], bleConst.HEADER_PAST_COMPL[1], bleConst.DELETE_PAST_COMPL_DATA, (complIdx >> 8) & 0xFF, complIdx & 0xFF];
    BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
        .then(() => {
            console.log('--- deletePastComplianceData: ');
        })
        .catch((error) => {
            console.log(error);
            throwUartNotificationError(`There was a problem with deleting past compliance data[${complIdx}]. Please try again.`);
        });
};

// LEGACY HANDLER for v1.0.0 firmware
// export const requestPastComplianceData = (peripheralId, command, val=0) => {
//     const data = [bleConst.HEADER_PAST_COMPL[0], bleConst.HEADER_PAST_COMPL[1], command, val];
//     BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
//         .then(() => {
//             console.log('--- requestPastComplianceData: ');
//         })
//         .catch((error) => {
//             console.log(error);
//             throwUartNotificationError('There was a problem with requesting past compliance data. Please try again.');
//         });
// };

export const checkAlarmData = (peripheralId) => {
    const data = bleConst.HEADER_ALARM_CHECKSUM;
    BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
        .then(() => {
            console.log('--- checkAlarmData: ');
        })
        .catch((error) => {
            console.log(error);
            throwUartNotificationError('There was a problem with checking alarm data. Please try again.');
        });
};

export const requestAlarmData = (peripheralId, compartment) => {
    const data = [bleConst.HEADER_ALARM_REQUEST[0], bleConst.HEADER_ALARM_REQUEST[1], compartment];
    BleManager.write(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID, data, 20)
        .then(() => {
            console.log('--- requestAlarmData: for compartment', compartment);
        })
        .catch((error) => {
            console.log(error);
            store.dispatch(updatePillbox({ prop: 'pendingTask', value: false }));
            throwUartNotificationError('There was a problem with requesting alarm data. Please try again.');
        });
};

export const requestAllAlarmData = (peripheralId) => {

    // Fetch alarm data from all compartments into single concatenated string
    store.dispatch(updatePillbox({ prop: 'pendingTask', value: true }));
    requestAlarmData(peripheralId, bleConst.COMPARTMENT1);
    
};

// '*' represents an LED that is on
// '-' represents an LED that is off
// NibbleVal      Compartment LED Display
// 0x0                  - - -
// 0x1                  - * -
// 0x2                  * - *
// 0x3                  * * *
// 0x4                  - - *
// 0x5                  - * *
// 0x6                  * - -
// 0x7                  * * -
export const illuminateCompartmentLeds = (peripheralId, ledsC7, ledsC6, ledsC5, ledsC4, ledsC3, ledsC2, ledsC1) => {
    // Make sure animations are stopped before setting static LED illumination
    // disableLeds(peripheralId);   // can't do sucessive writes, need to make consecutive

    const data = [(bleConst.COMMAND_LEDS << 4) | ledsC7, (ledsC6 << 4) | ledsC5, (ledsC4 << 4) | ledsC3, (ledsC2 << 4) | ledsC1].reverse();
    writeEllieChar(data, peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID);
    store.dispatch(updateLedState({ ledState: true }));
};

export const animateCompartmentLeds = (peripheralId, animation, compartment) => {
    let data = [bleConst.COMMAND_SELECT_LEDS << 4, 0, 0, 0];
    switch (compartment) {
        case bleConst.COMPARTMENT1:
            data[3] = animation;
            break;
        case bleConst.COMPARTMENT2:
            data[3] = animation << 4;
            break;
        case bleConst.COMPARTMENT3:
            data[2] = animation;
            break;
        case bleConst.COMPARTMENT4:
            data[2] = animation << 4;
            break;
        case bleConst.COMPARTMENT5:
            data[1] = animation;
            break;
        case bleConst.COMPARTMENT6:
            data[1] = animation << 4;
            break;
        case bleConst.COMPARTMENT7:
            data[0] |= animation;
            break;
        case bleConst.ALL_COMPARTMENTS:
            data[3] = animation;
            break;
        default:
    }

    data = data.reverse();
    writeEllieChar(data, peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID);
    if (compartment == bleConst.ALL_COMPARTMENTS) {
        compartment = null;
    }
    store.dispatch(updateLedState({ ledState: true, selectedCompartment: compartment }));
};

export const disableLeds = (peripheralId) => {
    const data = bleConst.ANIMATE_LEDS_STOP_DATA;
    writeEllieChar(data, peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_CHAR_UUID);
    store.dispatch(updateLedState({ ledState: false, selectedCompartment: null }));
};

// debug, alarm data
// logic to check if already connected to pill box on app return to background and first mount?
// chargeState/time, alarm data, new EllieGrid logo
// timeout for activityIndicator?

export const checkFlashStats = (peripheralId) => {
    const data = [bleConst.HEADER_DEBUG[0], bleConst.HEADER_DEBUG[1], 51];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const animateBatteryLevel = (peripheralId) => {
    const data = [bleConst.HEADER_DEBUG[0], bleConst.HEADER_DEBUG[1], 66];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const resetToFactorySettings = (peripheralId) => {
    const data = [bleConst.HEADER_FACTORY_RESET[0], bleConst.HEADER_FACTORY_RESET[1], 1];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const systemOffMode = (peripheralId) => {
    const data = [bleConst.HEADER_SYSTEM_OFF[0], bleConst.HEADER_SYSTEM_OFF[1], 1];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const softReset = (peripheralId) => {
    const data = [bleConst.HEADER_SOFT_RESET[0], bleConst.HEADER_SOFT_RESET[1], 1];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const clientDisconnect = (peripheralId) => {
    const data = [bleConst.HEADER_DISCONNECT[0], bleConst.HEADER_DISCONNECT[1]];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const easterEgg0 = (peripheralId) => {
    const data = [bleConst.HEADER_DEBUG[0], bleConst.HEADER_DEBUG[1], bleConst.EASTER_EGG_RIGHTHERE];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const easterEgg1 = (peripheralId) => {
    const data = [bleConst.HEADER_DEBUG[0], bleConst.HEADER_DEBUG[1], bleConst.EASTER_EGG_RUN];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const easterEggRegina = (peripheralId) => {
    const data = [bleConst.HEADER_DEBUG[0], bleConst.HEADER_DEBUG[1], bleConst.EASTER_EGG_REGINA];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const easterEggRainbow = (peripheralId) => {
    const data = [bleConst.HEADER_DEBUG[0], bleConst.HEADER_DEBUG[1], bleConst.EASTER_EGG_RAINBOW];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const easterEggTheaterChase = (peripheralId) => {
    const data = [bleConst.HEADER_DEBUG[0], bleConst.HEADER_DEBUG[1], bleConst.EASTER_EGG_THEATER_CHASE];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const debug7 = (peripheralId) => {
    const data = [bleConst.HEADER_DEBUG[0], bleConst.HEADER_DEBUG[1], 0x37];
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const sendWallMessage = (peripheralId, text) => {
    var data = [bleConst.HEADER_MESSAGE[0], bleConst.HEADER_MESSAGE[1]];
    for(var ii = 0; ii < text.length; ii++) {
        var val = text.charCodeAt(ii);
        data.push(val);
    }
    writeEllieChar(data, peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_RX_CHAR_UUID);
};

export const restoreDefaultUserPrefs = async (peripheralId, screen) => {
    store.dispatch(updatePillbox({ prop: 'pendingTask', value: true }));
    await setUserPref(peripheralId, bleConst.KEY_LED_R, 255);
    await setUserPref(peripheralId, bleConst.KEY_LED_G, 255);
    await setUserPref(peripheralId, bleConst.KEY_LED_B, 255);
    await setUserPrefWord(peripheralId, bleConst.KEY_CHAR_DURATION_MS, 1000);
    await setUserPrefWord(peripheralId, bleConst.KEY_INTER_CHAR_DURATION_MS, 250);
    await setUserPrefWord(peripheralId, bleConst.KEY_SPACE_DURATION_MS, 1000);
    store.dispatch(updatePillbox({ prop: 'pendingTask', value: false }));
};
