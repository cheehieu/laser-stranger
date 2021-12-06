/* eslint react-native/split-platform-components: 0 */
import { Alert, PermissionsAndroid, Platform, NativeModules, NativeEventEmitter  } from 'react-native';
import BleManager from 'react-native-ble-manager';
import DeviceSettings from 'react-native-device-settings';
import { pushTabsScreen } from '../navigation/navigation';

import store from '../store';
import * as bleConst from '../static/static__bluetooth';
import {
    BEGIN_CONNECT_PERIPHERAL,
    BEGIN_DISCONNECT_PERIPHERAL,
    BEGIN_SCAN,
    CLEAR_PERIPHERALS,
    CONNECT_PERIPHERAL_FAIL,
    CONNECT_PERIPHERAL_SUCCESS,
    DISABLE_BLUETOOTH_STATE,
    DISCONNECT_PERIPHERAL_FAIL,
    DISCONNECT_PERIPHERAL_SUCCESS,
    END_SCAN,
    SET_PERIPHERALS,
    STOP_SCAN,
    UPDATE_BLUETOOTH_PERMISSION,
    UPDATE_BLUETOOTH_STATE,
    UPDATE_BLUETOOTH,
} from './types';

import {
    handleBatteryNotification,
    handleStatusNotification,
    handleUartNotification,
    initializePillbox,
    initializePillboxFail,
    initializePillboxSuccess,
    runPostConnection,
    updatePillbox,
} from './actions__pillbox';
// import { setAsyncStoragePersistent } from './actions__resolve-processes';
// import { Navigation } from 'react-native-navigation';

// const Beacons = NativeModules.Beacons
// const BeaconsEmitter = new NativeEventEmitter(Beacons)

export const updateBluetooth = ({ prop, value }) => {
    return {
        type: UPDATE_BLUETOOTH,
        payload: { prop, value }
    };
};

export const updateBluetoothState = (enabledBle) => {
    return {
        type: UPDATE_BLUETOOTH_STATE,
        payload: enabledBle
    };
};

export const disableBluetoothState = () => {
    return {
        type: DISABLE_BLUETOOTH_STATE
    };
};

export const updateBluetoothPermission = ({ permissionStatus, shouldShowRequestPermissionRationale }) => {
    return {
        type: UPDATE_BLUETOOTH_PERMISSION,
        payload: { permissionStatus, shouldShowRequestPermissionRationale }
    };
};

export const beginScan = () => {
    return {
        type: BEGIN_SCAN
    };
};

export const endScan = () => {
    return {
        type: END_SCAN
    };
};

export const stopScan = () => {
    return {
        type: STOP_SCAN
    };
};

export const setPeripherals = ({ peripherals, sortedPeripherals }) => {
    return {
        type: SET_PERIPHERALS,
        payload: { peripherals, sortedPeripherals }
    };
};

export const clearPeripherals = () => {
    return {
        type: CLEAR_PERIPHERALS
    };
};

export const beginConnectPeripheral = () => {
    return {
        type: BEGIN_CONNECT_PERIPHERAL
    };
};

export const connectPeripheralSuccess = ({ primaryPeripheral, peripherals, sortedPeripherals }) => {
    return (dispatch, getState) =>{
        const selectedCompartment = getState().pillbox.selectedCompartment;
        dispatch({
            type: CONNECT_PERIPHERAL_SUCCESS,
            payload: { primaryPeripheral, peripherals, sortedPeripherals },
            selectedCompartment
        });
    }
    
    
};

export const connectPeripheralFail = () => {
    return {
        type: CONNECT_PERIPHERAL_FAIL
    };
};

export const beginDisconnectPeripheral = () => {
    return {
        type: BEGIN_DISCONNECT_PERIPHERAL
    };
};

export const disconnectPeripheralSuccess = ({ peripherals, sortedPeripherals }) => {
    return {
        type: DISCONNECT_PERIPHERAL_SUCCESS,
        payload: { peripherals, sortedPeripherals }
    };
};

export const disconnectPeripheralFail = () => {
    return {
        type: DISCONNECT_PERIPHERAL_FAIL
    };
};

