#ifndef _PLUCKY_CONFIG_HPP_
#define _PLUCKY_CONFIG_HPP_


// Config values are set via defines here.  

// There are some  global variables defined with the prefix "userSettingStr_".
// These string variables are being assigned DEFAULT values defined here, when they are declared.  
// These values  can/will get overridden if the user changes them via WebConfig.
//  - To change the defaults, edit the #defines in this file
//  - To use the values, read the global strings 
//  
// Doing it this way has the nice property of both working via the webconfig UI but also
// working cleanly if e.g. someone wants to disable web altogether, including the webconfig.
// In that case, the define'd default values will prevail. 
//

#define USER_SETTING_INT_STR_LEN 8

/***********************  General Config *******************/
#define DEFAULT_PROMISCUOUS "0"

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

/*************************  TCP Config *******************************/
#define DEFAULT_TCP_PORT "9090"

/*************************  WebConfig Config *******************************/
#define WIFI_DEFAULT_PASSWORD "decentDE1"

// There is a poweron-password-reset capability built into iotewebconf
// Daybreak Mk3b does not have any buttons wired up, so let's use
// pin 26 which is right next to GND; hopefully easy to short in an emergency
#define WIFI_CONFIG_PIN 26

/*************************  Developer Config *******************************/

// This should be sized no smaller than the longest possible message, plus room for
// a null-terminator. 
// Largest possible BLE message is ~20 bytes (packed binary)
// Allowing for several extra chars (e.g. "[M]", we can generously that 64 bytes of ascii hex.  
// Then let's just double that, as it doesn't amount to much and we are not tight on memory at the moment.  
#define READ_BUFFER_SIZE 128

/*************************  BLE P05 Handshake Workaround  *******************************/
// The OOB message {F}00000001 is sent from the BLE adaptor to the DE1 
// during connection startup, which enables a secondary flow control mechanism between the 
// DE1 and BLE adaptor via pin P05.  
// 
// This was added to the DE1/BLE firmware around Jul 13 2020 as a means to improve BLE reliability.
// 
// Ray H. from Decent explains:
// | The nRF51822 CPU used in the BLE module has a fundamental flaw in that the UART only has a 1-byte 
// | FIFO, and DMA to the UART is broken.
// |  
// | This was causing lost data
// | 
// | So, I manually pulled PO5 high when it is possible that the BLE module might be busy
//
// DAYBREAK Mk3b predated this change, and P05 was not connected on either end of the
// picoblade.  This results in a lockup as the DE1 believes the BLE adaptor to be busy.
// 
// The long term fix will be to maintain or propagate the P05 signal between DE1 in a future
// hardwae revision.  Or perhaps to emulate it, but presently there's no trace from the Feather
// to BLE P05 so would still require jumpering.
// 
// As a short term workaround we can either pull-down the pin on the DE1 adaptor board, or
// simply discard the OOB message {F}00000001 on its way from BLE to DE1.  We do the latter here.
// This has the unfortunate side effect of suppressing Ray's fix for the BLE lost data issue. 
// But ... every owner lived with that issue for more than two years so it's not terribly severe.
#define ENABLE_BLE_P05_WORKAROUND 1

// An OOB message can be sent to the DE1 to enable remote control.
// This is not a configuration supported by Decent and pay pose safety hazards.
#define ENABLE_REMOTE_OOB 1

// When this changes, the config portal forces a reconfig
#define CONFIG_VERSION "plucky-0.04"


#endif // _PLUCKY_CONFIG_HPP_