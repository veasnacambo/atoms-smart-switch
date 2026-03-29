import React, { useState, useEffect } from "react";
import {
  ScrollView,
  Text,
  View,
  TextInput,
  Pressable,
  ActivityIndicator,
  Alert,
} from "react-native";
import AsyncStorage from "@react-native-async-storage/async-storage";
import * as Haptics from "expo-haptics";
import { ScreenContainer } from "@/components/screen-container";
import { cn } from "@/lib/utils";

const DEVICE_IP_KEY = "smart_switch_device_ip";

export default function HomeScreen() {
  const [deviceIp, setDeviceIp] = useState("");
  const [tempIp, setTempIp] = useState("");
  const [switch1State, setSwitch1State] = useState<"ON" | "OFF" | null>(null);
  const [switch2State, setSwitch2State] = useState<"ON" | "OFF" | null>(null);
  const [loading, setLoading] = useState(false);
  const [activeButton, setActiveButton] = useState<string | null>(null);

  // Load device IP from AsyncStorage on mount
  useEffect(() => {
    const loadDeviceIp = async () => {
      try {
        const savedIp = await AsyncStorage.getItem(DEVICE_IP_KEY);
        if (savedIp) {
          setDeviceIp(savedIp);
          setTempIp(savedIp);
        }
      } catch (error) {
        console.error("Error loading device IP:", error);
      }
    };
    loadDeviceIp();
  }, []);

  // Save device IP to AsyncStorage
  const handleSaveIp = async () => {
    if (!tempIp.trim()) {
      Alert.alert("Error", "Please enter a valid device IP address");
      return;
    }
    try {
      await AsyncStorage.setItem(DEVICE_IP_KEY, tempIp);
      setDeviceIp(tempIp);
      Alert.alert("Success", "Device IP saved successfully");
    } catch (error) {
      Alert.alert("Error", "Failed to save device IP");
      console.error("Error saving device IP:", error);
    }
  };

  // Send control command to ESP32-C3
  const sendCommand = async (channel: 1 | 2, command: "on" | "off") => {
    if (!deviceIp.trim()) {
      Alert.alert("Error", "Please configure the device IP address first");
      return;
    }

    const buttonKey = `switch${channel}_${command}`;
    setActiveButton(buttonKey);
    setLoading(true);

    try {
      // Trigger haptic feedback
      await Haptics.impactAsync(Haptics.ImpactFeedbackStyle.Light);

      const url = `http://${deviceIp}/switch/${channel}/${command}`;
      const controller = new AbortController();
      const timeoutId = setTimeout(() => controller.abort(), 5000);
      
      const response = await fetch(url, {
        method: "GET",
        signal: controller.signal,
      });
      
      clearTimeout(timeoutId);

      if (response.ok) {
        // Update state based on command
        if (channel === 1) {
          setSwitch1State(command.toUpperCase() as "ON" | "OFF");
        } else {
          setSwitch2State(command.toUpperCase() as "ON" | "OFF");
        }
        // Success haptic feedback
        await Haptics.notificationAsync(
          Haptics.NotificationFeedbackType.Success
        );
      } else {
        throw new Error(`HTTP ${response.status}`);
      }
    } catch (error) {
      console.error("Error sending command:", error);
      // Error haptic feedback
      await Haptics.notificationAsync(Haptics.NotificationFeedbackType.Error);
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      Alert.alert(
        "Error",
        `Failed to control switch ${channel}. Make sure the device IP is correct and the device is reachable. (${errorMessage})`
      );
    } finally {
      setLoading(false);
      setActiveButton(null);
    }
  };

  return (
    <ScreenContainer className="bg-background">
      <ScrollView
        contentContainerStyle={{ flexGrow: 1 }}
        className="flex-1"
        showsVerticalScrollIndicator={false}
      >
        <View className="flex-1 gap-6 p-6">
          {/* Header */}
          <View className="gap-2">
            <Text className="text-3xl font-bold text-foreground">
              Smart Switch
            </Text>
            <Text className="text-sm text-muted">
              Control your ESP32-C3 smart switches
            </Text>
          </View>

          {/* Device IP Configuration Section */}
          <View className="gap-3 bg-surface rounded-2xl p-4 border border-border">
            <Text className="text-base font-semibold text-foreground">
              Device Configuration
            </Text>
            <View className="gap-2">
              <Text className="text-xs text-muted uppercase tracking-wide">
                Device IP Address
              </Text>
              <TextInput
                value={tempIp}
                onChangeText={setTempIp}
                placeholder="e.g., 192.168.1.100"
                placeholderTextColor="#9BA1A6"
                className="bg-background border border-border rounded-lg px-4 py-3 text-foreground"
                keyboardType="decimal-pad"
                editable={!loading}
              />
              <Pressable
                onPress={handleSaveIp}
                disabled={loading}
                style={({ pressed }) => [
                  {
                    transform: [{ scale: pressed && !loading ? 0.97 : 1 }],
                    opacity: loading ? 0.6 : 1,
                  },
                ]}
                className="bg-primary rounded-lg py-3 items-center"
              >
                <Text className="text-background font-semibold">Save IP</Text>
              </Pressable>
              {deviceIp && (
                <Text className="text-xs text-success">
                  ✓ Current IP: {deviceIp}
                </Text>
              )}
            </View>
          </View>

          {/* Switch 1 Control Section */}
          <View className="gap-3 bg-surface rounded-2xl p-4 border border-border">
            <View className="flex-row items-center justify-between">
              <Text className="text-lg font-semibold text-foreground">
                Switch 1
              </Text>
              <View
                className={cn(
                  "px-3 py-1 rounded-full",
                  switch1State === "ON"
                    ? "bg-success/20"
                    : switch1State === "OFF"
                      ? "bg-muted/20"
                      : "bg-border/20"
                )}
              >
                <Text
                  className={cn(
                    "text-xs font-semibold",
                    switch1State === "ON"
                      ? "text-success"
                      : switch1State === "OFF"
                        ? "text-muted"
                        : "text-foreground"
                  )}
                >
                  {switch1State || "—"}
                </Text>
              </View>
            </View>

            <View className="flex-row gap-3">
              <Pressable
                onPress={() => sendCommand(1, "on")}
                disabled={loading || activeButton !== null}
                style={({ pressed }) => [
                  {
                    flex: 1,
                    transform: [
                      { scale: pressed && !loading ? 0.97 : 1 },
                    ],
                    opacity: loading && activeButton !== "switch1_on" ? 0.5 : 1,
                  },
                ]}
                className="bg-success rounded-lg py-3 items-center justify-center"
              >
                {activeButton === "switch1_on" && loading ? (
                  <ActivityIndicator color="#ffffff" />
                ) : (
                  <Text className="text-background font-semibold">ON</Text>
                )}
              </Pressable>

              <Pressable
                onPress={() => sendCommand(1, "off")}
                disabled={loading || activeButton !== null}
                style={({ pressed }) => [
                  {
                    flex: 1,
                    transform: [
                      { scale: pressed && !loading ? 0.97 : 1 },
                    ],
                    opacity: loading && activeButton !== "switch1_off" ? 0.5 : 1,
                  },
                ]}
                className="bg-muted/30 rounded-lg py-3 items-center justify-center border border-border"
              >
                {activeButton === "switch1_off" && loading ? (
                  <ActivityIndicator color="#11181C" />
                ) : (
                  <Text className="text-foreground font-semibold">OFF</Text>
                )}
              </Pressable>
            </View>
          </View>

          {/* Switch 2 Control Section */}
          <View className="gap-3 bg-surface rounded-2xl p-4 border border-border">
            <View className="flex-row items-center justify-between">
              <Text className="text-lg font-semibold text-foreground">
                Switch 2
              </Text>
              <View
                className={cn(
                  "px-3 py-1 rounded-full",
                  switch2State === "ON"
                    ? "bg-success/20"
                    : switch2State === "OFF"
                      ? "bg-muted/20"
                      : "bg-border/20"
                )}
              >
                <Text
                  className={cn(
                    "text-xs font-semibold",
                    switch2State === "ON"
                      ? "text-success"
                      : switch2State === "OFF"
                        ? "text-muted"
                        : "text-foreground"
                  )}
                >
                  {switch2State || "—"}
                </Text>
              </View>
            </View>

            <View className="flex-row gap-3">
              <Pressable
                onPress={() => sendCommand(2, "on")}
                disabled={loading || activeButton !== null}
                style={({ pressed }) => [
                  {
                    flex: 1,
                    transform: [
                      { scale: pressed && !loading ? 0.97 : 1 },
                    ],
                    opacity: loading && activeButton !== "switch2_on" ? 0.5 : 1,
                  },
                ]}
                className="bg-success rounded-lg py-3 items-center justify-center"
              >
                {activeButton === "switch2_on" && loading ? (
                  <ActivityIndicator color="#ffffff" />
                ) : (
                  <Text className="text-background font-semibold">ON</Text>
                )}
              </Pressable>

              <Pressable
                onPress={() => sendCommand(2, "off")}
                disabled={loading || activeButton !== null}
                style={({ pressed }) => [
                  {
                    flex: 1,
                    transform: [
                      { scale: pressed && !loading ? 0.97 : 1 },
                    ],
                    opacity: loading && activeButton !== "switch2_off" ? 0.5 : 1,
                  },
                ]}
                className="bg-muted/30 rounded-lg py-3 items-center justify-center border border-border"
              >
                {activeButton === "switch2_off" && loading ? (
                  <ActivityIndicator color="#11181C" />
                ) : (
                  <Text className="text-foreground font-semibold">OFF</Text>
                )}
              </Pressable>
            </View>
          </View>

          {/* Info Section */}
          <View className="gap-2 bg-primary/10 rounded-lg p-3 border border-primary/20">
            <Text className="text-xs text-primary font-semibold">
              💡 Tip
            </Text>
            <Text className="text-xs text-foreground leading-relaxed">
              Make sure your device is connected to the same Wi-Fi network as
              your ESP32-C3. Use the device's local IP address (e.g.,
              192.168.x.x).
            </Text>
          </View>
        </View>
      </ScrollView>
    </ScreenContainer>
  );
}