// export const startMonitoring = () =>{
//     return (getState) => {
//         const url = "https://us-central1-elliegrid-app.cloudfunctions.net/monitoringNBeaconCheck?event=didStartRegion"
//         fetch(url).catch(err => alert(err))
//         const { persistent } = getState()
//         const { AUTOCONNECT_EN, ACCOUNT_TYPE, SAVED_PERIPHERAL } = persistent
//         if (AUTOCONNECT_EN && SAVED_PERIPHERAL && ACCOUNT_TYPE !== 'demo'){
//             Beacons.startMonitoringRegion(
//                 "6055892b-1c2d-411b-885c-3022c16ce5aa", 
//                 0, 
//                 0, 
//                 "identifier"
//             )
//         }
//     }
// }

// export const didEnterRegion = () =>{
//     return (dispatch, getState) => {
//         const url = "https://us-central1-elliegrid-app.cloudfunctions.net/monitoringNBeaconCheck?event=didEnterRegion"
//         fetch(url).catch(err => alert(err))
//         const { persistent } = getState()
//         const { AUTOCONNECT_EN, ACCOUNT_TYPE, SAVED_PERIPHERAL } = persistent
//         if (AUTOCONNECT_EN && SAVED_PERIPHERAL && ACCOUNT_TYPE !== 'demo'){
//             return dispatch(pseudoAutoConnect(SAVED_PERIPHERAL.id, 1, 0,true));
//         }
//     }
// }
// export const stopMonitoring = () => {
//     Beacons.stopMonitoringRegion(
//         "E1BEAC05-80D6-42C2-AA89-940F97C9BD02",
//         4000, 
//         2000, 
//         "identifier"
//     )
// }


/**
 * Handler for when a BLE scan stops
 */
export const bluetoothStopScan = () => {
    return (dispatch, getState) => {
        // stopPendingTimeout();
        // console.log('*** handleStopScan(): Scan is stopped');
        dispatch(endScan());
    }
};

/**
 * Handler for when BLE state changes
 * alert when BLE is off, ask user to turn on in settings
 * if permissions not granted, request or ask user to change app settings
 * @param {object} update - .state: 'off', 'turning_off', 'on', 'turning_on'
 */
export const bluetoothDidUpdateState = (update) => {
    return (dispatch, getState) =>{
        const state = getState()
        const appState = state.persistent.APP_STATE;
        const permissionStatus = state.persistent.GRANTED_PERMISSIONS;
        // console.log('*** handleDidUpdateState(): ', 'bleState:', update.state, '| appState:', appState, '| permissionStatus:', permissionStatus);
        if (permissionStatus === 'granted') {
            if (update.state === 'on') {
                dispatch(updateBluetoothState(true));
            } else if (update.state === 'off' && appState === 'active') {
                dispatch(disableBluetoothState());    //clear bluetooth data
                Alert.alert(
                    'Bluetooth Disabled',
                    'To continue, please make sure this device\'s Bluetooth is turned ON.',
                    [
                        { text: 'Cancel', style: 'cancel' },
                        { text: 'Settings', onPress: () => DeviceSettings.bluetooth() },
                    ],
                    { cancelable: false }
                );
            }
        } else {
            // Alert user to change permissions in app settings
            // --BUG if user allows permissions while Alert is open (did not press Cancel or Settings), then presses Cancel
            // --app will not register permissions change and thus won't automatically update permissionStatus
            // --resolve by triggering an AppStateChange
            if (permissionStatus === 'never_ask_again') {
                Alert.alert(
                    'Permission Request',
                    'EllieGrid will not function properly without the location permission. Please change it in the app settings.',
                    [
                        {
                            text: 'Cancel',
                            onPress: () => {
                                // store.dispatch(updateBluetoothPermission({ permissionStatus: '', shouldShowRequestPermissionRationale: false }));
                            },
                            style: 'cancel'
                        },
                        {
                            text: 'Settings',
                            onPress: () => {
                                dispatch(updateBluetoothPermission({ permissionStatus: '', shouldShowRequestPermissionRationale: false }));
                                DeviceSettings.app();
                            }
                        },
                    ],
                    { cancelable: false }
                );
            } else {
                // Check permissions, request if not permitted
                checkEllieGridPermissions(dispatch, getState);
            }
        }
    }
};

/**
 * Handler for when a a BLE peripheral is discovered during a scan
 * resorts the peripherals from discovery order to perceived range order (closest first)
 * @param {*} peripheral
 */
