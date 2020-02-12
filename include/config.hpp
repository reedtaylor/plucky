#ifndef _PLUCKY_CONFIG_HPP_
#define _PLUCKY_CONFIG_HPP_


// Config values are set via defines here.  

// There are some  global variables defined(extern) in this file with the prefix "userSettingStr_".
// These string variables are being assigned DEFAULT values here (via the defines).  
// These values  can/will get overridden if the user changes them via WebConfig.
//  - To change the defaults, edit the #defines
//  - To use the values, read the global strings 
//  
// Doing it this way has the nice property of both working via the webconfig UI but also
// working cleanly if e.g. someone wants to disable web altogether, including the webconfig.
// In that case, the define'd default values will prevail. 
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
#define DEFAULT_BLE_FLOW_CONTROL "1"

#ifndef SKIP_GLOBAL_EXTERNS
extern char *userSettingStr_bleFlowControl; 
#endif // SKIP_GLOBAL_EXTERNS

/*************************  TCP Config *******************************/
#define DEFAULT_TCP_PORT "9090"

#ifndef SKIP_GLOBAL_EXTERNS
extern char *userSettingStr_tcpPort;
#endif // SKIP_GLOBAL_EXTERNS

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