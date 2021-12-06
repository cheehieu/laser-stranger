import React, {Component} from 'react';
import { Image, Platform, StyleSheet, Text, ScrollView, View } from 'react-native';
import { connect } from 'react-redux';
import { Row } from 'native-base';
import * as actions from '../actions';
import * as bleConst from '../static/static__bluetooth';

type Props = {};
class TabAlphabetWall extends Component<Props> {

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
                {/* <Image
                    style={styles.image}
                    source={require('../images/alphabet_wall_off.png')}
                /> */}
                <Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'A') }}> A </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'B') }}> B </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'C') }}> C </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'D') }}> D </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'E') }}> E </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'F') }}> F </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'G') }}> G </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'H') }}> H </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'I') }}> I </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'J') }}> J </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'K') }}> K </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'L') }}> L </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'M') }}> M </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'N') }}> N </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'O') }}> O </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'P') }}> P </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'Q') }}> Q </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'R') }}> R </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'S') }}> S </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'T') }}> T </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'U') }}> U </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'V') }}> V </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'W') }}> W </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'X') }}> X </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'Y') }}> Y </Text>
                    <Text style={styles.textLetter} onPress={ () => {if (connected) actions.sendWallMessage(CONNECTED_PERIPHERAL.id, 'Z') }}> Z </Text>
                </Text>
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
        backgroundColor: '#55efc4',
        marginHorizontal: 20,
        paddingHorizontal: 40,
    },
    image: {
        // flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
        resizeMode: 'contain',
        width: '100%',
        height: '100%',
    },
    textLetter: {
        fontSize: 65,
        fontWeight: 'bold',
        color: 'black',
    }
});

const mapStateToProps = (state) => {
    const { CONNECTED_PERIPHERAL } = state.persistent;
    const { pendingTask } = state.pillbox;
    return { CONNECTED_PERIPHERAL, pendingTask };
};

export default connect(mapStateToProps, actions)(TabAlphabetWall);
