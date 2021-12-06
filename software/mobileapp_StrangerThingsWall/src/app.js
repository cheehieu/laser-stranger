import React, { Component } from 'react';
import { AppState, Dimensions, NativeEventEmitter, NativeModules, Text, View } from 'react-native';
import BleManager from 'react-native-ble-manager';
import { connect } from 'react-redux';
import { goToConnect, pushConnectScreen } from './navigation/navigation';
import * as actions from './actions';
import store from './store';

const { height, width } = Dimensions.get('window');
const BleManagerModule = NativeModules.BleManager;
const bleManagerEmitter = new NativeEventEmitter(BleManagerModule);

type Props = {};
class App extends Component<Props> {
    constructor(props){
        super(props)
        this.state = {
            appState: AppState.currentState,
            // iBeacon region information
            // uuid: 'E1BEAC05-80D6-42C2-AA89-940F97C9BD04',
            // identifier: 'PillboxRegion',
            // major: 4000,    //0x0FA0;
            // minor: 2000,    //0x07D0;
        }

        // Bind event handlers to the class instance
        this._handleDiscoverPeripheral = this._handleDiscoverPeripheral.bind(this);
        this._handleStopScan = this._handleStopScan.bind(this);
        this._handleUpdateValueForCharacteristic = this._handleUpdateValueForCharacteristic.bind(this);
        this._handleDisconnectedPeripheral = this._handleDisconnectedPeripheral.bind(this);
        this._handleDidUpdateState = this._handleDidUpdateState.bind(this);
        this._handleConnectedPeripheral = this._handleConnectedPeripheral.bind(this);
    }

    componentDidMount() {
        AppState.addEventListener('change', this._handleAppStateChange);

        // Start BLE manager and configure listeners
        BleManager.start({ showAlert: true, restoreIdentifierKey: "MySuperComplexKey" })
            .then(() => {
                console.log('* BleManager module started successfully');

                // Check Android permissions, set ENABLED_BLE=true
                this.props.dispatchCheckEllieGridPermissions();
            })
            .catch((error) => {
                // 'No bluetooth support' (e.g. simulator, AVD)
                console.log('Error initializing BleManager: ', error);
                Alert.alert(
                    'Bluetooth Not Supported',
                    `This mobile device does not support Bluetooth. Please try again using a different device. ${error}`,
                    [{ text: 'OK' }],
                    { cancelable: false }
                );
            });

        // TODO: only add BLE listeners if ENABLED_BLE (which is set in checkEllieGridPermissions...)
        this.handlerDiscoverPeripheral = bleManagerEmitter.addListener('BleManagerDiscoverPeripheral', this._handleDiscoverPeripheral);
        this.handlerStopScan = bleManagerEmitter.addListener('BleManagerStopScan', this._handleStopScan);
        this.handlerDisconnectPeripheral = bleManagerEmitter.addListener('BleManagerDisconnectPeripheral', this._handleDisconnectedPeripheral);
        this.handlerUpdateValueForChar = bleManagerEmitter.addListener('BleManagerDidUpdateValueForCharacteristic', this._handleUpdateValueForCharacteristic);
        this.handlerDidUpdateState = bleManagerEmitter.addListener('BleManagerDidUpdateState', this._handleDidUpdateState);
        this.handlerConnectPeripheral = bleManagerEmitter.addListener('BleManagerConnectPeripheral', this._handleConnectedPeripheral);

        // Add listener for Dimensions device screen orientation change
        // store.dispatch(actions.setScreenDims({ height, width }));
        // Dimensions.addEventListener('change', this._handleDimensionsChange);

        // Navigate to connect screen
        pushConnectScreen();
        // goToConnect();
    }

    componentWillUnmount() {
        // Remove listeners
        AppState.removeEventListener('change', this._handleAppStateChange);
        // Dimensions.removeEventListener('change', this._handleDimensionsChange);
        this.handlerDiscoverPeripheral.remove();
        this.handlerStopScan.remove();
        this.handlerDisconnectPeripheral.remove();
        this.handlerUpdateValueForChar.remove();
        this.handlerDidUpdateState.remove();
        this.handlerConnectPeripheral.remove();

        console.log('***** APP componentWillUnmount()');
    }