export const bluetoothDiscoverPeripheral = (peripheral) => {
    return (dispatch, getState) => {
        const state = getState()
        const peripherals = state.bluetooth.peripherals; //grabbing old value?
        if (!peripherals.has(peripheral.id)) {
            // console.log('*** _handleDiscoverPeripheral(): Found BLE peripheral ', peripheral);

            // Check if peripheral == PERIPHERAL_ID, so we can fix it near top of sortedArray
            const pidString = JSON.stringify(peripheral.id);
            const pidPersistent = state.persistent.PERIPHERAL_ID;
            if (pidString === pidPersistent) {
                // console.log(`Found matching PERIPHERAL_ID (${pidPersistent}) with RSSI: ${peripheral.rssi}`);
                peripheral.rssi = -1;   // setting fake RSSI so it will show up near top (after connected RSSI=0)
            }

            // Add peripheral into peripherals Map
            peripherals.set(peripheral.id, peripheral);

            // Sort peripherals by RSSI
            const sortedArray = sortPeripherals(peripherals);

            // peripherals being mutated with .set(), no need to pass?
            dispatch(setPeripherals({ peripherals: peripherals, sortedPeripherals: sortedArray }));
        }
    }
};

export const sortPeripherals = (peripherals) => {
    // TODO: sortedArray creation in external function, in order to generate new sortedArray after setting/clearing connected flag in peripherals (for UI)
    // Sort peripherals by descending RSSI (less negative <-> closer)
    let sortedArray = Array.from(peripherals.values());
    sortedArray.sort((a, b) => {
        return b.rssi - a.rssi;
    });

    // [iOS only?] Check for RSSI=127 (0x7f), [RSSI value cannot be read], move to end of array
    for (let ii = 0; ii < sortedArray.length - 1; ii++) {
        if (sortedArray[ii].rssi === 127) {
            // console.log('--- RSSI could not be read: ', sortedArray[ii]);
            sortedArray = sortedArray.concat(sortedArray.splice(0,1));
        }
    }

    return sortedArray;
}

/**
 * Handler for when data is received from BLE characteristic
 * @param {*} data
 */
export const bluetoothUpdateValueForCharacteristic = (data) => {
    return (dispatch, getState) =>{
        data.service = data.service.toUpperCase();
        data.characteristic = data.characteristic.toUpperCase();
        if (data.service === bleConst.EG_UART_SERVICE_UUID && data.characteristic === bleConst.EG_UART_TX_CHAR_UUID) {
            console.log('--- notification from EG_UART_TX_CHAR_UUID: ', data.value);
            handleUartNotification(data.value, dispatch, getState);
        } else if ((data.service === bleConst.BATTERY_SERVICE_UUID && data.characteristic === bleConst.BATTERY_LEVEL_CHAR_UUID) ||
            (data.service === bleConst.NOTIFY_BATTERY_SERVICE_UUID && data.characteristic === bleConst.NOTIFY_BATTERY_LEVEL_CHAR_UUID)) {
            console.log('--- notification from BATTERY_LEVEL_CHAR_UUID: ', data.value);
            handleBatteryNotification(data.value, dispatch, getState);
        } else if (data.service === bleConst.EG_COMMAND_SERVICE_UUID && data.characteristic === bleConst.EG_COMMAND_STATUS_CHAR_UUID) {
            console.log('--- notification from EG_COMMAND_STATUS_CHAR_UUID: ', data.value);
            handleStatusNotification(data.value, dispatch, getState);
        } else {
            console.log('_handleUpdateValueForCharacteristic(): ', data);
        }
    }
    
};

/**
 * Handler for when a BLE peripheral is connected
 * @param {*} data
 */
