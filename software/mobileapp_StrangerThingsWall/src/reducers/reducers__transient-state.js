import {
    RESET_TRANSIENT_STATE,
    UPDATE_ONE_TRANSIENT,
} from '../actions/types';

const INITIAL_STATE = {
    devicesFound: {},
    loading: null,
};

export default (state = INITIAL_STATE, action) => {
    // console.log('reducers__transient', action.type, state);

    switch (action.type) {

        case RESET_TRANSIENT_STATE:
            return { ...state, ...INITIAL_STATE };

        case UPDATE_ONE_TRANSIENT:
            return { ...state, [action.data.prop]: action.data.value };
        
        default:
            return state;
    }
};
