import { applyMiddleware, createStore } from 'redux';
import ReduxThunk from 'redux-thunk';
import logger from 'redux-logger';
import reducers from './reducers';

const store = createStore(
    reducers, 
    {}, 
    // Note: Uncomment/comment the following lines to enable redux-logger
    // applyMiddleware(ReduxThunk, logger)
    applyMiddleware(ReduxThunk)
);

export default store;
