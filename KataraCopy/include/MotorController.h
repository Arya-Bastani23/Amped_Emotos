#ifndef MOTORCONTROLLER_h
#define MOTORCONTROLLER_h
    #include <Arduino.h>
    #include <A_CAN.h>

    class MotorController {
        private:
            int idk;
            void SDOWrite(int numBytestoWrite, INT8U *data);
            void SDORead(INT8U *data);
            void SDORErrorResponse();
            INT8U calcUpperIndex(INT32U address);
            INT8U calcLowerIndex(INT32U address);
            INT8U calcSubIndex(INT32U address);

            A_CAN *canBus;
        public:
            MotorController(A_CAN *canBus1);
            void update();
            void enableMotor();
            void idleMotor();
            void disbleMotor();
            void sendRemStateCommand(int command);
            void sendRemTorqueComm(int torquePercent);

            void setRemMaxBrakeCurrent(int currentPercent);
            void setRemMaxMotoringCurrent(int currentPercent);
            



    };

#endif