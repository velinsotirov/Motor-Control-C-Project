#ifndef COMMMODULE_H
#define COMMMODULE_H

#include <windows.h>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <stdint.h>
#include <stdbool.h>

#include "serialib.h"

class CommModule {
    public:
        // checks which ports are available
        CommModule();

        // closes port
        ~CommModule();

        // when open port button is pushed
        bool openPort(int);

        // when close port button is pushed
        void closePort();

        // called in app to send output commands
        void sendPacket(uint8_t *packet, uint8_t len);

        // function called on the receive thread, it has an infinite loop which reads the bytes
        void receiveThreadLoop();

        // called in app cyclically to update the mode, speed and torque values shown on screen
        void getLastReceivedPacket(uint8_t *packet, uint8_t len);

        std::vector<std::string>& getAvailablePorts();

        static constexpr uint8_t TX_PACKET_LEN {3u};

    private:
        std::vector<std::string> availablePorts;
        serialib serial;

        std::thread receiveThread;
        std::atomic<bool> receiveLoopEnabled {false};
        std::mutex mutexLock;

        static const uint8_t READ_BYTE_TIMEOUT_MS {100u};
        static constexpr uint8_t RX_PACKET_LEN {8u};
        uint8_t packetBuffer[RX_PACKET_LEN];
        uint8_t num_bytes_received {0u};
        uint8_t lastReceivedPacket[RX_PACKET_LEN] {0u};
};

#endif // COMMMODULE_H