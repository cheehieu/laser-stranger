import {
    CONNECT_PERIPHERAL_SUCCESS,
    DISABLE_BLUETOOTH_STATE,
    DISCONNECT_PERIPHERAL_SUCCESS,
    INIT_PILLBOX_SUCCESS,
    RESET_SESSION_STATE,
    SET_ALL_PERSISTENT,
    SET_SCREEN_DIMS,
    UPDATE_APP_STATE,
    UPDATE_BLUETOOTH_PERMISSION,
    UPDATE_BLUETOOTH_STATE,
    UPDATE_NETWORK_CONNECTION,
    UPDATE_ONE_PERSISTENT,
} from '../actions/types';
import * as bleConst from '../static/static__bluetooth';

// import AsyncStorage from '@react-native-community/async-storage';

const INITIAL_STATE = {
    ACCEPTED_TERMS: null,
    ACCOUNT_TYPE: 'default',
    APP_STATE: '',
    APP_VERSION: '',
    AUTOCONNECT_EN: bleConst.DEFAULT_AUTOCONNECT_EN,
    CONNECTED_PERIPHERAL: null,
    ENABLED_BLE: false,
    GRANTED_PERMISSIONS: '',
    ONLINE: true,
    PERIPHERAL_ID: '',
    SAVED_PERIPHERAL: '',
    SCREEN_DIMS: null,
    SCREEN_HEIGHT: null,
    SCREEN_WIDTH: null,
    SHOW_PERMISSIONS_RATIONALE: false,
 };

export default (state = INITIAL_STATE, action) => {
    // console.log('reducers__persistent', action.type, state);

    switch (action.type) {

        case UPDATE_ONE_PERSISTENT: {
            const prop = action.data.prop;
            let value = action.data.value;

            if (value === '') {
                value = INITIAL_STATE[prop];
            }

            return { ...state, [prop]: value };
        }

        case SET_ALL_PERSISTENT: {
            const props = action.data.props;
            const values = action.data.values;
            const newState = Object.assign(state, {});
            props.forEach((prop, index) => {
                if (values[index] !== null) {
                    newState[prop] = values[index];
                }
            });
            return newState;
        }

        case RESET_SESSION_STATE:
            return { 
                ...state, 
                ...INITIAL_STATE,
                APP_STATE: state.APP_STATE,
                ENABLED_BLE: state.ENABLED_BLE,
                GRANTED_PERMISSIONS: state.GRANTED_PERMISSIONS,
                SHOW_PERMISSIONS_RATIONALE: state.SHOW_PERMISSIONS_RATIONALE,
            };

        case UPDATE_NETWORK_CONNECTION:
            return { ...state, ONLINE: action.data };

        case UPDATE_APP_STATE:
            return {
                ...state,
                APP_STATE: action.payload
            };

        case UPDATE_BLUETOOTH_STATE:
            return {
                ...state,
                ENABLED_BLE: action.payload
            };

        case DISABLE_BLUETOOTH_STATE:
            return {
                ...state,
                CONNECTED_PERIPHERAL: INITIAL_STATE.CONNECTED_PERIPHERAL,
                ENABLED_BLE: INITIAL_STATE.ENABLED_BLE,
                GRANTED_PERMISSIONS: INITIAL_STATE.GRANTED_PERMISSIONS,
                SHOW_PERMISSIONS_RATIONALE: INITIAL_STATE.SHOW_PERMISSIONS_RATIONALE
            };

        case UPDATE_BLUETOOTH_PERMISSION:
            return {
                ...state,
                GRANTED_PERMISSIONS: action.payload.permissionStatus,
                SHOW_PERMISSIONS_RATIONALE: action.payload.shouldShowRequestPermissionRationale
            };

        // Set CONNECTED_PERIPHERAL after INIT_PILLBOX_SUCCESS instead
        // case CONNECT_PERIPHERAL_SUCCESS:
        //     return {
        //         ...state,
        //         CONNECTED_PERIPHERAL: action.payload.primaryPeripheral,
        //     };

        case DISCONNECT_PERIPHERAL_SUCCESS:
            return {
                ...state,
                CONNECTED_PERIPHERAL: INITIAL_STATE.CONNECTED_PERIPHERAL,
            };

        case INIT_PILLBOX_SUCCESS:
            return {
                ...state,
                CONNECTED_PERIPHERAL: action.payload.primaryPeripheral,
            };

        case SET_SCREEN_DIMS:
            return {
                ...state,
                SCREEN_DIMS: action.payload.dims,
                SCREEN_HEIGHT: action.payload.height,
                SCREEN_WIDTH: action.payload.width,
            };
        
        default:
            return state;
    }
};
