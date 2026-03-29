# Smart Switch Controller

A React Native + Expo mobile app for controlling a 2-channel smart switch on an ESP32-C3 microcontroller via HTTP REST API over local Wi-Fi. This app provides a clean, intuitive interface for managing two independent relay switches from your Android device.

## Features

The Smart Switch Controller offers the following capabilities:

- **Device IP Configuration**: Store and manage the IP address of your ESP32-C3 device locally on the phone. The app persists this setting across restarts using AsyncStorage.
- **Two-Channel Control**: Independently control Switch 1 and Switch 2 with dedicated ON and OFF buttons for each channel.
- **Real-Time State Display**: Visual indicators show the current state (ON or OFF) of each switch with color-coded badges (green for ON, gray for OFF).
- **Loading Feedback**: Button states update during API requests, providing clear feedback that the command is being sent.
- **Error Handling**: Comprehensive error messages help diagnose connectivity issues with the ESP32-C3 device.
- **Haptic Feedback**: Tactile vibration feedback on button presses and command completion for enhanced user experience.
- **Dark Mode Support**: The app automatically adapts to the device's light or dark mode preference.
- **Responsive Design**: Optimized for Android portrait orientation with safe area handling for notched devices.

## Getting Started

### Prerequisites

Before you begin, ensure you have the following installed on your development machine:

- **Node.js** (version 18 or later)
- **npm** or **pnpm** (pnpm is recommended)
- **Expo CLI** (install globally with `npm install -g expo-cli`)
- **Android Studio** or **Expo Go** app on your Android device for testing

### Installation

Follow these steps to set up the project:

1. **Clone or extract the project** to your desired location:
   ```bash
   cd /home/ubuntu/smart_switch_app
   ```

2. **Install dependencies** using pnpm:
   ```bash
   pnpm install
   ```

3. **Start the development server**:
   ```bash
   pnpm dev:metro
   ```

   This command starts the Metro bundler and displays a QR code in the terminal.

### Running the App

You have two options to run the app on an Android device:

#### Option 1: Using Expo Go (Recommended for Development)

1. Install the **Expo Go** app from the Google Play Store on your Android device.
2. In the terminal where the dev server is running, you'll see a QR code.
3. Open Expo Go and scan the QR code to load the app.
4. The app will reload automatically when you make code changes.

#### Option 2: Building an APK

To create a standalone APK for distribution:

```bash
eas build --platform android --local
```

This command builds a production APK that can be installed directly on Android devices without Expo Go.

## Usage

### Configuring the Device IP

When you first open the app, follow these steps to configure your ESP32-C3:

1. Enter the local IP address of your ESP32-C3 in the **Device IP Address** field (e.g., `192.168.1.100`).
2. Tap the **Save IP** button to store the address locally.
3. A confirmation message will appear, and the current IP will be displayed below the input field.

The app will remember this IP address even after you close and reopen the app.

### Controlling Switches

Once the device IP is configured:

1. Tap the **ON** button to turn on a switch (green button).
2. Tap the **OFF** button to turn off a switch (gray button).
3. A loading indicator will appear briefly while the command is being sent.
4. The state indicator badge will update to reflect the new state.
5. If an error occurs, an alert will display with details about the failure.

### Troubleshooting

If you cannot control the switches, check the following:

- **Network Connectivity**: Ensure your phone and ESP32-C3 are connected to the same Wi-Fi network.
- **Device IP Address**: Verify that the IP address you entered is correct. You can find this in your router's connected devices list or on the ESP32-C3's serial output.
- **ESP32-C3 Firmware**: Confirm that the ESP32-C3 is running firmware that implements the required API endpoints (see below).
- **Firewall**: Check that your Wi-Fi network allows HTTP communication on port 80.
- **Device Reachability**: Try pinging the device from your phone using a network utility app to verify connectivity.

## ESP32-C3 API Specification

The app communicates with the ESP32-C3 using simple HTTP GET requests. Your firmware must implement the following endpoints:

### Endpoints

| Endpoint | Method | Description | Response |
|----------|--------|-------------|----------|
| `/switch/1/on` | GET | Turn on Switch 1 | HTTP 200 OK |
| `/switch/1/off` | GET | Turn off Switch 1 | HTTP 200 OK |
| `/switch/2/on` | GET | Turn on Switch 2 | HTTP 200 OK |
| `/switch/2/off` | GET | Turn off Switch 2 | HTTP 200 OK |

### Example Request

```
GET http://192.168.1.100/switch/1/on
```

### Example Response

A successful response should return HTTP status code 200 (OK). The response body can be empty or contain JSON data (the app ignores the body).

## ESP32-C3 Firmware Example

Below is a minimal example of how to implement the required API endpoints on your ESP32-C3 using the Arduino framework with the WiFi and WebServer libraries:

