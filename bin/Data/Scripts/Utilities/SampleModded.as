// Common sample initialization as a framework for all samples.
//    - Create Urho3D logo at screen;
//    - Set custom window title and icon;
//    - Create Console and Debug HUD, and use F1 and F2 key to toggle them;
//    - Toggle rendering options from the keys 1-8;
//    - Take screenshots with key 9;
//    - Handle Esc key down to hide Console or exit application;
//    - Init touch input on mobile platform using screen joysticks (patched for each individual sample)

Sprite@ logoSprite;
Scene@ scene_;
uint screenJoystickIndex = M_MAX_UNSIGNED; // Screen joystick index for navigational controls (mobile platforms only)
uint screenJoystickSettingsIndex = M_MAX_UNSIGNED; // Screen joystick index for settings (mobile platforms only)
bool touchEnabled = false; // Flag to indicate whether touch input has been enabled
bool paused = false; // Pause flag
bool drawDebug = false; // Draw debug geometry flag
Node@ cameraNode; // Camera scene node
float yaw = 0.0f; // Camera yaw angle
float pitch = 0.0f; // Camera pitch angle
const float TOUCH_SENSITIVITY = 2;
