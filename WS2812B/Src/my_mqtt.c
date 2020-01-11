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

void mqtt_connect(mqtt_client_t *client)
{

  struct mqtt_connect_client_info_t ci;

  /* Setup an empty client info structure */
  memset(&ci, 0, sizeof(ci));

  /* Minimal amount of information required is client identifier, set client_id as it IP*/
  ci.client_id = CLIENT_ID;
  ip_addr_t ip;
  IP4_ADDR(&ip, BROKER_IP1, BROKER_IP2, BROKER_IP3, BROKER_IP4);

  mqtt_client_connect(client, &ip, MQTT_PORT, mqtt_connection_cb, 0, &ci);
}

void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  if(status == MQTT_CONNECT_ACCEPTED) {
    printf("Successfully connected to MQTT broker %d.%d.%d.%d on port %d\n", BROKER_IP1, BROKER_IP2, BROKER_IP3, BROKER_IP4, MQTT_PORT);

    // Setup callback for incoming publish requests
    mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);

    // Subscribe color
    mqtt_subscribe(client, "color", 1, mqtt_sub_request_cb, "color");

    // Subscribe blink
    mqtt_subscribe(client, "blink", 1, mqtt_sub_request_cb, "blink");

    // Subscribe change
    mqtt_subscribe(client, "change", 1, mqtt_sub_request_cb, "change");

    // Subscribe dots
    mqtt_subscribe(client, "dots", 1, mqtt_sub_request_cb, "dots");

  } else {
	printf("Disconnected from MQTT broker %d.%d.%d.%d on port %d, reason: %d\n", BROKER_IP1, BROKER_IP2, BROKER_IP3, BROKER_IP4, MQTT_PORT, status);
    printf("Trying to reconnect\n");
    mqtt_connect(client);
  }
}

void mqtt_sub_request_cb(void *arg, err_t result)
{
  char *topic = (char*)arg ;
  if(result == ERR_OK) {
	  	  printf("Successfully subscribed on topic `%s`\n", topic);
     }if(result == ERR_TIMEOUT){
    	 printf("Subscribe on topic `%s` failed, no response was received within timeout\n", topic);
     }else if(result == ERR_ABRT){
    	 printf("Subscribe on topic `%s` failed, request denied\n", topic);
     }
}

void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
  if(flags & MQTT_DATA_FLAG_LAST) {
	char* parsed_data = get_data(data, len);
	printf("Received data `%s`\n", parsed_data);
	if(inpub_id == COLOR) {
    	handle_diode(parsed_data);
    } else if(inpub_id == BLINK) {
    	handle_blink(parsed_data);
    } else if(inpub_id == CHANGE) {
    	handle_change(parsed_data);
    } else if(inpub_id == DOTS) {
    	handle_dots(parsed_data);
    }
    else {printf("Not supported topic, ignoring payload\n");}
  } else {}
}

void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
	printf("Message on topic `%s` incoming\n", topic);
  if (strcmp(topic, "color") == 0) {
	 inpub_id = COLOR;
  } else if (strcmp(topic, "blink") == 0){
	  inpub_id = BLINK;
  } else if (strcmp(topic, "change") == 0){
	  inpub_id = CHANGE;
  } else if (strcmp(topic, "dots") == 0){
	  inpub_id = DOTS;
  }
}

char* get_data(const u8_t *data, u16_t len)
{
	char * new_data = (char*)malloc(len+1 * sizeof(char));
	strncpy(new_data, (const char*)data, len);
	new_data[len] = '\0';
	return new_data;
}

int parse_topic(char *data){
	if (strcmp(data, "color") == 0){
		return COLOR;
	} else if (strcmp(data, "blink") == 0){
		return BLINK;
	} else if (strcmp(data, "change") == 0){
		return CHANGE;
	} else if (strcmp(data, "dots") == 0){
		return DOTS;
	}
	return NONE;
}

int parse_message(char *data) {
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

void handle_diode(char *data){
	all_color(parse_message(data));
}

void handle_blink(char *data){
	blink(parse_message(data));
}

void handle_change(char *data){
	change(parse_message(data));
}

void handle_dots(char *data){
	three_dots_running(parse_message(data));
}
