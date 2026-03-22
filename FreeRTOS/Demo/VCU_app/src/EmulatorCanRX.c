
#include "FreeRTOS.h"
#include "queue.h"

#include "structs.h"

#include "console.h"
#include "EmulatorCanRX.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    uint8_t id;
    uint32_t data;
} dataCan;

static FILE *file = NULL;

/**
 * @brief Initialise la lecture du CSV
 */
int CAN_Replay_Init(const char *filename)
{
    file = fopen(filename, "r");
    if (!file)
    {
        console_print("no file found");
        return 0;
    }

    // Skip header (id;data)
    char buffer[128];
    fgets(buffer, sizeof(buffer), file);

    return 1;
}

/**
 * @brief Lit le prochain message depuis le CSV
 */
int CAN_ReadNext(dataCan *msg)
{
    if (!file || !msg)
        return 0;

    char line[128];

    while (1)
    {
        // lire une ligne
        if (fgets(line, sizeof(line), file) == NULL)
        {
            rewind(file);

            // skip header
            if (fgets(line, sizeof(line), file) == NULL)
                return 0;

            continue;
        }

        // trouver le séparateur ;
        char *sep = strchr(line, ';');
        if (!sep)
            continue;

        *sep = '\0';

        // parse ID (hex accepté grâce à base 0)
        unsigned long id = strtoul(line, NULL, 0);

        // parse data
        int data = strtol(sep + 1, NULL, 10);

        // sécurité
        if (id > 0xFF)
        {
            console_print("Invalid ID: %lu\n", id);
            continue;
        }

        msg->id = (uint8_t)id;
        msg->data = data;

        return 1;
    }
}

/**
 * @brief Ferme le fichier
 */
void CAN_Replay_Close(void)
{
    if (file)
    {
        fclose(file);
        file = NULL;
    }
}

typedef enum canIds
{
    CAN_wheelFRId = 0x20,
    CAN_wheelFLId = 0x21,
    CAN_wheelRRId = 0x22,
    CAN_wheelRLId = 0x23,

    CAN_apps1Id = 0x30,
    CAN_apps2Id = 0x31,

    CAN_motor_tempId = 0x40,
    CAN_inverter_tempId = 0x41,
    CAN_voltageId = 0x42,

} CanIds;

void EmulatorCanRx(void *pvParameters)
{
    CAN_Replay_Init("./dataSets/can_messages.csv");
    TickType_t lastWakeUp = xTaskGetTickCount();

    CanRxParams_t *params = (CanRxParams_t *)pvParameters;
    QueueHandle_t *xMainQueue = params->xMainQueue;
    QueueHandle_t *xTRVPQueue = params->xTRVPQueue;

    dataMessage_t msg = {0};

    while (1)
    {
        console_print("------(B)EmulatorCanRx------\n");

        dataCan canMsg;
        if (CAN_ReadNext(&canMsg))
        {
            msg.data = canMsg.data;
            msg.timestamp = xTaskGetTickCount();
            dataMessage_t msg = {0};

            console_print("decoded CAN msg: %x %d\n", msg.id, msg.data);

            // Envoyer à la queue appropriée selon l'ID avec switch
            switch ((CanIds)msg.id)
            {
            // Wheel speeds went to Main Task
            case CAN_wheelFRId:
                msg.id = wheelFRId;
                xQueueSend(*xTRVPQueue, &msg, pdMS_TO_TICKS(5));
                break;
            case CAN_wheelFLId:
                msg.id = wheelFLId;
                xQueueSend(*xTRVPQueue, &msg, pdMS_TO_TICKS(5));
                break;
            case CAN_wheelRRId:
                msg.id = wheelRRId;
                xQueueSend(*xTRVPQueue, &msg, pdMS_TO_TICKS(5));
                break;
            case CAN_wheelRLId:
                msg.id = wheelRLId;
                xQueueSend(*xTRVPQueue, &msg, pdMS_TO_TICKS(5));
                break;

            case CAN_apps1Id:
                msg.id = apps1Id;
                xQueueSend(*xTRVPQueue, &msg, pdMS_TO_TICKS(5));
                break;
            case CAN_apps2Id:
                msg.id = apps2Id;
                xQueueSend(*xTRVPQueue, &msg, pdMS_TO_TICKS(5));
                break;

            case CAN_motor_tempId:
                msg.id = motor_tempId;
                xQueueSend(*xMainQueue, &msg, pdMS_TO_TICKS(5));
                break;
            case CAN_inverter_tempId:
                msg.id = inverter_tempId;
                xQueueSend(*xMainQueue, &msg, pdMS_TO_TICKS(5));
                break;
            case CAN_voltageId:
                msg.id = voltageId;
                xQueueSend(*xMainQueue, &msg, pdMS_TO_TICKS(5));
                break;
            default:
                console_print("Unknown CAN ID: %x\n", msg.id);
                break;
            }
        }
        // console_print((ledState = !ledState) ? "Led ON\n" : "Led OFF\n");
        msg.timestamp = xTaskGetTickCount();
        console_print("------(E)EmulatorCanRx------\n\n");

        xTaskDelayUntil(&lastWakeUp, pdMS_TO_TICKS(1000));
    }
}