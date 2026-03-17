
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

void EmulatorCanRx(void *pvParameters)
{
    CAN_Replay_Init("./dataSets/can_messages.csv");
    TickType_t lastWakeUp = xTaskGetTickCount();
    unsigned long i = 0;
    CanMessage_t msg = {0};

    CanRxParams_t *params = (CanRxParams_t *)pvParameters;
    QueueHandle_t *xTemperatureVoltageQueue = params->xTemperatureVoltageQueue;
    QueueHandle_t *xMotorControllerQueue = params->xMotorControllerQueue;

    while (1)
    {
        console_print("------(B)EmulatorCanRx------\n");

        dataCan canMsg;
        if (CAN_ReadNext(&canMsg))
        {
            msg.id = canMsg.id;
            msg.data = canMsg.data;
            msg.timestamp = xTaskGetTickCount();

            console_print("decoded CAN msg: %x %d\n", msg.id, msg.data);

            // Envoyer à la queue appropriée selon l'ID
            if (msg.id >= 0x40 && msg.id <= 0x42)
            {
                xQueueSend(*xTemperatureVoltageQueue, &msg, pdMS_TO_TICKS(5));
            }
            else if ((msg.id >= 0x20 && msg.id <= 0x23) || (msg.id >= 0x30 && msg.id <= 0x31))
            {
                xQueueSend(*xMotorControllerQueue, &msg, pdMS_TO_TICKS(5));
            }
        }
        // console_print((ledState = !ledState) ? "Led ON\n" : "Led OFF\n");
        msg.id++;
        msg.timestamp = xTaskGetTickCount();
        console_print("------(E)EmulatorCanRx------\n\n");

        xTaskDelayUntil(&lastWakeUp, pdMS_TO_TICKS(1000));
    }
}