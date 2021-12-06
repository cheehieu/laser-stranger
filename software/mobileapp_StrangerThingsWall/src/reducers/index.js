import { combineReducers } from 'redux';
import BluetoothReducer from './reducers__bluetooth';
import PersistentStateReducer from './reducers__persistent-state';
import PillboxReducer from './reducers__pillbox';
import TransientStateReducer from './reducers__transient-state';

export default combineReducers({
    bluetooth: BluetoothReducer,
    persistent: PersistentStateReducer,
    pillbox: PillboxReducer,
    transient: TransientStateReducer,
});
