#ifndef MY_MQTT_H_
#define MY_MQTT_H_

#define BROKER_IP1 192
#define BROKER_IP2 168
#define BROKER_IP3 1
#define BROKER_IP4 243


void turn_led_on(int);
void turn_led_off(int);
void toggle_led(int, int);
void handle_diode(char*);
void example_do_connect(mqtt_client_t *client);
void mqtt_sub_request_cb(void *arg, err_t result);
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
void handle_diode(char*);
void handel_blink();
int parse_message(char*);


#endif
