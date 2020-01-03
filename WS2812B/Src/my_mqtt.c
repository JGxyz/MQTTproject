#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "my_diodes.h"
#include "dbgu.h"
#include "term_io.h"
#include "my_mqtt.h"
#include "ansi.h"
#include "lwip/api.h"
#include <string.h>

extern struct netif gnetif;
int inpub_id;

void displayOwnIp()
{

	xprintf(
		"My IP: %d.%d.%d.%d\n",
		ip4_addr1_16(netif_ip4_addr(&gnetif)),
		ip4_addr2_16(netif_ip4_addr(&gnetif)),
		ip4_addr3_16(netif_ip4_addr(&gnetif)),
		ip4_addr4_16(netif_ip4_addr(&gnetif))
		);
}

void handle_dhcp() {
	  xprintf("Obtaining address with DHCP...\n");

	  struct dhcp *dhcp = netif_dhcp_data(&gnetif);
	    do
	    {
	      xprintf("dhcp->state = %02X\n",dhcp->state);
	      vTaskDelay(250);
	    }while(dhcp->state != 0x0A);

	    xprintf("DHCP bound\n");
	    displayOwnIp();
}

void handle_diode(char * data){
	all_color(parse_message(data));
}

void handle_blink(char *data){
	blink(parse_message(data));
}

void handle_change(char *data){
	change(parse_message(data));
}

void mqtt_pub_request_cb(void *arg, err_t result)
{
  if(result != ERR_OK) {
    printf("Publish result: %d\n", result);
  }
}

void example_publish(mqtt_client_t *client, void *arg)
{
  const char *pub_payload= "PubSubHubLubJub";
  err_t err;
  u8_t qos = 2; /* 0 1 or 2, see MQTT specification */
  u8_t retain = 0; /* No don't retain such crappy payload... */
  err = mqtt_publish(client, "pub_topic", pub_payload, strlen(pub_payload), qos, retain, mqtt_pub_request_cb, arg);
  if(err != ERR_OK) {
    printf("Publish err: %d\n", err);
  }
}


int parse_message(char * data) {
	if (strcmp(data, "red") == 0){
		return RED;
	} else if (strcmp(data, "blue") == 0){
		return BLUE;
	} else if (strcmp(data, "green") == 0){
		return GREEN;
	} else if (strcmp(data, "white") == 0){
		return WHITE;
	} else if (strcmp(data, "cyan") == 0){
		return CYAN;
	} else if (strcmp(data, "yellow") == 0){
		return YELLOW;
	} else if (strcmp(data, "magenta") == 0){
		return MAGENTA;
	} else if (strcmp(data, "all") == 0){
		return ALL;
	} else if (strcmp(data, "mixed") == 0){
		return MIXED;
	} else if (strcmp(data, "basic") == 0){
		return BASIC;
	}
	return BLACK;
}

int parse_topic(char *data) {
	if (strcmp(data, "color") == 0){
		return COLOR;
	} else if (strcmp(data, "blink") == 0){
		return BLINK;
	} else if (strcmp(data, "change") == 0){
		return CHANGE;
	}
	return NONE;
}

char* get_data(const u8_t *data, u16_t len)
{
	char * new_data = (char*)malloc(len+1 * sizeof(char));
	strncpy(new_data, (const char*)data, len);
	new_data[len] = '\0';
	return new_data;
}

void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
  //printf("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);

  if(flags & MQTT_DATA_FLAG_LAST) {
    /* Last fragment of payload received (or whole part if payload fits receive buffer
       See MQTT_VAR_HEADER_BUFFER_LEN)  */

    /* Call function or do action depending on reference, in this case inpub_id */
    /*if(inpub_id == 0) {
      //Don't trust the publisher, check zero termination
      if(data[len-1] == 0) {
        printf("mqtt_incoming_data_cb: %s\n", (const char *)data);
      }
    }*/
	if(inpub_id == COLOR) {
    	handle_diode(get_data(data, len));
    } else if(inpub_id == BLINK) {
    	handle_blink(get_data(data, len));
    } else if(inpub_id == CHANGE) {
    	handle_change(get_data(data, len));
    }
    else {
      printf("mqtt_incoming_data_cb: Ignoring payload...\n");
    }
  } else {
    /* Handle fragmented payload, store in buffer, write to file or whatever */
  }
}

void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
  //printf("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);
  /* Decode topic string into a user defined reference */
  /*if(strcmp(topic, "print_payload") == 0) {
    inpub_id = 0;
  } */
  if (strcmp(topic, "color") == 0) {
    /* Turn on/off diode */
	 inpub_id = COLOR;
  } else if (strcmp(topic, "blink") == 0){
	  inpub_id = BLINK;
  } else if (strcmp(topic, "change") == 0){
	  inpub_id = CHANGE;
  }
}

void mqtt_sub_request_cb(void *arg, err_t result)
{
  /* Just print the result code here for simplicity,
     normal behaviour would be to take some action if subscribe fails like
     notifying user, retry subscribe or disconnect from server */
  printf("Subscribe result: %d\n", result);
}


void example_do_connect(mqtt_client_t *client)
{

  struct mqtt_connect_client_info_t ci;
  err_t err;

  /* Setup an empty client info structure */
  memset(&ci, 0, sizeof(ci));

  /* Minimal amount of information required is client identifier, so set it here */
   ci.client_id = "lwip_test";
   ip_addr_t ip;
   IP4_ADDR(&ip, BROKER_IP1, BROKER_IP2, BROKER_IP3, BROKER_IP4);

  /* Initiate client and connect to server, if this fails immediately an error code is returned
     otherwise mqtt_connection_cb will be called with connection result after attempting
     to establish a connection with the server.
     For now MQTT version 3.1.1 is always used */

  err = mqtt_client_connect(client, &ip, MQTT_PORT, mqtt_connection_cb, 0, &ci);

  /* For now just print the result code if something goes wrong */
  if(err != ERR_OK) {
    printf("mqtt_connect return %d\n", err);
  }
}

void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  err_t err;
  if(status == MQTT_CONNECT_ACCEPTED) {
    printf("mqtt_connection_cb: Successfully connected\n");

    /* Setup callback for incoming publish requests */
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);

    /* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */
    err = mqtt_subscribe(client, "color", 1, mqtt_sub_request_cb, arg);
    err = mqtt_subscribe(client, "blink", 1, mqtt_sub_request_cb, arg);
    err = mqtt_subscribe(client, "change", 1, mqtt_sub_request_cb, arg);

    if(err != ERR_OK) {
      printf("mqtt_subscribe return: %d\n", err);
    }
  } else {
    printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);

    /* Its more nice to be connected, so try to reconnect */
    example_do_connect(client);
  }
}
