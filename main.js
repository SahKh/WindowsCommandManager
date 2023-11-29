const { app, BrowserWindow, Tray, Menu } = require('electron');
const path = require('path');


let tray = null;
let mainWindow = null;

function createWindow() {
    mainWindow = new BrowserWindow({ width: 800, height: 600 });
    mainWindow.loadFile("index.html");

    // Handle window close event
    mainWindow.on('closed', () => {
        mainWindow = null;
    });
}

function createTray() {
    tray = new Tray(path.join(__dirname, 'icons/icon1.png'));

    const contextMenu = Menu.buildFromTemplate([
        {
            label: 'Open', // Label for opening the GUI
            click: () => { 
                createWindow();
            },
        },
        {
            label: 'Exit', // Label for exiting the application
            click: () => {
                app.quit();
            },
        },
    ]);

    tray.setToolTip('Your App Name');
    tray.setContextMenu(contextMenu);

    // Handle tray icon click event (optional)
    tray.on('click', () => {
        createWindow();
    });
}

app.on('ready', () => {
    createTray();
    createWindow();
});

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});

app.on('activate', () => {
    if (mainWindow === null) {
        createWindow();
    }
});
