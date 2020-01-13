#ifndef MY_MQTT_H_
#define MY_MQTT_H_

#define BROKER_IP1 192
#define BROKER_IP2 168
#define BROKER_IP3 1
#define BROKER_IP4 243
#define CLIENT_ID "123.456.789"

void handle_dhcp();
void displayOwnIp();
void mqtt_connect(mqtt_client_t *client);
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
void mqtt_sub_request_cb(void *arg, err_t result);
void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
char* get_data(const u8_t *data, u16_t len);
int parse_message(char*);
void handle_color(char*);
void handle_blink(char*);
void handle_change(char*);
void handle_dots(char*);

#endif