    _handleAppStateChange = (nextAppState) => {
        const { appState } = this.state;
        if (appState.match(/inactive|background/) && nextAppState === 'active') {
            // console.log('_handleAppStateChange(): App has come to the foreground!');

            // // Check BLE state
            // BleManager.checkState();

            // // TODO: run getConnectedPeripherals().id == CONNECTED_PERIPHERAL.id to make sure still valid connection, 
            // // and only 1 connection also do a reconcileProcess to reset reducer states?
            // if (this.props.ENABLED_BLE) {
            //     this.props.dispatchSiftConnectedEllieGrids(); //TODO
            // }

            // // Attempt to "autoconnect" to SAVED_PERIPHERAL in 500ms
            // const AUTOCONNECT_EN = this.props.AUTOCONNECT_EN;
            // const SAVED_PERIPHERAL = this.props.SAVED_PERIPHERAL;
            // const ACCOUNT_TYPE = this.props.ACCOUNT_TYPE;
            // if ((AUTOCONNECT_EN === true) && (SAVED_PERIPHERAL !== null) && (ACCOUNT_TYPE !== 'demo')) {
            //     console.log('--- pseudoAutoConnect to ', SAVED_PERIPHERAL);
            //     this.props.updatePillbox({ prop: 'connectionMethod', value: 'pseudo' });
            //     this.props.pseudoAutoConnect(SAVED_PERIPHERAL.id, 1, 500);
            // }
        }

        // // AppState = 'inactive'
        // if ((appState.match(/active|background/)) && (nextAppState.match(/inactive/))) {
        //     console.log('AppState has become inactive.');
        // }

        // // AppState = 'background'
        // if ((appState.match(/active|inactive/)) && (nextAppState === 'background')) {
        //     console.log('AppState has become background.');
        // }

        this.setState({ appState: nextAppState });
        this.props.updateAppState(nextAppState);
    }

    _handleDimensionsChange = (dims) => {
        const dimsWindow = dims['window'];
        const mode = (dimsWindow.height > dimsWindow.width) ? 'portrait' : 'landscape';
        console.log(`_handleDimensionsChange(window) HxW: ${dimsWindow.height} x ${dimsWindow.width}`);
        console.log(`_handleDimensionsChange(window) Device is in ${mode} mode`, dims);

        // Store dims object to have access to both window and screen (height, width, scale, fontScale)
        store.dispatch(actions.setScreenDims({ dims, height: dimsWindow.height, width: dimsWindow.width }));

        // TODO: respond to device orientation changes
    }

    _handleStopScan() {
        this.props.bluetoothStopScan();
    }

    _handleDidUpdateState(update) {
        this.props.bluetoothDidUpdateState(update);
    }

    _handleDiscoverPeripheral(peripheral) {
        this.props.bluetoothDiscoverPeripheral(peripheral);
    }

    _handleUpdateValueForCharacteristic(data) {
        this.props.bluetoothUpdateValueForCharacteristic(data);
    }

    _handleConnectedPeripheral(data) {
        this.props.bluetoothConnectedPeripheral(data);
    }

    _handleDisconnectedPeripheral(data) {
        this.props.bluetoothDisconnectedPeripheral(data);
    }

    render() {
        return (
            <View />
        );
    }
}

export default connect(({persistent}) => {
    const { 
        ENABLED_BLE, 
        AUTOCONNECT_EN,
        SAVED_PERIPHERAL, 
        CONNECTED_PERIPHERAL, 
    } = persistent
    return {
        ENABLED_BLE, 
        AUTOCONNECT_EN,
        SAVED_PERIPHERAL, 
        CONNECTED_PERIPHERAL,
    }
}, actions)(App);
