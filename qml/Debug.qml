/*
 * ============================================
 * File: qml/Debug.qml
 * Description: Debug logging singleton - disabled in Release builds
 * ============================================
 */

pragma Singleton
import QtQuick 2.15

QtObject {
    id: root
    
    // Enable debug logs based on build type
    // This will be controlled by C++ at startup
    property bool enabled: false
    
    // Log functions that check enabled flag
    function log(message) {
        if (enabled) {
            console.log(message)
        }
    }
    
    function debug(context, message) {
        if (enabled) {
            console.log("[DEBUG]", context + ":", message)
        }
    }
    
    function info(context, message) {
        if (enabled) {
            console.log("[INFO]", context + ":", message)
        }
    }
    
    function warn(context, message) {
        if (enabled) {
            console.warn("[WARN]", context + ":", message)
        }
    }
    
    function error(context, message) {
        // Errors always print, even in Release
        console.error("[ERROR]", context + ":", message)
    }
}