```cpp
#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Relay pins
const int RELAY_1_PIN = 4;
const int RELAY_2_PIN = 5;

// Create web server on port 80
WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Initialize relay pins
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, LOW);  // Start with relays off
  digitalWrite(RELAY_2_PIN, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Define API endpoints
  server.on("/switch/1/on", HTTP_GET, []() {
    digitalWrite(RELAY_1_PIN, HIGH);
    server.send(200, "text/plain", "Switch 1 ON");
    Serial.println("Switch 1 turned ON");
  });
  
  server.on("/switch/1/off", HTTP_GET, []() {
    digitalWrite(RELAY_1_PIN, LOW);
    server.send(200, "text/plain", "Switch 1 OFF");
    Serial.println("Switch 1 turned OFF");
  });
  
  server.on("/switch/2/on", HTTP_GET, []() {
    digitalWrite(RELAY_2_PIN, HIGH);
    server.send(200, "text/plain", "Switch 2 ON");
    Serial.println("Switch 2 turned ON");
  });
  
  server.on("/switch/2/off", HTTP_GET, []() {
    digitalWrite(RELAY_2_PIN, LOW);
    server.send(200, "text/plain", "Switch 2 OFF");
    Serial.println("Switch 2 turned OFF");
  });
  
  // Start the web server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
```

### Firmware Setup Instructions

1. **Install Arduino IDE**: Download and install the Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software).

2. **Add ESP32 Board Support**: Follow the [Espressif Arduino-ESP32 installation guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) to add ESP32 boards to the Arduino IDE.

3. **Update Credentials**: Replace `YOUR_SSID` and `YOUR_PASSWORD` with your Wi-Fi network credentials.

4. **Adjust Relay Pins**: If your relay module uses different GPIO pins, update `RELAY_1_PIN` and `RELAY_2_PIN` accordingly.

5. **Upload Firmware**: Connect your ESP32-C3 to your computer via USB, select the correct board and COM port in Arduino IDE, and click Upload.

6. **Verify Connection**: Open the Serial Monitor (9600 baud) to see the device's IP address after it connects to Wi-Fi.

## Project Structure

The project follows the Expo Router file-based routing convention:

```
smart_switch_app/
├── app/
│   ├── (tabs)/
│   │   ├── index.tsx          # Main home screen with switch controls
│   │   ├── index.test.tsx     # Unit tests for the app
│   │   └── _layout.tsx        # Tab navigation layout
│   └── _layout.tsx            # Root layout with providers
├── components/
│   ├── screen-container.tsx   # SafeArea wrapper component
│   └── ui/
│       └── icon-symbol.tsx    # Icon mapping for tab bar
├── hooks/
│   ├── use-colors.ts          # Theme colors hook
│   └── use-color-scheme.ts    # Dark/light mode detection
├── lib/
│   ├── utils.ts               # Utility functions (cn)
│   └── theme-provider.tsx     # Global theme context
├── assets/
│   └── images/                # App icons and splash screens
├── app.config.ts              # Expo app configuration
├── tailwind.config.js         # Tailwind CSS configuration
├── theme.config.js            # Theme color tokens
└── package.json               # Project dependencies
```

## Technology Stack

The Smart Switch Controller is built with modern, production-ready technologies:

- **React Native 0.81**: Cross-platform mobile framework
- **Expo SDK 54**: Managed React Native platform with built-in native modules
- **TypeScript 5.9**: Type-safe JavaScript
- **NativeWind 4**: Tailwind CSS for React Native
- **Expo Router 6**: File-based routing for navigation
- **AsyncStorage**: Local data persistence
- **Expo Haptics**: Haptic feedback support
- **Vitest**: Unit testing framework

## Development

### Running Tests

Execute the unit test suite with:

```bash
pnpm test
```

This runs all tests in the project and displays coverage information.

### Code Formatting

Format your code to match the project style:

```bash
pnpm format
```

### Type Checking

Check for TypeScript errors:

```bash
pnpm check
```

## Building for Production

To create a production-ready APK for distribution on Google Play Store or direct installation:

```bash
eas build --platform android
```

This command uses Expo's build service to compile your app in the cloud. You'll need an Expo account and the EAS CLI installed.

## Customization

### Changing Colors

Edit `theme.config.js` to customize the app's color scheme:

```javascript
const themeColors = {
  primary: { light: '#0a7ea4', dark: '#0a7ea4' },
  success: { light: '#22C55E', dark: '#4ADE80' },
  // ... other colors
};
```

### Changing the App Name

Update the `appName` field in `app.config.ts`:

```typescript
const env = {
  appName: "Your App Name",
  // ...
};
```

## Limitations and Future Enhancements

The current version has the following limitations:

- **HTTP Only**: The app uses unencrypted HTTP for communication. For production deployments, consider implementing HTTPS with self-signed certificates on the ESP32-C3.
- **No Authentication**: The API endpoints are not protected by authentication. Add API key validation to the ESP32-C3 firmware for security.
- **Single Device**: The app can only control one ESP32-C3 device at a time. Future versions could support multiple devices.
- **No State Polling**: The app does not periodically poll the device for state updates. If the switches are controlled externally, the app's state display may become out of sync.

Future enhancements could include:

- HTTPS support with certificate pinning
- API key authentication
- Support for multiple devices
- Automatic state polling or WebSocket updates
- Schedule-based automation
- History and logging of switch actions
- Voice control integration

## License

This project is provided as-is for personal and educational use.

## Support

For issues, questions, or suggestions, please refer to the project documentation or contact the development team.

## Acknowledgments

This app was built using the Expo framework and React Native ecosystem. Special thanks to the Expo team for providing an excellent development platform for mobile applications.
