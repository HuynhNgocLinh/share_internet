/*!
 *  @file		    inet.h
 *  @author  	  Linh Huynh
 *	@copyright	
 *  @version 	  1.00.00.20190420
 *  @date    	  April 20 , 2019
 *  @note
 */
#ifndef __INET_H__
#define __INET_H__

#ifdef __cplusplus
extern "C"{
#endif
/*******************************************************************************
 *  INCLUDES
 ******************************************************************************/
#include "usbd_cdc_if.h"
/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/*******************************************************************************
 * TYPEDEF
 ******************************************************************************/
/**
 * @brief
 */
typedef enum 
{
  INET_STATUS_OK,
  INET_STATUS_ERR_NULL_PTR,
  c
} inet_status_t;

/**
 * @brief
 */
typedef enum 
{
  INET_CONNECTION_STATUS_CONNECTED,
  INET_CONNECTION_STATUS_DISCONNECTED
} inet_connection_status_t;

typedef char     inet_ip_addr_t;

typedef char     inet_type_t;

typedef uint16_t inet_port_t;

typedef uint8_t  inet_data_t;

typedef uint8_t  inet_length_t;

/**
 * @brief
 */
typedef struct 
{
  inet_connection_status_t status;
  inet_ip_addr_t           ip_addr[15];
  inet_type_t              type;
  inet_port_t              port;
} inet_info_connection_t;

/**
 * @brief
 */
typedef struct 
{

} inet_t;

/**
 * @brief
 */
typedef void  (*inet_callback_t) (uint8_t *buf, uint32_t len);
/*******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
inet_status_t inet_init();

inet_status_t inet_get_info_connection(inet_info_connection_t *info_connection);

inet_status_t inet_establish_tcp_connection(inet_t         *inet, 
                                            inet_ip_addr_t *ip_addr,
                                            inet_port_t     port);

inet_status_t inet_establish_udp_connection(inet_t         *inet, 
                                            inet_ip_addr_t *ip_addr,
                                            inet_port_t     port);

inet_status_t inet_send_data(inet_data_t *data, inet_length_t len);

inet_status_t inet_receive_data(inet_data_t *data);

inet_status_t inet_close_connection();




#ifdef __cplusplus
}
#endif

#endif // __INET_H__