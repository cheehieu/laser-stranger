import {
    CONNECT_PERIPHERAL_SUCCESS,
    DISABLE_BLUETOOTH_STATE,
    DISCONNECT_PERIPHERAL_SUCCESS,
    INIT_PILLBOX_SUCCESS,
    RESET_SESSION_STATE,
    RESET_STATE,
    SELECT_COMPARTMENT,
    SET_PILLBOX_LOADING,
    UPDATE_BATTERY_LEVEL,
    UPDATE_DEVICE_INFO,
    UPDATE_LED_STATE,
    UPDATE_PILLBOX_STATE,
    UPDATE_PILLBOX,
    UPDATE_VOLUME_LEVEL,
} from '../actions/types';

const INITIAL_STATE = {
    alarmBetweenHbDur: '',
    alarmBetweenPingDur: '',
    alarmPingTone: null,
    alarmsDetailStr: '',
    batteryIcon: '',
    batteryLevel: null,
    buttonState: false,
    charDuration: '',
    chargingState: false,
    complianceCount: 0,
    connectionMethod: '',     // onPressPeripheral()='user', pseudoAutoConnect()='pseudo', TBD='auto'
    deviceName: '',
    fwRevision: '',
    hwRevision: '',
    interCharDuration: '',
    ledB: '',
    ledG: '',
    ledR: '',
    ledState: false,
    lidState: false,
    manufName: '',
    nusMaxDataLen: null,
    pendingTask: false,
    pendingTaskTimeoutId: null,
    primaryPeripheral: null,
    selectedCompartment: null,
    serialNumber: '',
    spaceDuration: '',
    swRevision: '',
    testingBle: false,
    timeConnected: null,
    userPrefsValue: null,
    volumeLevel: null,

    loading: false,
};

export default (state = INITIAL_STATE, action) => {
    // console.log('reducers__pillbox', action.type, state);

    switch (action.type) {

        case RESET_SESSION_STATE:
            return { ...state, ...INITIAL_STATE };

        case DISABLE_BLUETOOTH_STATE:
            return {
                ...state,
                ...INITIAL_STATE
            };

        case CONNECT_PERIPHERAL_SUCCESS:
            return { 
                ...state, 
                deviceName: action.payload.primaryPeripheral.name,
                primaryPeripheral: action.payload.primaryPeripheral,
            };

        case DISCONNECT_PERIPHERAL_SUCCESS:
            return {
                ...state,
                ...INITIAL_STATE,
                // pendingTask: state.pendingTask,
                // pendingTaskTimeoutId: state.pendingTaskTimeoutId,
                // pendingTaskTimeoutId set to null. needs to be preserved?
            };

        case INIT_PILLBOX_SUCCESS: {
            const batteryLevel = action.payload.batteryLevel || state.batteryLevel;
            const batteryIcon = action.payload.batteryIcon || state.batteryIcon;
            // const chargingState = action.payload.chargingState || state.chargingState;
            // const buttonState = action.payload.buttonState || state.buttonState;
            // const lidState = action.payload.lidState || state.lidState;
            const timeConnected = action.payload.timeConnected || state.timeConnected;
            // const volumeLevel = action.payload.volumeLevel || state.volumeLevel;

            return {
                ...state, 
                batteryLevel,
                batteryIcon,
                // chargingState,
                // buttonState,
                // lidState,
                timeConnected,
                // volumeLevel,
            };
        }

        case UPDATE_BATTERY_LEVEL:
            return { 
                ...state, 
                batteryLevel: action.payload.batteryLevel,
                batteryIcon: action.payload.batteryIcon
            };

        case UPDATE_PILLBOX_STATE:
            return { 
                ...state, 
                chargingState: action.payload.chargingState,
                buttonState: action.payload.buttonState,
                lidState: action.payload.lidState,
                batteryIcon: action.payload.batteryIcon
            };

        case UPDATE_DEVICE_INFO:
            return { 
                ...state, 
                deviceName: action.payload.deviceName,
                serialNumber: action.payload.serialNumber,
                manufName: action.payload.manufName,
                hwRevision: action.payload.hwRevision,
                fwRevision: action.payload.fwRevision,
                swRevision: action.payload.swRevision,
            };

        case SET_PILLBOX_LOADING:
            return {
                ...state,
                loading: action.payload
            };

        case SELECT_COMPARTMENT:
            return {
                ...state,
                selectedCompartment: action.payload
            };

        case UPDATE_LED_STATE:
            return {
                ...state,
                ledState: action.payload.ledState,
                selectedCompartment: action.payload.selectedCompartment
            };

        case UPDATE_VOLUME_LEVEL:
            return {
                ...state,
                volumeLevel: action.payload
            };

        case UPDATE_PILLBOX:
            return { ...state, [action.payload.prop]: action.payload.value };
        
        case RESET_STATE:
            return INITIAL_STATE;
            
        default:
            return state;
    }
};
