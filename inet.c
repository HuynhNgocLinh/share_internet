/*!
 *  @file		    inet.c
 *  @author  	  Linh Huynh
 *	@copyright	
 *  @version 	  1.00.00.20190420
 *  @date    	  April 20 , 2019
 *  @note
 */
/*******************************************************************************
 *  INCLUDES
 ******************************************************************************/
#include "inet.h"
#include "cbuffer.h"
/*******************************************************************************
 * MACRO
 ******************************************************************************/
/*******************************************************************************
 * VARIABLE
 ******************************************************************************/
static CBuffer_T cbuffer_handle;
static uint8_t cbuffer_data[1024];
/*******************************************************************************
 * TYPEDEF 
 ******************************************************************************/
/*******************************************************************************
 * ENUM
 ******************************************************************************/
enum 
{
  INET_GET_INFO = 0          ,
  INET_ESTABLISH_CONNECTION  ,
  INET_SEND_DATA             ,
  INET_RECEIVE_DATA          ,
  INET_CLOSE_CONNECTION      , 
  INET_MAX
};

enum 
{
  INET_TYPE_TCP              ,
  INET_TYPE_UDP              ,
  INET_TYPE_MAX
};

enum {
  INET_REP_OK                ,
  INET_REP_ERROR             ,
	INET_REP_CONNECT_OK        ,
  INET_REP_SEND_OK           ,
  INET_REP_MAX
};

enum  
{
  INET_STATUS_IDX  = 0       ,
  INET_IP_ADDR_IDX = 1       ,
  INET_TYPE_IDX    = 16      ,
  INET_PORT_IDX    = 19      
}
/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define INET_TX_LEN  (128)
#define INET_RX_LEN  (128)

const char *const inet_at_command[INET_MAX] = {
  "AT+CIPSTATUS"        
  "AT+CIPSTART="
  "AT+CIPSEND"
  "AT+CIRD"
  "AT+CIPCLOSE"
};

const char *const inet_type[INET_TYPE_MAX] = {
  "TCP,",														//< 0. The TCP/IP protocol 
  "UDP,"														//< 1. The UDP protocol
};

const char *const inet_rep[SIM868_REP_MAX] = {
	"\r\nOK\r\n",											//< 0. The reponse is ok
	"\r\nERROR\r\n",									//< 1. the reponse is error
	"\r\nCONNECT OK\r\n",							//< 2. Connect server ok
	"\r\nSEND OK\r\n",								//< 4. Send data ok
};
/*******************************************************************************
 * FUNCTIONS - Callback
 ******************************************************************************/
void inet_receive_callback_fnx(uint8_t *buf, uint32_t len)
{
  CBuffer_Write(&cbuffer_handle, buf, len);
}
/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/
inet_status_t inet_transfer(uint8_t *tx_buf, uint16_t tx_len, uint8_t *rx_buf, uint32_t rx_len);
/*******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
inet_status_t inet_init()
{
  // Init CDC
  MX_USB_DEVICE_Init();

  // Init cbuffer
  CBuffer_Init(&cbuffer_handle, cbuffer_data, 1024);
}

/******************************************************************************/
inet_status_t inet_get_info_connection(inet_info_connection_t *info_connection)
{
  char tx_buf[INET_TX_LEN];
  char rx_buf[22];

  uint8_t tx_len = strlen(inet_at_command[INET_GET_INFO]);

  // Copy command
  memcpy(tx_buf, inet_at_command[INET_GET_INFO], tx_len);

  // Send command 
  inet_transfer((uint8_t *)tx_buf, strlen(tx_buf), rx_buf, sizeof(rx_buf), 1000);

  // Check response
  info_connection->status  = rx_buf[INET_STATUS_IDX] - 48;
  info_connection->ip_addr = rx_buf[INET_IP_ADDR_IDX];
  info_connection->type    = rx_buf[INET_TYPE_IDX];
}

/******************************************************************************/
inet_status_t inet_establish_tcp_connection(inet_t         *inet, 
                                            inet_ip_addr_t *ip_addr,
                                            inet_port_t     port);
{
  char tx_buf[INET_TX_LEN];
  char rx_buf[INET_RX_LEN];
  char space_buf[] = ",";

  uint8_t tx_len = strlen(inet_at_command[INET_ESTABLISH_CONNECTION]);
  uint8_t rx_len = strlen(inet_rep[INET_REP_OK]);


  // Copy command
  memcpy(tx_buf, inet_at_command[INET_ESTABLISH_CONNECTION], tx_len);
  // Copy type
  tx_len = strlen(inet_type[INET_TYPE_TCP]);
  memcpy(&tx_buf[strlen(tx_buf)], inet_type[INET_TYPE_TCP], tx_len);
  // Copy string ","
  memcpy(&tx_buf[strlen(tx_buf)], space_buf, strlen(space_buf));
  // Copy ip address
  memcpy(&tx_buf[strlen(tx_buf)], ip_addr, strlen(ip_addr));
  // Copy string ","
  memcpy(&tx_buf[strlen(tx_buf)], space_buf, strlen(space_buf));
  // Copy port
  sprintf(&tx_buf[strlen(tx_buf)], "%d", port);

  // Send command 
  inet_transfer((uint8_t *)tx_buf, strlen(tx_buf), rx_buf, rx_len, 1000);
  
  // Check response
  if (strstr(rx_buf, inet_rep[INET_REP_OK]) != NULL && 
      strstr(rx_buf, inet_rep[INET_REP_CONNECT_OK]) != NULL)
    return INET_STATUS_OK;
  else 
    return INET_STATUS_ERR_OPTION;


}

