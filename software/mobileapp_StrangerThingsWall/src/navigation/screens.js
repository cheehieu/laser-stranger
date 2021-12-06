import { Navigation } from 'react-native-navigation';
import { Provider } from 'react-redux';

export const registerScreensWithRedux = (store, Provider) => {
    // Navigation.registerComponentWithRedux('LeftSideMenu', () => require('../components/tab_settings').default, Provider, store);
    // Navigation.registerComponentWithRedux('RightSideMenu', () => require('../components/tab_messaging').default, Provider, store);
    Navigation.registerComponentWithRedux('Connect', () => require('../components/connect').default, Provider, store);
    Navigation.registerComponentWithRedux('TabAlphabetWall', () => require('../components/tab_alphabet_wall').default, Provider, store);
    Navigation.registerComponentWithRedux('TabMessaging', () => require('../components/tab_messaging').default, Provider, store);
    Navigation.registerComponentWithRedux('TabLightPatterns', () => require('../components/tab_light_patterns').default, Provider, store);
    Navigation.registerComponentWithRedux('TabSettings', () => require('../components/tab_settings').default, Provider, store);
}
