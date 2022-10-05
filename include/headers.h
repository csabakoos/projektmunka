/*
    Here the current system must be defined!
    (It can either be LEFT for the left glove or RIGHT for the right one.)

    It is crucial to do so since all the later I2C communications with the sensors will
    depend on the individual PIN assignments that can be seen and set accordingly below.
    (In case of a mismatch no error will shown only FALSE SENSORY DATA will be recieved,
    hence the grave importance of being precise with this!)
*/
#define SYSTEM RIGHT

/*
    Below can be seen and changed the appropriate pins for the LEFT or RIGHT hand.
    These are the ones that are currently associated with the individual fingers.
    (From the 1st which is the thumb all the way to 5th that is the pinky!)
*/
#if SYSTEM == LEFT
    #define MPU_1_SCL 17
    #define MPU_1_SDA 16
    #define MPU_2_SCL 19
    #define MPU_2_SDA 18
    #define MPU_3_SCL 32
    #define MPU_3_SDA 33
    #define MPU_4_SCL 25
    #define MPU_4_SDA 26
    #define MPU_5_SCL 27
    #define MPU_5_SDA 14
#elif SYSTEM == RIGHT
    #define MPU_1_SCL 27
    #define MPU_1_SDA 14
    #define MPU_2_SCL 25
    #define MPU_2_SDA 26
    #define MPU_3_SCL 32
    #define MPU_3_SDA 33
    #define MPU_4_SCL 21
    #define MPU_4_SDA 19
    #define MPU_5_SCL 17
    #define MPU_5_SDA 16
#endif

