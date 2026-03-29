const fs = require("fs");
const path = require("path");
const dotenv = require("dotenv");

function loadEnv() {
  try {
    // Path to .env file (project root)
    const envPath = path.resolve(__dirname, "../.env");

    if (fs.existsSync(envPath)) {
      dotenv.config({ path: envPath });
      console.log("✅ Loaded .env from:", envPath);
    } else {
      console.log("⚠️ No .env file found at:", envPath);
    }
  } catch (error) {
    console.error("❌ Error loading .env:", error);
  }
}

module.exports = { loadEnv };
