import React from 'react';
import { ActivityIndicator, StyleSheet, Text, View } from 'react-native';
import { Colors } from './styles__colors';

const { BLACK, WHITE } = Colors;

const PendingTaskIndicator = (props) => {
    const {
        customTextStyle,
        customContainerStyle,
        spinnerAnimating,
        spinnerColor,
        spinnerSize,
        textNumLines,
        text,
        // TODO: add support for a <visible> prop instead of conditioning in the parent View
    } = props;

    return (
        <View style={[styles.viewStyle, customContainerStyle]}>
            <ActivityIndicator
                animating={spinnerAnimating || false}
                color={spinnerColor || WHITE}
                size={spinnerSize || 'large'}
            />
            <Text
                style={[styles.textStyle, customTextStyle]}
                numberOfLines={textNumLines || 10}
            >
                {text}
            </Text>
        </View>
    );
};

const styles = StyleSheet.create({
    textStyle: {
        color: WHITE,
        // fontFamily: 'SourceSansPro-Bold',
        fontSize: 18,
        fontStyle: 'italic',
        letterSpacing: 2,
        margin: 10,
        marginTop: 5
    },
    viewStyle: {
        justifyContent: 'center',
        alignItems: 'center',
        position: 'absolute',
        left: 0,
        right: 0,
        top: 0,
        bottom: 0,
        backgroundColor: BLACK,
        opacity: 0.5,
    },
});

export { PendingTaskIndicator };