export const bluetoothConnectedPeripheral = (data) => {
    return (dispatch, getState)=>{
        // if(Platform.OS === 'android'){
        //     Beacons.pauseMonitoring()
        // }
        // fetch("https://us-central1-elliegrid-app.cloudfunctions.net/monitoringNBeaconCheck?event=BLEConnectionSuccessfull1")
        const state = getState()
        const AUTOCONNECT_EN = state.persistent.AUTOCONNECT_EN;
        const connectionMethod = state.pillbox.connectionMethod;

        console.log('*** component handleConnectedPeripheral(): CONNECTED to ', data.peripheral);
        // Handle redux state connected logic
        // logic to set p.connected = true... [mutates state.peripherals] --> move to reducer?
        const peripherals = state.bluetooth.peripherals;
        // // const AUTOCONNECT_EN = state.persistent.AUTOCONNECT_EN;
        // // const connectionMethod =state.pillbox.connectionMethod;
        // fetch(`https://us-central1-elliegrid-app.cloudfunctions.net/monitoringNBeaconCheck?event=${connectionMethod}`)

        const p = ((AUTOCONNECT_EN === true) && (connectionMethod === 'pseudo')) ? state.persistent.SAVED_PERIPHERAL : peripherals.get(data.peripheral);
        if (p) {
            p.connected = true;
            peripherals.set(data.peripheral, p);
            const sortedArray = sortPeripherals(peripherals);

            // Set connected peripheral's RSSI to 0 to keep sorted at the top
            p.rssi = 0;
            dispatch(connectPeripheralSuccess({ primaryPeripheral: p, peripherals: peripherals, sortedPeripherals: sortedArray }));
        }
    }
    
};

/**
 * Handler for when a BLE peripheral is disconnected
 * iOS: listener not called when disconnecting from Nordic app --> left in perpetual loading state
 * @param {*} data
 */
export const bluetoothDisconnectedPeripheral = (data) => {
    return (dispatch, getState) =>{
        const state = getState()
        console.log('*** handleDisconnectedPeripheral(): DISCONNECTED from', data.peripheral);
        // Error with pseudo-autoconnect to SAVED_PERIPHERAL
        const AUTOCONNECT_EN = state.persistent.AUTOCONNECT_EN;
        const CONNECTED_PERIPHERAL = state.persistent.CONNECTED_PERIPHERAL;
        const connectionMethod = state.pillbox.connectionMethod;
        if ((AUTOCONNECT_EN === true) && (connectionMethod === 'pseudo')) {
            console.log('--- bluetoothDisconnectedPeripheral() | Unable to connect to SAVED_PERIPHERAL:', data.peripheral);
        }

        // if(Platform.OS === 'android' && CONNECTED_PERIPHERAL != null && AUTOCONNECT_EN){
        //     Beacons.continueMonitoring()
        // }
        // logic to set p.connected = false... [mutates state.peripherals] --> move to reducer?
        const peripherals = state.bluetooth.peripherals;
        const p = peripherals.get(data.peripheral);
        if (p) {
            p.connected = false;
            peripherals.set(data.peripheral, p);
            const sortedArray = sortPeripherals(peripherals);
            dispatch(disconnectPeripheralSuccess({ peripherals: peripherals, sortedPeripherals: sortedArray }));
        } else {
            dispatch(disconnectPeripheralSuccess({ peripherals: new Map(), sortedPeripherals: [] })); // reset to empty Map, array
        }
    }
    
};

/**
 * Check for required app permissions [only needed in Android 23+]
 * if permissions granted, proceed to check on BLE state
 * if permissions denied, request the permissions
 */
export const checkEllieGridPermissions = (dispatch, getState) => {
    if (Platform.OS === 'android' && Platform.Version >= 23) {
        PermissionsAndroid.check(PermissionsAndroid.PERMISSIONS.ACCESS_COARSE_LOCATION)
            .then((result) => {
                if (result) {
                    // console.log('* checkEllieGridPermissions(): ACCESS_COARSE_LOCATION Permission is', result);
                    dispatch(updateBluetoothPermission({ permissionStatus: 'granted', shouldShowRequestPermissionRationale: false }));
                    BleManager.enableBluetooth()
                        .then(() => {
                            // console.log('* enableBluetooth(): already enabled OR User allowed to turn on');
                            dispatch(updateBluetoothState(true));
                        })
                        .catch((error) => {
                            console.log('* enableBluetooth(): User refused to enable Bluetooth: ', error);
                            BleManager.checkState();
                        });
                } else {
                    requestLocationPermission(dispatch,getState);
                }
            })
            .catch((error) => console.log(error));
    } else {
        // No extra permissions are needed for iOS or Android < 23
        dispatch(updateBluetoothPermission({ permissionStatus: 'granted', shouldShowRequestPermissionRationale: false }));
        BleManager.checkState();
    }
};
export const dispatchCheckEllieGridPermissions=()=>{
    return (dispatch, getState)=>{
        checkEllieGridPermissions(dispatch, getState)
    }
}
/**
 * Request the ACCESS_COARSE_LOCATION permission [only in Android 23+]
 * if user grants request, proceed to check on BLE state
 * if user denies request, display rationale and request again
 * if user selects never_ask_again, alert rationale and link to app settings
 */