/******************************************************************************/
inet_status_t inet_establish_udp_connection(inet_t         *inet, 
                                            inet_ip_addr_t *ip_addr,
                                            inet_port_t     port)
{
  char tx_buf[INET_TX_LEN];
  char rx_buf[INET_RX_LEN];
  char space_buf[] = ",";

  uint8_t tx_len = strlen(inet_at_command[INET_ESTABLISH_CONNECTION]);
  uint8_t rx_len = strlen(inet_rep[INET_REP_OK]);

  // Copy command
  memcpy(tx_buf, inet_at_command[INET_ESTABLISH_CONNECTION], tx_len);
  // Copy type
  tx_len = strlen(inet_type[INET_TYPE_UDP]);
  memcpy(&tx_buf[strlen(tx_buf)], inet_type[INET_TYPE_UDP], tx_len);
  // Copy string ","
  memcpy(&tx_buf[strlen(tx_buf)], space_buf, strlen(space_buf));
  // Copy ip address
  memcpy(&tx_buf[strlen(tx_buf)], ip_addr, strlen(ip_addr));
  // Copy string ","
  memcpy(&tx_buf[strlen(tx_buf)], space_buf, strlen(space_buf));
  // Copy port
  sprintf(&tx_buf[strlen(tx_buf)], "%d", port);

  // Send command 
  inet_transfer((uint8_t *)tx_buf, strlen(tx_buf), rx_buf, rx_len, 1000);
  
  // Check response
  if (strstr(rx_buf, inet_rep[INET_REP_OK]) != NULL && 
      strstr(rx_buf, inet_rep[INET_REP_CONNECT_OK]) != NULL)
    return INET_STATUS_OK;
  else 
    return INET_STATUS_ERR_OPTION;
}                                            

inet_status_t inet_send_data( inet_data_t *data, inet_length_t len)
{
  char tx_buf[INET_TX_LEN];
  char rx_buf[INET_RX_LEN];
  char enter_buf[] = "\n>\n";
  
  uint8_t tx_len = strlen(inet_at_command[INET_SEND_DATA]);
  uint8_t rx_len = strlen(inet_rep[INET_REP_SEND_OK]);

  // Copy command
  memcpy(tx_buf, inet_at_command[INET_SEND_DATA], txLen);  
  // Copy enter char
  memcpy(tx_buf[strlen(tx_buf)], enter_buf, strlen(enter_buf));
  // Copy data
  memcpy(tx_buf[strlen(tx_buf)], data, len);

  // Send command 
  inet_transfer((uint8_t *)tx_buf, strlen(tx_buf), rx_buf, rx_len, 1000);

  if (strstr(rx_buf, inet_rep[INET_REP_SEND_OK]) != NULL)
    return INET_STATUS_OK;
  else 
    return INET_STATUS_ERR_OPTION;
}

inet_status_t inet_receive_data(inet_data_t *data)
{
  char tx_buf[INET_TX_LEN];
  char rx_buf[1024];

  uint8_t tx_len = strlen(inet_at_command[INET_RECEIVE_DATA]);
  
  // Copy command
  memcpy(tx_buf, inet_at_command[INET_RECEIVE_DATA], tx_len);

  // Send command 
  inet_transfer((uint8_t *)tx_buf, strlen(tx_buf), rx_buf, rx_len, 1000);

  if (strstr(rx_buf, "<\n") != NULL)
  {
    data = &rx_buf[1];
  }
  else 
  {
    return INET_STATUS_ERR_OPTION;
  }

  return INET_STATUS_OK;
}

inet_status_t inet_close_connection()
{
  char tx_buf[INET_TX_LEN];
  char rx_buf[INET_RX_LEN];

  uint8_t tx_len = strlen(inet_at_command[INET_CLOSE_CONNECTION]);
  uint8_t rx_len = strlen(inet_rep[INET_REP_OK]);


  // Copy command
  memcpy(tx_buf, inet_at_command[INET_CLOSE_CONNECTION], tx_len);

  // Send command 
  inet_transfer((uint8_t *)tx_buf, strlen(tx_buf), rx_buf, rx_len, 1000);
 
  if (strstr(rx_buf, inet_rep[INET_REP_OK]) != NULL)
    return INET_STATUS_OK;
  else 
    return INET_STATUS_ERR_OPTION;
}

inet_status_t inet_transfer(uint8_t *tx_buf, 
                            uint16_t tx_len, 
                            uint8_t *rx_buf, 
                            uint32_t rx_len
                            uint32_t timeout)
{
  if (tx_buf)
  {
    CDC_Transmit_FS(tx_buf, tx_len);
  }
  else 
  {
    return INET_STATUS_ERR_NULL_PTR;
  }

  if (rx_buf)
  {
    uint16_t t_len;
    for (uint32_t i = =; i < timeout; i++)
    {
      t_len = CBuffer_DataCount(&cbuffer_handle);
      if (t_len >= rx_len)
      {
        t_len = rx_len;
        break;
      }
    }
  }

  CBuffer_Read(&cbuffer_handle, rx_buf, t_len);

  // Clear buffer 
  CBuffer_Clear(&cbuffer_handle);
  memset(&cbuffer_data, 0x00, sizeof(cbuffer_data));

  return INET_STATUS_OK;
}