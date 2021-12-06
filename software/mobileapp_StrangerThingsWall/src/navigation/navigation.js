import { Navigation } from 'react-native-navigation';
import Icon from 'react-native-vector-icons/MaterialCommunityIcons';

export const goToTabs = () => {
    Promise.all([
        Icon.getImageSource('wall', 30, '#2d3436'),
        Icon.getImageSource('message-text', 30, '#0984e3'),
        Icon.getImageSource('lightbulb-on', 30, '#fff200'),
        Icon.getImageSource('settings', 30, '#95a5a6'),
    ]).then((res) => {
        Navigation.setRoot({
            root: {
                stack: {
                    id: 'App',
                    children: [
                    {
                        component: {
                            name: 'Connect',
                        }
                    },
                    {
                        bottomTabs: {
                            id: 'BottomTabsId',
                            children: [
                                {
                                    component: {
                                        name: 'TabAlphabetWall',
                                        options: {
                                            bottomTab: {
                                                fontSize: 12,
                                                text: 'Wall',
                                                icon: res[0],
                                                selectedIconColor: '#2d3436',
                                                selectedTextColor: 'black',
                                            }
                                        }
                                    },
                                },
                                {
                                    component: {
                                        name: 'TabMessaging',
                                        options: {
                                            bottomTab: {
                                            text: 'Message',
                                            fontSize: 12,
                                            icon: res[1],
                                            selectedIconColor: '#0984e3',
                                            selectedTextColor: 'black',
                                            }
                                        }
                                    },
                                },
                                {
                                    component: {
                                        name: 'TabLightPatterns',
                                        options: {
                                            bottomTab: {
                                                text: 'Light Patterns',
                                                fontSize: 12,
                                                icon: res[2],

                                                selectedIconColor: '#f6e58d',
                                                selectedTextColor: 'black',
                                            }
                                        }
                                    },
                                },
                                {
                                    component: {
                                        name: 'TabSettings',
                                        options: {
                                            bottomTab: {
                                                text: 'Settings',
                                                fontSize: 12,
                                                icon: res[3],
                                                selectedIconColor: '#95a5a6',
                                                selectedTextColor: 'black',
                                            }
                                        }
                                    },
                                },
                            ],
                        }
                    }
                    ],
                }
            }
        });
    })
};

export const goToConnect = () => Navigation.setRoot({
    root: {
        sideMenu: {
            left: {
                component: {
                    name: 'LeftSideMenu',
                    passProps: {
                        text: 'This is a left side menu screen'
                    }
                }
            },
            center: {
                component: {
                    name: 'Connect'
                },
            },
            right: {
                component: {
                    name: 'RightSideMenu',
                    passProps: {
                        text: 'This is a right side menu screen'
                    }
                }
            }
        }
    }
});

export const pushTabsScreen = () => {
    Promise.all([
        Icon.getImageSource('wall', 30, '#2d3436'),
        Icon.getImageSource('message-text', 30, '#0984e3'),
        Icon.getImageSource('lightbulb-on', 30, '#fff200'),
        Icon.getImageSource('settings', 30, '#95a5a6'),
    ]).then((res) => {
        return Navigation.push('App', {
            bottomTabs: {
                id: 'BottomTabsId',
                children: [
                    {
                        component: {
                            name: 'TabAlphabetWall',
                            options: {
                                bottomTab: {
                                    fontSize: 12,
                                    text: 'Wall',
                                    icon: res[0],
                                    selectedIconColor: '#2d3436',
                                    selectedTextColor: 'black',
                                }
                            }
                        },
                    },
                    {
                        component: {
                            name: 'TabMessaging',
                            options: {
                                bottomTab: {
                                text: 'Message',
                                fontSize: 12,
                                icon: res[1],
                                selectedIconColor: '#0984e3',
                                selectedTextColor: 'black',
                                }
                            }
                        },
                    },
                    {
                        component: {
                            name: 'TabLightPatterns',
                            options: {
                                bottomTab: {
                                    text: 'Light Patterns',
                                    fontSize: 12,
                                    icon: res[2],

                                    selectedIconColor: '#f6e58d',
                                    selectedTextColor: 'black',
                                }
                            }
                        },
                    },
                    {
                        component: {
                            name: 'TabSettings',
                            options: {
                                bottomTab: {
                                    text: 'Settings',
                                    fontSize: 12,
                                    icon: res[3],
                                    selectedIconColor: '#95a5a6',
                                    selectedTextColor: 'black',
                                }
                            }
                        },
                    },
                ],
            }
        });
    })
};

export const pushConnectScreen = () => {
    return Navigation.push('App', {
        sideMenu: {
            // left: {
            //     component: {
            //         name: 'LeftSideMenu',
            //         passProps: {
            //             text: 'This is a left side menu screen'
            //         }
            //     }
            // },
            center: {
                component: {
                    name: 'Connect',
                    options: {
                        topBar: {
                            visible: false,
                            height: 0
                        }
                    }
                },
            },
            // right: {
            //     component: {
            //         name: 'RightSideMenu',
            //         passProps: {
            //             text: 'This is a right side menu screen'
            //         }
            //     }
            // }
        },
    });
};