export const requestLocationPermission = (dispatch,getState) => {
    let rationale = {};
    const state = getState();
    if (state.persistent.SHOW_PERMISSIONS_RATIONALE) {
        rationale = {
            title: 'Permission Rationale',
            message: 'EllieGrid needs to access this device\'s approximate location in order to communicate with your pill box.'
        };
    }
    PermissionsAndroid.request(PermissionsAndroid.PERMISSIONS.ACCESS_COARSE_LOCATION, rationale)
        .then((result) => {
            // console.log('*** requestLocationPermission(): ', result);
            if (result === 'granted') {
                dispatch(updateBluetoothPermission({ permissionStatus: result, shouldShowRequestPermissionRationale: false }));
                BleManager.checkState();
            } else if (result === 'denied') {
                dispatch(updateBluetoothPermission({ permissionStatus: result, shouldShowRequestPermissionRationale: true }));
            } else {
                dispatch(updateBluetoothPermission({ permissionStatus: result, shouldShowRequestPermissionRationale: false }));
            }
        })
        .catch((error) => console.log(error));
};

/**
 * BLE scan for peripherals advertising the EG_UART_SERVICE_UUID
 * scan lasts for SCAN_SECONDS, event listener bluetoothStopScan() on scan finish
 * @param {int} mode - scan mode for Android (latency vs. power)
 */
export const startEllieGridScan = (mode) => {
    return (dispatch, getState) => {
        const state = getState();
        if (state.persistent.ENABLED_BLE && state.bluetooth.scanning === false) {
            // Check for existing connected peripherals to include them at the top of scan render
            siftConnectedEllieGrids(dispatch, getState)
                .then(() => {
                    // Perform scan
                    BleManager.scan([bleConst.EG_UART_SERVICE_UUID], bleConst.SCAN_SECONDS, false, { scanMode: mode })
                        .then(() => {
                            // setPendingTimeout(bleConst.TIMEOUT_MS_SCAN); // commenting this to prevent transparent overlay
                            store.dispatch(beginScan());
                        })
                        .catch((error) => console.log(error));  //TODO: add support for scan error
                })
                .catch((error) => console.log(error));
        }
    };
};

/**
 * Stop the BLE scan
 */
export const stopEllieGridScan = () => {
    BleManager.stopScan()
        .then(() => {
            // Set scanning flag false in case END_SCAN reducer not called
            store.dispatch(stopScan());
            // console.log('--- Scan stopped.');
        })
        .catch((error) => console.log(error));
};

export const dispatchSiftConnectedEllieGrids = () =>{
    return (dispatch, getState)=>{
        siftConnectedEllieGrids(dispatch, getState)
    }
}
// Check if connected matches with CONNECTED_PERIPHERAL and only 1 connection
export const siftConnectedEllieGrids = (dispatch, getState) => {
    return new Promise((resolve, reject) => {
        BleManager.getConnectedPeripherals([bleConst.EG_UART_SERVICE_UUID])
            .then((peripheralsArray) => {
                // console.log('--- siftConnectedEllieGrids Found ', peripheralsArray.length, 'connected peripherals: ', peripheralsArray);

                // If found 0 connected peripherals, set state to disconnected
                // CONNECTED_PERIPHERAL, remove all peripherals' connected flag


                // Else if 1 connected peripheral, verify sole connection matches with with CONNECTED_PERIPHERAL


                // Else, disconnect from all connections (except CONNECTED_PERIPHERAL?) (try disconnecting from client side?)


                // // attempt a disconnect here
                // if (peripheralsArray[0]) {
                //     console.log('--- attempt disconnect from ', peripheralsArray[0].id);
                //     disconnectPeripheral(peripheralsArray[0].id);
                // }

                const state = getState()
                // Merge the peripheralsArray to peripherals and set the connected flag
                const peripherals = state.bluetooth.peripherals;
                for (let ii = 0; ii < peripheralsArray.length; ii++) {
                    // Check if peripheralsArray[ii] is in peripherals, set to connected and add to peripherals
                    if (!peripherals.has(peripheralsArray[ii].id)) {
                        peripheralsArray[ii].connected = true;
                        peripherals.set(peripheralsArray[ii].id, peripheralsArray[ii]);
                    } else {
                        const p = peripherals.get(peripheralsArray[ii].id);
                        p.connected = true;
                        peripherals.set(peripheralsArray[ii].id, p);
                    }

                    // Set last connected peripheral as the CONNECTED_PERIPHERAL and sort to render in FlatList
                    if (ii == peripheralsArray.length - 1) {
                        const sortedArray = sortPeripherals(peripherals);

                        // This also "clears" the bluetooth reducer state
                        dispatch(connectPeripheralSuccess({ primaryPeripheral: peripheralsArray[peripheralsArray.length - 1], peripherals: peripherals, sortedPeripherals: sortedArray }));
                    }
                }

                resolve(true);
            })
            .catch((error) => {
                console.log(error);
                reject(error);
            });
    });
};

