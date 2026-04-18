#pragma once

/**
 * @file nms_can.h
 * 
 ********************************** README *****************************************
 * This file contains the software infrastructure to handle CAN messages for any MCU
 * on the NMS Electronics System. This is only a software-level infrastructure, which
 * means that the hardware interface to the CAN buses must still be implemented in the
 * MCU project folder. This software processes CAN message through a TX and an RX 
 * FreeRTOS tasks, which means that the MCU software is responsible for adding CAN 
 * messages to both queues as desired.
 * 
 * There are a couple of steps to take to flawlessly integrate this software into an 
 * MCU project:
 * - Include "nms_can.h" in a file in the MCU project folder (it is highly suggested
 *      to include this in "can.h")
 * - Define "canBuses" in a file in the MCU project folder based on the CAN buses
 *      physically connected to the MCU (it is highly suggested to define it in 
 *      "can.c")
 * - Define "canHwTx" and "canHwInit" to provide this software infrastructure a 
 *      hardware interface to the CAN buses
 * - Call "canInit"!!!
 * 
 * Here are also some additional tips for better implementations:
 * - Reference "ecu-front" firmware to see how the software integration was done
 * - Define a "CanBusMap" (or similar) enum that contains the mapping of the physical
 *      CAN bus name to the index within the "canBuses" array. This defines the CAN bus
 *      ID of each bus (which is part of the CAN message structure and should allow
 *      the software to understand how to route CAN messages)
 * - Define functions to convert bus IDs to CAN peripheral handles specific to the MCU
 *      HAL driver (this allows for seamless determination of which bus each message 
 *      should be transmitted to)
 */

#include "FreeRTOS.h"
#include "semphr.h"
#include <stdbool.h>
#include <stdint.h>

#define CAN_MSG_TIMESTAMP(converter, msg_name) (converter->can_##msg_name##_time_stamp_rx)

#define CAN_MSG_TIMEOUT(converter, msg_name, timeout)                                              \
  (((xTaskGetTickCount() - CAN_MSG_TIMESTAMP(converter, msg_name)) * portTICK_PERIOD_MS) > timeout)

/**
 * @brief This struct holds a CAN bus object
 * 
 * Each CAN bus has a converter, a function to unpack CAN messages into signals,
 * a function to pack signals into CAN messages, a function that returns the 
 * DLC of any given CAN message on that bus, and TX and RX semaphores that can be
 * used to manage TX and RX operations on the CAN bus
 * Each CAN bus also has error counters for TX hardware, TX software, RX hardware,
 * and RX software errors
 */
typedef struct
{
  void *converter;
  int (*unpack_message)(void *converter, const unsigned long id, uint64_t data, uint8_t dlc,
                        uint32_t time_stamp);
  int (*pack_message)(void *converter, const unsigned long id, uint64_t *data);
  int (*message_dlc)(const unsigned long id);
  uint16_t txHwErrCount, txSwErrCount, rxHwErrCount, rxSwErrCount;
  SemaphoreHandle_t rxSem, txSem; // semaphores to manage rx and tx messages for the CAN bus
} CanBus_t;

/**
 * @brief Pointer to an array of CanBus_t that holds the CAN bus objects for each bus
 * connected to the MCU that includes this software interface.
 * 
 * THIS MUST BE DEFINED WITHIN THE MCU'S PROJECT FOLDER (i.e. in the can.c file)
 */
extern CanBus_t *canBuses;

typedef union
{
  uint64_t data_u64;
  uint8_t data_u8[8];
} CanData_t;

typedef struct
{
  uint32_t id;
  uint8_t dlc;
  uint8_t bus_id;
  CanData_t data;
} CanMsg_t;

/**
 * @note THIS MUST BE DEFINED WITHIN THE MCU'S PROJECT FOLDER (i.e. in the can.c file)
 * 
 * @brief Transmit a CAN message on a physical CAN bus
 * 
 * @param msg CAN message
 * @return true CAN message was transmitted on CAN bus
 * @return false CAN message was not transmitted CAN bus
 */
bool canHwTx(CanMsg_t *msg);

/**
 * @note THIS MUST BE DEFINED WITHIN THE MCU'S PROJECT FOLDER (i.e. in the can.c file)
 * 
 * @brief Initialize all the CAN hardware peripherals
 * 
 */
void canHwInit(void);

/**
 * @brief  Add CAN message to transmit queue
 * 
 * @param msg CAN message
 * @return true CAN message was queued
 * @return false CAN message was not queued
 */
bool canTx(CanMsg_t *msg);

/**
 * @brief Add CAN message to the receive queue to be processed
 * 
 * @param msg CAN message
 * @return true CAN message was queued
 * @return false CAN message was not queued
 */
bool canRx(CanMsg_t *msg);

/**
 * @brief Pack a CAN message using a converter, given the message's ID
 * and bus ID, and add the CAN message to the transmit queue. 
 * 
 * @note Signals must be written to the correct converter using 
 * "encode" functions for this to transmit the correct data
 * 
 * @param id CAN message ID
 * @param bus_id CAN bus ID
 * @return true CAN message was packed and added to the transmit queue
 * @return false CAN message not packed and not added to transmit queue
 */
bool packCanTx(uint32_t id, uint8_t bus_id);

/**
 * @brief Initialize CAN tasks, queues, and hardware peripherals if necessary
 * 
 * @param txQueueSize size of the CAN transmit queue. If 0, tx task will not be created
 * @param rxQueueSize size of the CAN receive queue. If 0, rx task will not be created
 */
void canInit(uint32_t txQueueSize, uint32_t rxQueueSize);