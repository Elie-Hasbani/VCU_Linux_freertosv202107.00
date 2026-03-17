#define DERATE_INVERTER_HIGHTEMP (1U << 0)
#define DERATE_INVERTER_OVERTEMP (1U << 1)
#define DERATE_UNDERVOLT (1U << 2)
#define DERATE_OVERCURR (1U << 3)
#define DERATE_APPS1_OUTDATED (1U << 4)
#define DERATE_APPS2_OUTDATED (1U << 5)
#define DERATE_WHEEL_OUTDATED(i) (1U << 6 + (i))
#define DERATE_BREAK_OUTDATED (1U << 10)
#define DERATE_INVERTER_TEMP_OUTDATED (1U << 11)
#define DERATE_MOTOR_TEMP_OUTDATED (1U << 12)
#define DERATE_MOTOR_HIGHTEMP (1U << 13)
#define DERATE_MOTOR_OVERTEMP (1U << 14)

typedef enum canIds
{
    wheel1Id = 0x20,
    wheel2Id = 0x21,
    wheel3Id = 0x22,
    wheel4Id = 0x23,

    apps1Id = 0x30,
    apps2Id = 0x31,

    motor_tempId = 0x40,
    inverter_tempId = 0x41,
    voltageId = 0x42,

    throtle_cmdId = 0x80

} CanIds;
