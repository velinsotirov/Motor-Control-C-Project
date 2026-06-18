
#include <windows.h>
#include <stdbool.h>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

#include "serialib.h"
#include "commModule.h"

CommModule::CommModule() {

    // find all open comm ports
    for (int i = 1; i <= 32; i++) {
        std::string portName = "COM" + std::to_string(i);

        char targetPath[5000];
        if (QueryDosDeviceA(portName.c_str(), targetPath, sizeof(targetPath)) != 0) {
            availablePorts.push_back(portName);
        }
    }
}
CommModule::~CommModule() {
    // if the thread is running, stop it
    if (receiveLoopEnabled) {
        closePort();
    }
}

void CommModule::receiveThreadLoop() {
    uint8_t byte;
    while(receiveLoopEnabled) {
        // read one byte, with timeout
        int result = serial.readBytes(&byte, 1, READ_BYTE_TIMEOUT_MS);

        // check if we read a byte within the time or not
        if (result == 1) {
            // we successfully read a byte within the time, push to buffer
            packetBuffer[num_bytes_received] = byte;
            num_bytes_received++;
        }
        else {
            // we did NOT successfully read a byte within the time
            // reset buffer
            num_bytes_received = 0u;
        }

        // check if we wrote 8 bytes, if so, copy buffer to exhange variable and reset
        if (num_bytes_received == RX_PACKET_LEN) {
            // lock mutex while this if statement is executing (we are copying)
            std::lock_guard<std::mutex> guard(mutexLock);
            for (int i = 0; i < RX_PACKET_LEN; i++) {
                lastReceivedPacket[i] = packetBuffer[i];
            }
            num_bytes_received = 0u;
        }
    }
}

void CommModule::getLastReceivedPacket(uint8_t *packet, uint8_t len) {
    // lock mutex while this function is executing (we are copying)
    std::lock_guard<std::mutex> guard(mutexLock);
    for (int i = 0; i < RX_PACKET_LEN; i++) {
        packet[i] = lastReceivedPacket[i];
    }
}

std::vector<std::string>& CommModule::getAvailablePorts() {
    return availablePorts;
}

bool CommModule::openPort(int selectedPortNumber) {
    std::string selectedPort = availablePorts[selectedPortNumber];
    char openResult = serial.openDevice(selectedPort.c_str(),115200,SERIAL_DATABITS_8,SERIAL_PARITY_NONE,SERIAL_STOPBITS_1);

    // failed to open
    if (openResult != 1) {
        return false;
    }

    // turn on UART polling loop "receiveThreadLoop" and allow it to read data
    receiveLoopEnabled = true;
    receiveThread = std::thread(&CommModule::receiveThreadLoop, this);
    return true;
}

void CommModule::closePort() {
    // disallow thread loop and close it
    receiveLoopEnabled = false;

    // try closing a thread only if a thread is open
    if (receiveThread.joinable()) {
        receiveThread.join();
    }

    // close UART COM port
    serial.closeDevice();
}

void CommModule::sendPacket(uint8_t *packet, uint8_t len) {
    serial.writeBytes(packet, len);
}