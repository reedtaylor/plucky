#ifndef _PLUCKY_CONFIG_HPP_
#define _PLUCKY_CONFIG_HPP_


// Note that all global variables defined in this 
// file with the prefix "userSettingStr_" are being
// assigned DEFAULT values here.  These values 
// can/will get overridden if the user changes them
// via WebConfig.   
// 
// Doing it this way has the nice properties of working
// cleanly if someone wants to disable web altogether.
//
#define USER_SETTING_INT_STR_LEN 8

/*************************  Serial Config *******************************/

// 1 = enable flow control by default [recommended]
// 0 = disable flow controlby default
// 
// When the Decent BLE adaptor is installed, this HW flow control 
// should be enabled [set to 1].  Otherwise data loss may occur.
//   
// If the BLE adaptor header is empty, webconfig should be used to
// disble flow control.  Probably do not need to change it here
// until a new version of DAYBREAK (beyond Mk3b) is released.
char userSettingStr_bleFlowControl[USER_SETTING_INT_STR_LEN] = "1";

/*************************  TCP Config *******************************/
char userSettingStr_tcpPort[USER_SETTING_INT_STR_LEN] = "9090";

/*************************  WebConfig Config *******************************/
#define WIFI_DEFAULT_PASSWORD "decentDE1"

// There is a poweron-password-reset capability built into iotewebconf
// Daybreak Mk3b does not have any buttons wired up, so let's use
// pin 26 which is right next to GND; hopefully easy to short in an emergency
#define WIFI_CONFIG_PIN 26

/*************************  Developer Config *******************************/

// Largest possible BLE message is ~20 bytes (packed binary)
// 64 bytes of ascii hex.  Doubling that just because it doesn't amount to much.
// Largest possible DE1 serial message therefore <64 bytes of ascii (representing 32 bytes packed) 
// Not tight on memory at the moment.
#define READ_BUFFER_SIZE 128

// When this changes, the config portal forces a reconfig
#define CONFIG_VERSION "plucky-0.04"


#endif // _PLUCKY_CONFIG_HPP_