/**
 * Connect to a BLE peripheral and retrieve services and chars
 * show spinner until successful connection
 * if error, show alert with details and try again
 */
export const connectPeripheral = (peripheralId, isWokenByBeacon) => {
    return (dispatch, getState) => {
        setPendingTimeout(bleConst.TIMEOUT_MS_CONNECT, dispatch, getState);
        dispatch(beginConnectPeripheral());     //endConnectPeripheral?
        BleManager.connect(peripheralId)
            .then(async () => {
                // stopPendingTimeout(false, dispatch, getState);

                // // Retrieve services and start notifications
                // await BleManager.retrieveServices(peripheralId); // const peripheralDetails
                // await BleManager.startNotification(peripheralId, bleConst.EG_UART_SERVICE_UUID, bleConst.EG_UART_TX_CHAR_UUID);
                // await BleManager.startNotification(peripheralId, bleConst.EG_COMMAND_SERVICE_UUID, bleConst.EG_COMMAND_STATUS_CHAR_UUID);
                // await BleManager.startNotification(peripheralId, bleConst.BATTERY_SERVICE_UUID, bleConst.BATTERY_LEVEL_CHAR_UUID);

                // pushTabsScreen();


                stopPendingTimeout(bleConst.KEEP_PENDING_TASK, dispatch, getState);

                // Initialize pill box
                setPendingTimeout(bleConst.TIMEOUT_MS_INITIALIZE, dispatch, getState);
                initializePillbox(peripheralId, dispatch, getState)
                    .then((result) => {
                        const state = getState();
                        stopPendingTimeout(false,dispatch,getState);
                        
                        // Merge pillbox.primaryPeripheral to result
                        const primaryPeripheral = state.pillbox.primaryPeripheral;
                        const mergedResult = Object.assign(result, { primaryPeripheral });
                        dispatch(initializePillboxSuccess(mergedResult));

                        // Navigate to tabs
                        // pushTabsScreen();

                        runPostConnection(peripheralId, dispatch, getState);
                    })
                    .catch((error) => {
                        const state = getState();
                        stopPendingTimeout(false,dispatch,getState);
                        console.log(error);
                        dispatch(initializePillboxFail());
                        dispatch(disconnectPeripheral(peripheralId));
                    });
            })
            .catch((error) => {
                const state = getState();
                stopPendingTimeout(false, dispatch, getState);
                dispatch(connectPeripheralFail());
                console.log(error);

                Alert.alert(
                    'Connection Error',
                    'There was a problem connecting. Please try again.',
                    [{ text: 'OK' }],
                    { cancelable: true }
                );
            });
    };
};

/**
 * Disconnect from a peripheral
 */
