#pragma once

/**
 * Use this file for configuration of the Aliot library.
 * Allows for more flexibility in testing different methods of implementation.
*/

// Comment out to disable debug output
#define ALIOT_DEBUG

// #define ALIOT_EVENTS_MACROS

// Comment out to use the static variables instead of macros
// #ifdef ALIOT_EVENTS_MACROS
//     #define EVT_CONNECT_SUCCESS "connect_success"
//     #define EVT_CONNECT_OBJECT "connect_object"
//     #define EVT_ERROR "error"
//     #define EVT_PING "ping"
//     #define EVT_PONG "pong"
//     #define EVT_UPDATE_DOC "update_doc"
// #endif /* ALIOT_EVENTS_MACROS */
