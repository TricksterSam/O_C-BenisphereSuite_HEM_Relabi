/*
 *
 * compile options.
 *
 */

#ifndef OC_OPTIONS_H_
#define OC_OPTIONS_H_

/* ------------ uncomment for Buchla/non-octaval/quartertone support: -------------------------------  */
//#define BUCHLA_SUPPORT
/* ------------ uncomment for use with Northernlight cOC program card: ------------------------------  */
//#define BUCHLA_cOC
/* ------------ uncomment for use with Northernlight 4U / cOC or 2OC: -------------------------------  */
//#define BUCHLA_4U
/* ------------ uncomment for use with Northernlight 2OC on the left side: --------------------------  */
//#define NORTHERNLIGHT_2OC_LEFTSIDE
/* ------------ print debug messages to USB serial --------------------------------------------------  */
//#define PRINT_DEBUG
/* ------------ flip screen / IO mapping ------------------------------------------------------------  */
//#define FLIP_180
/* ------------ invert screen pixels ----------------------------------------------------------------  */
//#define INVERT_DISPLAY
/* ------------ use DAC8564 -------------------------------------------------------------------------  */
//#define DAC8564

/* Flags for the full-width apps, these enable/disable them in OC_apps.ino but also zero out the app   */
/* files to prevent them from taking up space. Only Enigma is enabled by default.                      */
#define ENABLE_APP_ENIGMA
#define ENABLE_APP_MIDI
#define ENABLE_APP_NEURAL_NETWORK
#define ENABLE_APP_PONG
#define ENABLE_APP_DARKEST_TIMELINE


#endif

