import {
    RESET_SESSION_STATE,
    SET_ALL_PERSISTENT,
    SET_SCREEN_DIMS,
    UPDATE_APP_STATE,
    UPDATE_ONE_PERSISTENT,
} from './types';
import { getStatusBarHeight } from 'react-native-iphone-x-helper';

export const updateOnePersistent = ({ prop, value }) => ({
    type: UPDATE_ONE_PERSISTENT,
    data: { prop, value }
});

export const setAllPersistent = ({ props, values }) => ({
    type: SET_ALL_PERSISTENT,
    data: { props, values }
});

export const resetSessionState = () => ({
    type: RESET_SESSION_STATE
});

export const updateAppState = (nextAppState) => {
    return (dispatch, getState) => {
        const state = getState()
        const APP_STATE = state.persistent.APP_STATE;
        const selected = state.selected;
        dispatch( {
            type: UPDATE_APP_STATE,
            payload: nextAppState,
            APP_STATE,
            selected
        })
    }
    
};

export const setScreenDims = ({ dims, height, width }) => {
    return {
        type: SET_SCREEN_DIMS,
        payload: { dims, height, width }
    };
};
