import React, { Component } from 'react';
import { Alert, FlatList, SafeAreaView, StyleSheet } from 'react-native';
import NetInfo from '@react-native-community/netinfo';
import { connect } from 'react-redux';
import { Body, Button, Fab, Icon, Left, ListItem, Right, Text } from 'native-base';
import { PendingTaskIndicator } from './common';
import * as actions from '../actions';
import * as bleConst from '../static/static__bluetooth';
import { pushTabsScreen } from '../navigation/navigation';

type Props = {};
class Connect extends Component<Props> {
    state = {
        refreshing: false,
    }

    componentDidMount() {
        this.props.startEllieGridScan(bleConst.SCAN_MODE_LOW_LATENCY);
    }

    /**
     * Handler for refreshing the list view with a swipe gesture
     * on refresh, clear peripheral list and scan again
     */
    onRefresh() {
        const { clearPeripherals, startEllieGridScan } = this.props;
        this.setState({ refreshing: true });

        // Clear peripheral list
        clearPeripherals();

        // Scan again
        this.setState({ refreshing: false });
        startEllieGridScan(bleConst.SCAN_MODE_LOW_LATENCY);
    }

    /**
     * Handler for pressing a peripheral in the list view
     * if peripheral is disconnected, attempt to connect (and vice-versa)
     */
    async onPressPeripheral(peripheral) {
        const { connectPeripheral, disconnectPeripheral, scanning } = this.props;

        if (peripheral) {
            // Stop scanning
            if (scanning) {
                await actions.stopEllieGridScan();
            }

            // Attempt to connect/disconnect
            if (peripheral.connected == true) {
                disconnectPeripheral(peripheral.id);
            } else {
                connectPeripheral(peripheral.id);
            }
        }
    }

    render() {
        const {
            CONNECTED_PERIPHERAL, 
        } = this.props;
        const connected = CONNECTED_PERIPHERAL !== null;

        return (
            <SafeAreaView style={styles.container}>
                <Icon
                    name='wifi'
                    type='MaterialCommunityIcons'
                    style={{marginTop: 30, padding: 5, fontSize: 50, color: '#3498db', backgroundColor: '#bdc3c7' }}
                    onPress={ () => {
                        NetInfo.fetch().then(state => {
                            console.log(state);
                        });

                        // NetInfo.isConnected.fetch().then(isConnected => {
                        //     if (isConnected) {
                        //         console.log(`isConnected? ${isConnected}`);
                        //     }
                        // });
                    }}
                />
                <Text style={{fontStyle: 'italic'}}>NetInfo</Text>

                <Icon
                    name='bluetooth'
                    type='MaterialCommunityIcons'
                    style={{marginTop: 30, padding: 5, fontSize: 50, color: '#3498db', backgroundColor: '#bdc3c7' }}
                    onPress={ () => {
                        this.props.startEllieGridScan(bleConst.SCAN_MODE_LOW_LATENCY);
                    }}
                />

                <Text style={{fontStyle: 'italic'}}>{this.props.bleCommStatus}</Text>

                <FlatList
                    style={styles.flatlist}
                    data={this.props.sortedPeripherals}
                    keyExtractor={item => item.id}
                    refreshing={this.state.refreshing}
                    onRefresh={this.onRefresh.bind(this)}
                    renderItem={({ item }) => {
                        const listItemColor = (item.connected) ? '#2ecc71' : '#f0f0f0';
                        const textWeight = (item.connected) ? 'bold' : 'normal';
                        const leftIconName = (item.connected) ? 'bluetooth-connected' : 'bluetooth-disabled';
                        const leftIconColor = (item.connected) ? '#0082fc' : '#bdc3c7';
                        return (
                            <ListItem icon style={{ backgroundColor: listItemColor, marginTop: 10, marginBottom: 10 }} onPress={this.onPressPeripheral.bind(this, item)}>
                                <Left>
                                    <Button>
                                        <Icon active name='bluetooth' />
                                    </Button>
                                </Left>
                                <Body>
                                    <Text style={{ color: '#000', fontSize: 24, fontWeight: textWeight }}>{item.name}</Text>
                                    {/* <Text note>{item.id}</Text> */}
                                </Body>
                                <Right>
                                    <Text note>{item.rssi}</Text>
                                </Right>
                            </ListItem>
                        );
                    }}
                />

                { this.props.pendingTask && 
                    <PendingTaskIndicator
                        spinnerAnimating={this.props.pendingTask}
                        text={this.props.bleCommStatus}
                    />
                }

                { connected && 
                    <Fab
                        active={true}
                        style={{ backgroundColor: '#f1c40f' }}
                        position="topRight"
                        onPress={() => { if (connected) pushTabsScreen() }}>
                        <Icon name="md-arrow-round-forward" />
                    </Fab>
                }
            </SafeAreaView>
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
    list: {
        flexDirection: 'row',
        flex: 1,
        backgroundColor: '#0d47a1', 
        margin: 10,
        marginTop: 5,
        borderTopWidth: 0,
        borderBottomWidth: 0,
        borderRadius: 5,
    },
    flatlist: {
        backgroundColor: '#f8f8f8',
        margin: 20,
        width: '80%',
    },
    title: {
        fontSize: 30,
        fontWeight: 'bold',
        margin: 20,
        width: '80%',
    },
});

const mapStateToProps = (state) => {
    const { scanning, sortedPeripherals, bleCommStatus } = state.bluetooth;
    const { CONNECTED_PERIPHERAL, ENABLED_BLE } = state.persistent;
    const { pendingTask } = state.pillbox;
    return { CONNECTED_PERIPHERAL, scanning, sortedPeripherals, bleCommStatus, ENABLED_BLE, pendingTask };
};

export default connect(mapStateToProps, actions)(Connect);
