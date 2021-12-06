import {
    UPDATE_ONE_TRANSIENT,
    RESTORE_TRANSIENT_FROM_PERSISTENT,
    RESET_TRANSIENT_STATE,
    CLOSE_PROMPT
} from './types';

export const updateOneTransient = ({ prop, value }) => ({
    type: UPDATE_ONE_TRANSIENT,
    data: { prop, value }
});

export const restoreTransientFromPersistent = ({ props, values }) => ({
    type: RESTORE_TRANSIENT_FROM_PERSISTENT,
    data: { props, values }
});

export const resetTransientState = () => ({
    type: RESET_TRANSIENT_STATE
});

export const closePrompt = () => ({
    type: CLOSE_PROMPT
});
