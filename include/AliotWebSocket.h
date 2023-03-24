#ifndef ALIOT_WEBSOCKET_H
#define ALIOT_WEBSOCKET_H

#include <WebSocketsClient.h>
#include <Arduino.h>


typedef enum {
    // Connection events
    AWSEvent_CONNECT_WATCHER,
    AWSEvent_CONNECT_OBJECT,
    AWSEvent_CONNECT_PROJECT,
    AWSEvent_DISCONNECT_PROJECT,
    AWSEvent_CONNECT_SUCCESS,
    AWSEvent_PING,
    AWSEvent_PONG,

    // Document events
    AWSEvent_UPDATE_DOC,
    AWSEvent_RECEIVE_DOC,
    AWSEvent_SUBSCRIBE_LISTENER,
    AWSEvent_UNSUBSCRIBE_LISTENER,
    AWSEvent_SUBSCRIBE_LISTENER_SUCCESS,
    AWSEvent_UNSUBSCRIBE_LISTENER_SUCCESS,
    AWSEvent_RECEIVE_LISTEN,

    // Broadcast events
    AWSEvent_SEND_BROADCAST,
    AWSEvent_RECEIVE_BROADCAST,

    // Error events
    AWSEvent_ERROR,

    // Action events
    AWSEvent_SEND_ACTION,
    AWSEvent_RECEIVE_ACTION,
    AWSEvent_SEND_ACTION_DONE,
    AWSEvent_RECEIVE_ACTION_DONE,
    AWSEvent_SEND_ROUTE,
    AWSEvent_UPDATE_INTERFACE,
    AWSEvent_RECEIVE_INTERFACE,

    // Http events
    AWSEvent_GET_DOC,
    AWSEvent_GET_FIELD,

} AWSEvent_t;


class AliotWebSocket {
    public:
        WebSocketsClient m_AWSClient;
        AliotWebSocket();
        ~AliotWebSocket();

        void run();
        static void* callback(WStype_t type, uint8_t * payload, size_t length);

        void event();
    



};

#endif /*ALIOT_WEBSOCKET_H*/