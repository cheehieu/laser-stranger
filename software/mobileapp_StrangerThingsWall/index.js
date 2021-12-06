import { Navigation } from 'react-native-navigation';
import { Provider } from 'react-redux';
import { registerScreensWithRedux } from './src/navigation/screens';
import store from './src/store';
import App from './src/app';

Navigation.registerComponentWithRedux('App', () => App, Provider, store);
registerScreensWithRedux(store, Provider);

Navigation.events().registerAppLaunchedListener(() => {
    Navigation.setDefaultOptions({
        topBar: {
            background: {
                color: '#444'
            },
            title: {
                color: 'white',
            },
            backButton: {
                title: '', // Remove previous screen name from back button
                color: 'white'
            },
            buttonColor: 'white',
        },
        statusBar: {
            style: 'light'
        },
        // layout: {
        //     orientation: ['portrait']
        // },
        bottomTabs: {
            // titleDisplayMode: 'alwaysShow'
        },
        bottomTab: {
            textColor: '#BBB',
            iconColor: '#BBB',
            selectedTextColor: 'black',
            selectedIconColor: 'black',
        }
    });

    Navigation.setRoot({
        root: {
            stack: {
                id: 'App',
                children: [
                    {
                        component: {
                            name: 'App',
                        }
                    }
                ],
            }
        }
    });
});
