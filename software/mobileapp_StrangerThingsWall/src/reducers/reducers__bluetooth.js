import { Alert } from 'react-native';
import {
    BEGIN_CONNECT_PERIPHERAL,
    BEGIN_DISCONNECT_PERIPHERAL,
    BEGIN_INIT_PILLBOX,
    BEGIN_SCAN,
    CLEAR_PERIPHERALS,
    CONNECT_PERIPHERAL_FAIL,
    CONNECT_PERIPHERAL_SUCCESS,
    DISABLE_BLUETOOTH_STATE,
    DISCONNECT_PERIPHERAL_FAIL,
    DISCONNECT_PERIPHERAL_SUCCESS,
    END_SCAN,
    INIT_PILLBOX_FAIL,
    INIT_PILLBOX_SUCCESS,
    RECONCILE_PROCESS_RESULTS,
    RESET_SESSION_STATE,
    SET_PERIPHERALS,
    STOP_SCAN,
    UPDATE_BLUETOOTH,
    UPDATE_BLUETOOTH_STATE,
    RESET_STATE
} from '../actions/types';

const INITIAL_STATE = {
    bleCommStatus: '',
    initializing: false,
    peripherals: new Map(),
    periphSetIdx: 0,
    scanning: false,
    sortedPeripherals: [],
};

export default (state = INITIAL_STATE, action) => {
    // console.log('reducers__bluetooth', action.type, state);

    switch (action.type) {

        case RESET_SESSION_STATE:
            return { ...state, ...INITIAL_STATE };

        case RECONCILE_PROCESS_RESULTS:
           return {
               ...state,
               initializing: INITIAL_STATE.initializing,
               peripherals: INITIAL_STATE.peripherals,
               periphSetIdx: INITIAL_STATE.periphSetIdx,
               scanning: INITIAL_STATE.scanning,
               sortedPeripherals: INITIAL_STATE.sortedPeripherals,
               // maintain bleCommStatus to display status
            };

        case DISABLE_BLUETOOTH_STATE:
            return {
                ...state,
                ...INITIAL_STATE,
                bleCommStatus: 'This device\'s Bluetooth is disabled.'
            };

        case BEGIN_SCAN:
            return {
                ...state,
                scanning: true,
                bleCommStatus: 'Scanning...',
            };

        case END_SCAN: {
            const numPillboxes = state.sortedPeripherals.length;
            let resultsStr = `Found ${numPillboxes} devices.`;

            if (numPillboxes === 1) {
                resultsStr = 'Found 1 device.';
            } else if (numPillboxes === 0) {
                resultsStr = 'No devices found.';
                const msg = 'Ensure that your device is sufficiently charged and in range, and try again.'

                Alert.alert(resultsStr, msg);
            }

            return {
                ...state,
                scanning: false,
                bleCommStatus: resultsStr,
            };
        }

        case STOP_SCAN:
            return {
                ...state,
                scanning: false,
            }

        case SET_PERIPHERALS:
            return {
                ...state,
                peripherals: action.payload.peripherals,
                sortedPeripherals: action.payload.sortedPeripherals
            };

        case CLEAR_PERIPHERALS:
            state.peripherals.clear();  // mutates state, but needed to refresh sortedArray...
            return {
                ...state,
                peripherals: INITIAL_STATE.peripherals,
                sortedPeripherals: INITIAL_STATE.sortedPeripherals,
            };

        case BEGIN_CONNECT_PERIPHERAL:
            return {
                ...state,
                bleCommStatus: 'Connecting...',
                scanning: false,
            };

        case CONNECT_PERIPHERAL_SUCCESS: {
            return {
                ...state,
                sortedPeripherals: action.payload.sortedPeripherals,
                peripherals: action.payload.peripherals,
                bleCommStatus: 'Connected.',    //on iOS, happens after Initializing... so status updates out of order
            };
        }

        case CONNECT_PERIPHERAL_FAIL:
            return {
                ...state,
                bleCommStatus: 'Connection Error.',
            };

        case BEGIN_INIT_PILLBOX:
            return {
                ...state,
                bleCommStatus: 'Initializing...',
            };

        case INIT_PILLBOX_SUCCESS:
            return {
                ...state,
                bleCommStatus: `Connected.`,
            };

        case INIT_PILLBOX_FAIL:
            return {
                ...state,
                bleCommStatus: 'Initialization Error.',
            };

        case BEGIN_DISCONNECT_PERIPHERAL:
            return {
                ...state,
                bleCommStatus: 'Disconnecting...',
            };

        case DISCONNECT_PERIPHERAL_SUCCESS:
            return {
                ...state,
                sortedPeripherals: action.payload.sortedPeripherals,
                peripherals: action.payload.peripherals,
                bleCommStatus: 'Disconnected.',
            };

        case DISCONNECT_PERIPHERAL_FAIL:
            return {
                ...state,
                bleCommStatus: 'Disconnection Error.',
            };

        case UPDATE_BLUETOOTH_STATE:
            return {
                ...state,
                bleCommStatus: INITIAL_STATE.bleCommStatus,
            };

        case UPDATE_BLUETOOTH:
            return { ...state, [action.payload.prop]: action.payload.value };   //key interpolation for propName
        case RESET_STATE:
            return INITIAL_STATE;
        default:
            return state;
    }
};
