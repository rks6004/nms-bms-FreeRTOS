#include "nms_can.h"
#include "queue.h"
#include "task.h"
#include "util.h"

static QueueHandle_t canRxQueueHandle = NULL;
static QueueHandle_t canTxQueueHandle = NULL;

bool packCanTx(uint32_t id, uint8_t bus_id)
{
  CanMsg_t msg;

  msg.id = id;
  msg.dlc = canBuses[bus_id].message_dlc(id);
  msg.bus_id = bus_id;
  // check if we are in an interrupt context
  if (IS_ISR())
  {
    // pack the message using the converter of the CAN bus
    // corresponding to the bus_id of the message
    canBuses[bus_id].pack_message(canBuses[bus_id].converter, id, &msg.data.data_u64);
  }
  else
  {
    taskENTER_CRITICAL();
    // pack the message using the converter of the CAN bus
    // corresponding to the bus_id of the message
    canBuses[bus_id].pack_message(canBuses[bus_id].converter, id, &msg.data.data_u64);
    taskEXIT_CRITICAL();
  }

  // add the message to the transmit queue
  return canTx(&msg);
}

bool canTx(CanMsg_t *msg)
{
  if (IS_ISR())
  {
    if (!xQueueSendFromISR(canTxQueueHandle, msg, NULL))
    {
      canBuses[msg->bus_id].txSwErrCount += 1;
      return false;
    }
  }
  else
  {
    if (!xQueueSend(canTxQueueHandle, msg, 0))
    {
      canBuses[msg->bus_id].txSwErrCount += 1;
      return false;
    }
  }
  return true;
}

bool canRx(CanMsg_t *msg)
{
  // check if we are in an interrupt context
  if (IS_ISR())
  {
    if (!xQueueSendFromISR(canRxQueueHandle, msg, NULL))
    {
      canBuses[msg->bus_id].rxSwErrCount += 1;
      return false;
    }
  }
  else
  {
    if (!xQueueSend(canRxQueueHandle, msg, 0))
    {
      canBuses[msg->bus_id].rxSwErrCount += 1;
      return false;
    }
  }
  return true;
}

/**
 * @brief Task to receive CAN messages
 * 
 * This task runs indefinitely, waiting for messages to be added to the queue.
 * When a message is received, it unpacks the data using the CAN converter.
 * 
 * @param args Unused parameter
 */
static void canRxTask(void *args)
{
  CanMsg_t msg;

  for (;;)
  {
    // if there is a message to be received in the FIFO, service it
    if (xQueueReceive(canRxQueueHandle, &msg, portMAX_DELAY))
    {
      taskENTER_CRITICAL();
      // unpack the message using the converter of the CAN bus
      // corresponding to the bus_id of the message
      canBuses[msg.bus_id].unpack_message(canBuses[msg.bus_id].converter,
                                          msg.id,
                                          msg.data.data_u64,
                                          msg.dlc,
                                          xTaskGetTickCount());
      taskEXIT_CRITICAL();
    }
  }
}

/**
 * @brief Task to transmit CAN messages
 * 
 * This task runs indefinitely, waiting for messages to be added to the queue.
 * When a message is received, it is transmitted to the physical CAN bus.
 * 
 * @param args Unused parameter
 */
static void canTxTask(void *args)
{
  CanMsg_t msg;

  for (;;)
  {
    // if there is a message to be sent in the FIFO, service it
    if (xQueueReceive(canTxQueueHandle, &msg, portMAX_DELAY))
    {
      // transmit the message on the physical CAN bus and check for errors
      if (!canHwTx(&msg))
      {
        canBuses[msg.bus_id].txHwErrCount += 1;
      }
    }
  }
}

void canInit(uint32_t txQueueSize, uint32_t rxQueueSize)
{

  // create the CAN TX task if necessary
  if (txQueueSize)
  {
    canTxQueueHandle = xQueueCreate(txQueueSize, sizeof(CanMsg_t));
    xTaskCreate(canTxTask, "canTxTask", configMINIMAL_STACK_SIZE, NULL, PRIORITY_HIGH, NULL);
  }

  // create the CAN RX task if necessary
  if (rxQueueSize)
  {
    canRxQueueHandle = xQueueCreate(rxQueueSize, sizeof(CanMsg_t));
    xTaskCreate(canRxTask, "canRxTask", configMINIMAL_STACK_SIZE, NULL, PRIORITY_HIGH, NULL);
  }

  // initialize the CAN hardware peripherals
  canHwInit();
}
