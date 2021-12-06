import React, {Component} from 'react';
import { Platform, StyleSheet, Text, TextInput, ScrollView, View} from 'react-native';
import { connect } from 'react-redux';
import { Button } from 'native-base';
import * as actions from '../actions';
import * as bleConst from '../static/static__bluetooth';

type Props = {};
class TabMessaging extends Component<Props> {
    constructor(props) {
        super(props);
        this.state = { 
            text: '',
        };
    }

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
                <TextInput
                    style={styles.textInput}
                    onChangeText={(text) => this.setState({text})}
                    value={this.state.text}
                    editable={true}
                    multiline={true}
                    numberOfLines={5}
                    maxLength={100}
                    placeholder={'Enter a message...'}
                    clearButtonMode={'while-editing'}
                />
                <Button
                    style={styles.button}
                    active={true}
                    large
                    info
                    rounded
                    onPress={() => {
                        if (connected) {
                            actions.sendWallMessage(CONNECTED_PERIPHERAL.id, this.state.text);
                        }
                    }}
                >
                    <Text style={styles.buttonText}>Send</Text>
                </Button>
            </View>
            </ScrollView>
        );
    }
}

const styles = StyleSheet.create({
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
    container: {
        flex: 1,
        justifyContent: 'flex-start',
        alignItems: 'center',
        backgroundColor: '#F5FCFF',
    },
    textInput: {
        borderColor: 'gray',
        borderWidth: 1,
        fontSize: 20,
        marginTop: 30,
        textAlign: 'left',
        textAlignVertical: 'top',
        width: '90%',
        height: '50%',
    },
});

const mapStateToProps = (state) => {
    const { CONNECTED_PERIPHERAL } = state.persistent;
    const { pendingTask } = state.pillbox;
    return { CONNECTED_PERIPHERAL, pendingTask };
};

export default connect(mapStateToProps, actions)(TabMessaging);
