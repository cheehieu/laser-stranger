import React, {Component} from 'react';
import { Platform, StyleSheet, Text, ScrollView, View } from 'react-native';
import { connect } from 'react-redux';
import { Icon } from 'native-base';
import * as actions from '../actions';
import * as bleConst from '../static/static__bluetooth';

type Props = {};
class TabLightPatterns extends Component<Props> {

    componentWillMount() {

    }

    componentDidMount() {

    }

    componentWillUnMount() {

    }

    render() {
        const {
            CONNECTED_PERIPHERAL, 
        } = this.props;
        const connected = CONNECTED_PERIPHERAL !== null;
        
        return (
            <ScrollView>
            <View style={styles.container}>
                <Text
                    style={styles.textQuestion}
                    onPress={ () => {
                        if (connected) {
                            actions.easterEgg0(CONNECTED_PERIPHERAL.id);
                        }
                    }}
                >
                    Where are you?
                </Text>

                <Text
                    style={styles.textQuestion}
                    onPress={ () => {
                        if (connected) {
                            actions.easterEgg1(CONNECTED_PERIPHERAL.id);
                        }
                    }}
                >
                    What should I do?
                </Text>

                <Text
                    style={styles.textQuestion}
                    onPress={ () => {
                        if (connected) {
                            const val = Math.random();
                            if (val < 0.5) {
                                actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'YES');
                            } else {
                                actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'NO');
                            }
                        }
                    }}
                >
                    Ask a Y/N question...
                </Text>

                <Icon 
                    name='egg-easter'
                    type='MaterialCommunityIcons'
                    style={{fontSize: 50, color: '#2ecc71'}}
                    onPress={ () => {
                        if (connected) {
                            actions.easterEggRegina(CONNECTED_PERIPHERAL.id);
                        }
                    }}
                />

                <Icon 
                    name='palette'
                    type='MaterialCommunityIcons'
                    style={{fontSize: 50, color: '#c0392b'}}
                    onPress={ () => {
                        if (connected) {
                            actions.easterEggRainbow(CONNECTED_PERIPHERAL.id);
                        }
                    }}
                />

                <Icon 
                    name='alarm-light'
                    type='MaterialCommunityIcons'
                    style={{fontSize: 50, color: '#3498db'}}
                    onPress={ () => {
                        if (connected) {
                            actions.easterEggTheaterChase(CONNECTED_PERIPHERAL.id);
                        }
                    }}
                />
            </View>
            </ScrollView>
        );
    }
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
        backgroundColor: '#F5FCFF',
    },
    textQuestion: {
        fontSize: 40,
        fontWeight: 'bold',
        fontStyle: 'italic',
        color: 'black',
        margin: 10,
        backgroundColor: '#bdc3c7',
    }
});

const mapStateToProps = (state) => {
    const { CONNECTED_PERIPHERAL } = state.persistent;
    const { pendingTask } = state.pillbox;
    return { CONNECTED_PERIPHERAL, pendingTask };
};

export default connect(mapStateToProps, actions)(TabLightPatterns);
