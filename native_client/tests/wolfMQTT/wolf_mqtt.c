//#include <sys/types.h>
//#include <sys/socket.h>
#include "native_client/src/public/socket_syscalls.h"
#include "native_client/src/public/socket_types.h"
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "native_client/src/tests/wolfMQTT/mqtt_types.h"
#include "native_client/src/tests/wolfMQTT/mqtt_socket.h"
#include "native_client/src/tests/wolfMQTT/mqtt_packet.h"
#include "native_client/src/tests/wolfMQTT/mqtt_client.h"

#define MQTT_HOST               "127.0.0.1"
#define MQTT_QOS                MQTT_QOS_0
#define MQTT_KEEP_ALIVE_SEC     60
#define MQTT_CMD_TIMEOUT_MS     30000
#define MQTT_CON_TIMEOUT_MS     5000
#define MQTT_CLIENT_ID          "wolfMQTTClient"
#define MQTT_TOPIC_NAME         "wolfMQTT/test/ECG"
#define MQTT_PUBLISH_MSG        "Test Publish"
#define MQTT_USERNAME           NULL
#define MQTT_PASSWORD           NULL
#ifdef ENABLE_MQTT_TLS    
    #define MQTT_USE_TLS        1
    #define MQTT_PORT           8883
#else
    #define MQTT_USE_TLS        0
    #define MQTT_PORT           1883
#endif

#define INVALID_SOCKET_FD         -1
#define MQTT_MAX_PACKET_SZ      1024
#define PRINT_BUFFER_SIZE       80



/* Local Functions */

/* msg_new on first data callback */
/* msg_done on last data callback */
/* msg->total_len: Payload total length */
/* msg->buffer: Payload buffer */
/* msg->buffer_len: Payload buffer length */
/* msg->buffer_pos: Payload buffer position */

static int Qwaves[10] = {0};
static int idx = 0;

static void updateQwave(char *qwave_str)
{
    int q = atoi(qwave_str);
    Qwaves[idx++] = q;
    if (idx >= 10)
      idx = 0;
    int sum = 0;
    printf("Qwaves: [");
    for (int i = 0; i < 10; i++){
        printf("%d, ", Qwaves[i]);
        sum += Qwaves[i];
    }
    
    printf("], sum= %d, avg = %d, curr = %d, is_anomaly? %d\n", sum, sum / 10, q, (sum / 10 - q > 30 || sum / 10 - q < -30) ? 1 : 0);
}

static int mqtt_message_cb(MqttClient *client, MqttMessage *msg,
    byte msg_new, byte msg_done)
{
    byte buf[PRINT_BUFFER_SIZE+1];
    word32 len;

    (void)client;

    if (msg_new) {
        /* Determine min size to dump */
        len = msg->topic_name_len;
        if (len > PRINT_BUFFER_SIZE) {
            len = PRINT_BUFFER_SIZE;
        }
        XMEMCPY(buf, msg->topic_name, len);
        buf[len] = '\0'; /* Make sure its null terminated */

        /* Print incoming message */
        PRINTF("MQTT Message: Topic %s, Qos %d, Len %u",
            buf, msg->qos, msg->total_len);
    }

    /* Print message payload */
    len = msg->buffer_len;
    if (len > PRINT_BUFFER_SIZE) {
        len = PRINT_BUFFER_SIZE;
    }
    XMEMCPY(buf, msg->buffer, len);
    buf[len] = '\0'; /* Make sure its null terminated */
    PRINTF("Payload (%d - %d): %s",
        msg->buffer_pos, msg->buffer_pos + len, buf);

    if (msg_done) {
        updateQwave((char *)buf);
        PRINTF("MQTT Message: Done");
    }

    /* Return negative to terminate publish processing */
    return MQTT_CODE_SUCCESS;
}

static void setup_timeout(struct timeval* tv, int timeout_ms)
{
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms % 1000) * 1000;

    /* Make sure there is a minimum value specified */
    if (tv->tv_sec < 0 || (tv->tv_sec == 0 && tv->tv_usec <= 0)) {
        tv->tv_sec = 0;
        tv->tv_usec = 100;
    }
}

/*
static int socket_get_error(int sockFd)
{
    int so_error = 0;
    int len = sizeof(so_error);
    getsockopt(sockFd, SOL_SOCKET, SO_ERROR, &so_error, &len);
    return so_error;
}
*/