export const disconnectPeripheral = (peripheralId) => {
    return (dispatch, getState) => {
        setPendingTimeout(bleConst.TIMEOUT_MS_DISCONNECT, dispatch, getState);
        dispatch(beginDisconnectPeripheral());

        // TODO: Uninitialize pill box and stop notifications before disconnecting? uninitializePillbox()
        BleManager.disconnect(peripheralId)
            .then(() => {
                stopPendingTimeout(false,dispatch,getState);
                // console.log('--- Disconnected.');
                const state = getState()
                const peripherals = state.bluetooth.peripherals;
                const sortedPeripherals = state.bluetooth.sortedPeripherals;
                dispatch(disconnectPeripheralSuccess({ peripherals: peripherals, sortedPeripherals: sortedPeripherals }));
            })
            .catch((error) => {
                const state = getState()
                stopPendingTimeout(false,dispatch,getState);
                console.log(error);
                dispatch(disconnectPeripheralFail());
                Alert.alert(
                    'Connection Error',
                    'There was a problem disconnecting. Please try again or close this app.',
                    [{ text: 'OK' }],
                    { cancelable: true }
                );
            });
    };
};

// Attempt to connect to peripheralId num_attempts times, every attempt_ms seconds
export const pseudoAutoConnect = (peripheralId, num_attempts, attempt_ms, isWokenByBeacon) => {
    return (dispatch, getState) =>{
        const state = getState()
        // Return if already connected
        if (state.persistent.CONNECTED_PERIPHERAL != null) {
            console.log('---pseudoAutoConnect() ALREADY CONNECTED TO A PILL BOX');
            return;
        }
        
        // loop num_attempts, every attempt_secs
        // TODO: Set autoconnect flag to differentiate between user connect (display error messages)
        // console.log('---pseudoAutoConnect() attempting to connect to ', peripheralId, num_attempts, attempt_ms);
        if(isWokenByBeacon){
            dispatch(connectPeripheral(peripheralId, isWokenByBeacon));
        }else{
            const pseudoAutoConnectId = setTimeout(() => {
                // console.log('---pseudoAutoConnectId ', pseudoAutoConnectId, num_attempts, attempt_ms);
                return dispatch(connectPeripheral(peripheralId));
                // recursively call pseudoAutoConnect(num_attempts-1)
            }, attempt_ms);
        }
    }
   
};

export const clearAllPeripherals = () => {
    store.dispatch(clearPeripherals());
};

/**
 * Stop pendingTask and display alert message if task exceeds time_ms
 * Used for connect, disconnect, initializePillbox, critical BLE data transfer (alarms, compliance)
 * TODO: change instances of loading to pendingTask
 */
export const setPendingTimeout = (time_ms, dispatch, getState) => {
    dispatch(updatePillbox({ prop: 'pendingTask', value: true }));

    // Check if pending flag still set
    const pendingTimeoutId = setTimeout(() => {
        const state = getState()
        const pendingTaskFlag = state.pillbox.pendingTask;
        const pendingTaskTimeoutIdFlag = state.pillbox.pendingTaskTimeoutId;
        console.log('--- TIMEOUT!!! setPendingTimeout(', time_ms, '): ', pendingTaskFlag, pendingTaskTimeoutIdFlag);
        if (pendingTaskFlag) {
            console.log('--- pending flag still set after ', time_ms / 1000, 'seconds');
            dispatch(updatePillbox({ prop: 'pendingTask', value: false }));

            // Prevent alert during report notification scene and pseudo
            const connectionMethod = state.pillbox.connectionMethod;  // gets cleared on disconnectPeripheral()
            Alert.alert(
                'Timeout Error',
                'There was a problem completing that task... Please try again.',
                [{ text: 'OK' }],
                { cancelable: true }
            );
        }
    }, time_ms);

    dispatch(updatePillbox({ prop: 'pendingTaskTimeoutId', value: pendingTimeoutId }));
    // console.log('--- setPendingTimeout() id: ', pendingTimeoutId);
};

/**
 * Stops the current setTimeout()
 * Used for connect, disconnect, initializePillbox, critical BLE data transfer (alarms, compliance)
 */
export const stopPendingTimeout = (keepPendingTask = false, dispatch, getState) => {
    const state = getState();
    const pendingTimeoutId = state.pillbox.pendingTaskTimeoutId;
    clearTimeout(pendingTimeoutId);
    dispatch(updatePillbox({ prop: 'pendingTaskTimeoutId', value: null }));

    if (keepPendingTask !== bleConst.KEEP_PENDING_TASK) {
        dispatch(updatePillbox({ prop: 'pendingTask', value: false }));
    }
    // console.log('--- stopPendingTimeout() id: ', pendingTimeoutId);
};
