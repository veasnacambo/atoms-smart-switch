# Smart Switch Controller - Design Plan

## Overview
A simple, clean mobile app for controlling a 2-channel smart switch on an ESP32-C3 via HTTP REST API over local Wi-Fi. The app targets Android with a portrait orientation and one-handed usage in mind.

## Screen List

1. **Home Screen (Main Control Screen)**
   - Device IP configuration input
   - Switch 1 control section
   - Switch 2 control section
   - Visual state indicators

## Primary Content and Functionality

### Home Screen
- **Device IP Input Section** (top)
  - Text input field for device IP address (e.g., "192.168.1.100")
  - Save button or auto-save on blur
  - Display current device IP or placeholder text

- **Switch 1 Control Section** (middle-top)
  - Title: "Switch 1"
  - Current state indicator (visual: color change, text: "ON" or "OFF")
  - ON button (green, prominent)
  - OFF button (gray/muted)
  - Button feedback on press

- **Switch 2 Control Section** (middle-bottom)
  - Title: "Switch 2"
  - Current state indicator (visual: color change, text: "ON" or "OFF")
  - ON button (green, prominent)
  - OFF button (gray/muted)
  - Button feedback on press

- **Status/Error Display**
  - Show loading state while sending requests
  - Display error messages if API calls fail
  - Show success feedback (brief toast or color flash)

## Key User Flows

### Flow 1: Configure Device IP
1. User enters device IP address in the text input field
2. App stores IP locally (AsyncStorage)
3. IP persists across app restarts

### Flow 2: Control Switch 1 ON
1. User taps "Switch 1 ON" button
2. App sends GET request to `http://{device_ip}/switch/1/on`
3. Button shows loading state
4. On success: Switch 1 state updates to "ON" (green indicator)
5. On error: Display error message

### Flow 3: Control Switch 1 OFF
1. User taps "Switch 1 OFF" button
2. App sends GET request to `http://{device_ip}/switch/1/off`
3. Button shows loading state
4. On success: Switch 1 state updates to "OFF" (gray indicator)
5. On error: Display error message

### Flow 4: Control Switch 2 (same as Flow 2 & 3, but for channel 2)
1. User taps "Switch 2 ON" or "Switch 2 OFF" button
2. App sends GET request to `http://{device_ip}/switch/2/on` or `/switch/2/off`
3. State updates and displays

## Color Choices

- **Primary Color**: `#0a7ea4` (teal/blue) - for active states and accents
- **Success/ON State**: `#22C55E` (green) - indicates switch is ON
- **OFF State**: `#9BA1A6` (muted gray) - indicates switch is OFF
- **Background**: `#ffffff` (white) in light mode, `#151718` (dark) in dark mode
- **Surface**: `#f5f5f5` (light gray) in light mode, `#1e2022` (dark) in dark mode
- **Error**: `#EF4444` (red) - for error messages
- **Text**: `#11181C` (dark) in light mode, `#ECEDEE` (light) in dark mode

## Layout Specifications

- **Safe Area**: Use ScreenContainer for proper SafeArea handling
- **Padding**: 16-20px horizontal padding, 12-16px vertical padding between sections
- **Button Size**: 48-56px height for touch targets (iOS HIG standard)
- **Typography**: 
  - Section titles: 18-20px, bold
  - Button labels: 16px, medium weight
  - State indicator: 14px, regular
- **Spacing**: 12-16px between major sections
- **Corner Radius**: 8-12px for buttons and input fields

## Interaction Patterns

- **Button Press Feedback**: Scale 0.97 + haptic feedback (light impact)
- **Loading State**: Disable button, show spinner or opacity change
- **Error Feedback**: Red text, optional error toast
- **Success Feedback**: Brief color flash or state update animation