static int mqtt_net_connect(void *context, const char* host, word16 port,
    int timeout_ms)
{
    int rc;
    int sockFd, *pSockFd = (int*)context;
    struct sockaddr_in addr;
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    if (pSockFd == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    (void)timeout_ms;

    /* get address */
    XMEMSET(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    XMEMSET(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    rc = getaddrinfo(host, NULL, &hints, &result);
    if (rc >= 0 && result != NULL) {
        struct addrinfo* res = result;

        /* prefer ip4 addresses */
        while (res) {
            if (res->ai_family == AF_INET) {
                result = res;
                break;
            }
            res = res->ai_next;
        }
        if (result->ai_family == AF_INET) {
            addr.sin_port = htons(port);
            addr.sin_family = AF_INET;
            addr.sin_addr =
                ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
        }
        else {
            rc = -1;
        }
        freeaddrinfo(result);
    }
    if (rc < 0) {
        return MQTT_CODE_ERROR_NETWORK;
    }

    sockFd = socket(addr.sin_family, SOCK_STREAM, 0);
    if (sockFd < 0) {
        return MQTT_CODE_ERROR_NETWORK;
    }

    /* Start connect */
    rc = connect(sockFd, (struct sockaddr*)&addr, sizeof(addr));
    if (rc < 0) {
        /*  
        PRINTF("NetConnect: Error %d (Sock Err %d)",
            rc, socket_get_error(*pSockFd));
        */
        close(sockFd);
        return MQTT_CODE_ERROR_NETWORK;
    }

    /* save socket number to context */
    *pSockFd = sockFd;

    return MQTT_CODE_SUCCESS;
}

static int mqtt_net_read(void *context, byte* buf, int buf_len, int timeout_ms)
{
    int rc;
    int *pSockFd = (int*)context;
    int bytes = 0;
    struct timeval tv;

    if (pSockFd == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    /* Setup timeout */
    setup_timeout(&tv, timeout_ms);
    setsockopt(*pSockFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));

    /* Loop until buf_len has been read, error or timeout */
    while (bytes < buf_len) {
        rc = (int)recv(*pSockFd, &buf[bytes], buf_len - bytes, 0);
        if (rc < 0) {
            //rc = socket_get_error(*pSockFd);
            //if (rc == 0)
            //    break; /* timeout */
            PRINTF("NetRead: Error %d", rc);
            return MQTT_CODE_ERROR_NETWORK;
        }
        bytes += rc; /* Data */
    }

    if (bytes == 0) {
        return MQTT_CODE_ERROR_TIMEOUT;
    }

    return bytes;
}

static int mqtt_net_write(void *context, const byte* buf, int buf_len,
    int timeout_ms)
{
    int rc;
    int *pSockFd = (int*)context;
    struct timeval tv;

    if (pSockFd == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    /* Setup timeout */
    setup_timeout(&tv, timeout_ms);
    setsockopt(*pSockFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));

    rc = (int)send(*pSockFd, buf, buf_len, 0);
    if (rc < 0) {
        /*
        PRINTF("NetWrite: Error %d (Sock Err %d)",
            rc, socket_get_error(*pSockFd));
        */
        return MQTT_CODE_ERROR_NETWORK;
    }

    return rc;
}

static int mqtt_net_disconnect(void *context)
{
    int *pSockFd = (int*)context;

    if (pSockFd == NULL) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    close(*pSockFd);
    *pSockFd = INVALID_SOCKET_FD;

    return MQTT_CODE_SUCCESS;
}

#ifdef ENABLE_MQTT_TLS
static int mqtt_tls_verify_cb(int preverify, WOLFSSL_X509_STORE_CTX* store)
{
    char buffer[WOLFSSL_MAX_ERROR_SZ];
    PRINTF("MQTT TLS Verify Callback: PreVerify %d, Error %d (%s)",
        preverify, store->error, store->error != 0 ?
            wolfSSL_ERR_error_string(store->error, buffer) : "none");
    PRINTF("  Subject's domain name is %s", store->domain);

    if (store->error != 0) {
        /* Allowing to continue */
        /* Should check certificate and return 0 if not okay */
        PRINTF("  Allowing cert anyways");
    }

    return 1;
}

/* Use this callback to setup TLS certificates and verify callbacks */
static int mqtt_tls_cb(MqttClient* client)
{
    int rc = WOLFSSL_FAILURE;

    client->tls.ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (client->tls.ctx) {
        wolfSSL_CTX_set_verify(client->tls.ctx, WOLFSSL_VERIFY_PEER,
                               mqtt_tls_verify_cb);

		/* default to success */
        rc = WOLFSSL_SUCCESS;

	#if !defined(NO_CERT)
    #if 0
    	/* Load CA certificate buffer */
        rc = wolfSSL_CTX_load_verify_buffer(client->tls.ctx, caCertBuf,
                                          caCertSize, WOLFSSL_FILETYPE_PEM);
    #endif
    #if 0
        /* If using a client certificate it can be loaded using: */
        rc = wolfSSL_CTX_use_certificate_buffer(client->tls.ctx,
                           clientCertBuf, clientCertSize, WOLFSSL_FILETYPE_PEM);
    #endif
    #endif /* !NO_CERT */
    }

    PRINTF("MQTT TLS Setup (%d)", rc);

    return rc;
}
#else
static int mqtt_tls_cb(MqttClient* client)
{
    (void)client;
    return 0;
}
#endif /* ENABLE_MQTT_TLS */

static word16 mqtt_get_packetid(volatile word16 *mPacketIdLast)
{
    /* Check rollover */
    if (*mPacketIdLast >= MAX_PACKET_ID) {
        *mPacketIdLast = 0;
    }

    return ++(*mPacketIdLast);
}


int mqtt_test(void)
{
    int ret;
    MqttObject mqtt_obj;
    MqttTopic topics[1];
    MqttClient m_client;
    MqttNet m_network;
    int m_sockfd = INVALID_SOCKET_FD;
    byte m_send_buf[MQTT_MAX_PACKET_SZ];
    byte m_read_buf[MQTT_MAX_PACKET_SZ];
    volatile word16 m_packet_id_last;

    // mqtt init
    memset((void *)&m_network, 0, sizeof(m_network));

    m_network.connect = mqtt_net_connect;
    m_network.read = mqtt_net_read;
    m_network.write = mqtt_net_write;
    m_network.disconnect = mqtt_net_disconnect;
    m_network.context = &m_sockfd;

    ret = MqttClient_Init(  &m_client, &m_network, mqtt_message_cb, 
                            m_send_buf, sizeof(m_send_buf), 
                            m_read_buf, sizeof(m_read_buf), 
                            MQTT_CON_TIMEOUT_MS);
    if (ret != MQTT_CODE_SUCCESS)
        goto exit;

    printf("MQTT Init Success\n");

    // connect to broker
    ret = MqttClient_NetConnect(
            &m_client, MQTT_HOST, MQTT_PORT, MQTT_CON_TIMEOUT_MS, 
            MQTT_USE_TLS, mqtt_tls_cb
        );

    if (ret != MQTT_CODE_SUCCESS)
        goto exit;

    
    printf("MQTT Network Connect Success: Host %s, Port %d, UseTLS %d", MQTT_HOST, MQTT_PORT, MQTT_USE_TLS);

    // send connect and wait for ack
    memset(&mqtt_obj, 0, sizeof(mqtt_obj));
    mqtt_obj.connect.keep_alive_sec = MQTT_KEEP_ALIVE_SEC;
    mqtt_obj.connect.client_id = MQTT_CLIENT_ID;
    mqtt_obj.connect.username = MQTT_USERNAME;
    mqtt_obj.connect.password = MQTT_PASSWORD;

    ret = MqttClient_Connect(&m_client, &mqtt_obj.connect);

    if (ret != MQTT_CODE_SUCCESS)
        goto exit;

    printf("MQTT Broker Connect Success: ClientID %s, Username %s, Password %s", 
            MQTT_CLIENT_ID, 
            (MQTT_USERNAME == NULL) ? "NULL":MQTT_USERNAME,
            (MQTT_PASSWORD == NULL) ? "NULL":MQTT_PASSWORD
        );
    // subscribe and wait for ack
    memset(&mqtt_obj, 0, sizeof(mqtt_obj));
    topics[0].topic_filter = MQTT_TOPIC_NAME;
    topics[0].qos = MQTT_QOS;
    mqtt_obj.subscribe.packet_id = mqtt_get_packetid(&m_packet_id_last);
    mqtt_obj.subscribe.topic_count = sizeof(topics) / sizeof(MqttTopic);
    mqtt_obj.subscribe.topics = topics;

    ret = MqttClient_Subscribe(&m_client, &mqtt_obj.subscribe);

    if (ret != MQTT_CODE_SUCCESS)
        goto exit;
   
    printf("MQTT Subscribe Success: Topic %s, QoS %d\n",
                MQTT_TOPIC_NAME, MQTT_QOS);
    
    // publish
    memset(&mqtt_obj, 0, sizeof(mqtt_obj));
    mqtt_obj.publish.qos = MQTT_QOS;
    mqtt_obj.publish.topic_name = MQTT_TOPIC_NAME;
    mqtt_obj.publish.packet_id = mqtt_get_packetid(&m_packet_id_last);
    mqtt_obj.publish.buffer = (byte*)MQTT_PUBLISH_MSG;
    mqtt_obj.publish.total_len = XSTRLEN(MQTT_PUBLISH_MSG);
    ret = MqttClient_Publish(&m_client, &mqtt_obj.publish);
    if (ret != MQTT_CODE_SUCCESS)
        goto exit;
    

    printf("MQTT Publish: Topic %s, Qos %d, Message %s\n",
        mqtt_obj.publish.topic_name, mqtt_obj.publish.qos, mqtt_obj.publish.buffer); 
	
	/* Wait for messages */
    while (1) {
        ret = MqttClient_WaitMessage_ex(&m_client, &mqtt_obj, MQTT_CMD_TIMEOUT_MS);
        if (ret == MQTT_CODE_ERROR_TIMEOUT) {
            /* send keep-alive ping */
            ret = MqttClient_Ping_ex(&m_client, &mqtt_obj.ping);
            if (ret != MQTT_CODE_SUCCESS) {
                break;
            }
            printf("MQTT Keep-Alive Ping\n");
        }
        else if (ret != MQTT_CODE_SUCCESS) {
            break;
        }
    }

exit:
    if (ret != MQTT_CODE_SUCCESS)
    {
        printf("MQTT error %d: %s\n", ret, MqttClient_ReturnCodeToString(ret));
    }
    return ret;
}

int main(void)
{
    int ret;
    ret = mqtt_test();
    return ret;
}
