import React, {Component} from 'react';
import { Platform, StyleSheet, Text, TextInput, ScrollView, View } from 'react-native';
import { connect } from 'react-redux';
import { Button, Icon } from 'native-base';
import * as actions from '../actions';
import * as bleConst from '../static/static__bluetooth';

type Props = {};
class TabSettings extends Component<Props> {
    constructor(props) {
        super(props);
        this.state = { 
            ledR: '',
            ledG: '',
            ledB: '',
            charDuration: '',
            interCharDuration: '',
            spaceDuration: '',
        };
    }

    componentWillMount() {

    }

    async componentDidMount() {
        const { 
            CONNECTED_PERIPHERAL,
            ledR,
            ledG,
            ledB,
            charDuration,
            interCharDuration,
            spaceDuration,
        } = this.props;

        this.setState({ ledR });
        this.setState({ ledG });
        this.setState({ ledB });
        this.setState({ charDuration });
        this.setState({ interCharDuration });
        this.setState({ spaceDuration });
    }

    componentWillUnMount() {

    }

    render() {
        const {
            CONNECTED_PERIPHERAL, 
            batteryLevel,
            batteryIcon,
        } = this.props;
        const connected = CONNECTED_PERIPHERAL !== null;
        
        return (
            <ScrollView>
            <View style={styles.container}>
                <View>
                    <Text style={styles.textTitle}>Battery Level</Text>
                    <View style={styles.centerRowViewStyle}>
                        <Text style={styles.batteryLevelTextStyle}>{batteryLevel || '?'}%</Text>
                        <Icon 
                            name={batteryIcon || 'battery-unknown'}
                            type='MaterialCommunityIcons'
                            style={styles.batteryLevelIconStyle}
                        />
                    </View>
                </View>

                <Text style={styles.textTitle}>Time Settings</Text>
                <View style={styles.viewParam}>
                    <Text style={styles.textParam}>Character Duration (ms): </Text>
                    <TextInput
                        style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                        onChangeText={(charDuration) => this.setState({charDuration})}
                        value={this.state.charDuration}
                        keyboardType={'number-pad'}
                        enablesReturnKeyAutomatically={true}
                        returnKeyType={'done'}
                        onSubmitEditing={() => actions.setUserPrefWord(CONNECTED_PERIPHERAL.id, bleConst.KEY_CHAR_DURATION_MS, parseInt(this.state.charDuration))}
                    />
                </View>
                <View style={styles.viewParam}>
                    <Text style={styles.textParam}>Inter-character Duration (ms): </Text>
                    <TextInput
                        style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                        onChangeText={(interCharDuration) => this.setState({interCharDuration})}
                        value={this.state.interCharDuration}
                        keyboardType={'number-pad'}
                        enablesReturnKeyAutomatically={true}
                        returnKeyType={'done'}
                        onSubmitEditing={() => actions.setUserPrefWord(CONNECTED_PERIPHERAL.id, bleConst.KEY_INTER_CHAR_DURATION_MS, parseInt(this.state.interCharDuration))}
                    />
                </View>
                <View style={styles.viewParam}>
                    <Text style={styles.textParam}>Space Duration (ms): </Text>
                    <TextInput
                        style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                        onChangeText={(spaceDuration) => this.setState({spaceDuration})}
                        value={this.state.spaceDuration}
                        keyboardType={'number-pad'}
                        enablesReturnKeyAutomatically={true}
                        returnKeyType={'done'}
                        onSubmitEditing={() => actions.setUserPrefWord(CONNECTED_PERIPHERAL.id, bleConst.KEY_SPACE_DURATION_MS, parseInt(this.state.spaceDuration))}
                    />
                </View>                  

                <Text style={styles.textTitle}>Light Settings</Text>
                <View style={styles.viewParam}>
                    <Text style={styles.textParam}>Red (0-255): </Text>
                    <TextInput
                        style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                        onChangeText={(ledR) => this.setState({ledR})}
                        value={this.state.ledR}
                        keyboardType={'number-pad'}
                        enablesReturnKeyAutomatically={true}
                        returnKeyType={'done'}
                        onSubmitEditing={() => actions.setUserPref(CONNECTED_PERIPHERAL.id, bleConst.KEY_LED_R, parseInt(this.state.ledR))}
                    />
                </View>
                <View style={styles.viewParam}>
                    <Text style={styles.textParam}>Green (0-255): </Text>
                    <TextInput
                        style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                        onChangeText={(ledG) => this.setState({ledG})}
                        value={this.state.ledG}
                        keyboardType={'number-pad'}
                        enablesReturnKeyAutomatically={true}
                        returnKeyType={'done'}
                        onSubmitEditing={() => actions.setUserPref(CONNECTED_PERIPHERAL.id, bleConst.KEY_LED_G, parseInt(this.state.ledG))}
                    />
                </View>
                <View style={styles.viewParam}>
                    <Text style={styles.textParam}>Blue (0-255): </Text>
                    <TextInput
                        style={{height: 40, borderColor: 'gray', borderWidth: 1}}
                        onChangeText={(ledB) => this.setState({ledB})}
                        value={this.state.ledB}
                        keyboardType={'numeric'}
                        onSubmitEditing={() => actions.setUserPref(CONNECTED_PERIPHERAL.id, bleConst.KEY_LED_B, parseInt(this.state.ledB))}
                    />
                </View>
                
                <Button
                    style={styles.button}
                    active={true}
                    large
                    info
                    rounded
                    onPress={() => {
                        if (connected) {
                            actions.restoreDefaultUserPrefs(CONNECTED_PERIPHERAL.id);
                            this.setState({ ledR: '255' });
                            this.setState({ ledG: '255' });
                            this.setState({ ledB: '255' });
                            this.setState({ charDuration: '1000' });
                            this.setState({ interCharDuration: '250' });
                            this.setState({ spaceDuration: '1000' });
                        }
                    }}
                >
                    <Text style={styles.buttonText}>Restore Defaults</Text>
                </Button>
                {/* <Button
                    style={styles.button}
                    active={true}
                    large
                    warning
                    rounded
                    onPress={() => {
                        if (connected) {
                            actions.debug7(CONNECTED_PERIPHERAL.id);
                        }
                    }}
                >
                    <Text style={styles.buttonText}>Debug</Text>
                </Button> */}
            </View>
            </ScrollView>
        );
    }
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        // justifyContent: 'center',
        alignItems: 'center',
        backgroundColor: '#F5FCFF',
    },
    textTitle: {
        fontSize: 30,
        fontWeight: 'bold',
        color: 'black',
        marginTop: 20,
    },
    textParam: {
        fontSize: 20,
        color: 'black',
    },
    viewParam: {
        flexDirection: 'row', 
        justifyContent: 'center', 
        alignItems: 'center',
        marginVertical: 5,
    },
    centerRowViewStyle: { 
        flexDirection: 'row', 
        justifyContent: 'center', 
        alignItems: 'center' 
    },
    batteryLevelTextStyle: { 
        fontSize:20, 
        fontWeight:'bold', 
        color:'gray' 
    },
    batteryLevelIconStyle: { 
        fontSize:35, 
        color:'gray', 
        alignSelf:'center' 
    },
    button: {
        marginTop: 20,
        alignSelf: 'center',
    },
    buttonText: {
        color: 'white',
        fontSize: 20,
        fontWeight: 'bold',
        marginHorizontal: 50,
    },
});

const mapStateToProps = (state) => {
    const { CONNECTED_PERIPHERAL } = state.persistent;
    const {
        batteryLevel,
        batteryIcon,
        charDuration,
        interCharDuration,
        ledB,
        ledG,
        ledR,
        pendingTask,
        spaceDuration,
    } = state.pillbox;
    return {
        CONNECTED_PERIPHERAL,
        batteryLevel,
        batteryIcon,
        charDuration,
        interCharDuration,
        ledB,
        ledG,
        ledR,
        pendingTask,
        spaceDuration,
    };
};

export default connect(mapStateToProps, actions)(TabSettings);
