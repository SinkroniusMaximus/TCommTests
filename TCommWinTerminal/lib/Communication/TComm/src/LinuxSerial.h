#ifdef __linux__
#ifndef LINUXSERIAL_H
#define LINUXSERIAL_H

namespace TComm
{
    class HardwareSerial : public StreamInterface
    {
    public:
        HardwareSerial() : serialHandle(-1) {}
        ~HardwareSerial()
        {
            if (serialHandle != -1) ::close(serialHandle);
        }

        void begin(const std::string& comPort, unsigned long baudrate)
        {
            // PRINT("Opening serial port " << comPort);
            if (serialHandle != -1) close(serialHandle);
            
            serialHandle = open(comPort.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
            if (serialHandle < 0)
            {
                std::cerr << "Error opening " << comPort << ": " << strerror(errno) << std::endl;
                return;
            }
            
            // Use termios2 for custom baud rates
            struct termios2 tty;
            if (ioctl(serialHandle, TCGETS2, &tty) != 0) {
                std::cerr << "Error from TCGETS2: " << strerror(errno) << std::endl;
                return;
            }

            tty.c_iflag &= ~IGNBRK;                     // Ignore break condition
            tty.c_iflag &= ~(INLCR | ICRNL);            // Disable CR -> LF and NL -> CR conversion
            // tty.c_lflag = 0;                            // No signaling chars, no echo, no canonical processing
            // tty.c_oflag = 0;                            // No remapping, no delays
            tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Raw input
            tty.c_oflag &= ~OPOST;                      // Disable post-processing
            tty.c_cc[VMIN] = 1;                         // Read at least 1 character
            tty.c_cc[VTIME] = 5;                        // Timeout (0.5 seconds)

            tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // No software flow control
            tty.c_cflag |= (CLOCAL | CREAD);            // Enable receiver, set local mode
            tty.c_cflag &= ~CSIZE;
            tty.c_cflag |= CS8;                         // 8 data bits
            tty.c_cflag &= ~(PARENB | PARODD);          // No parity
            tty.c_cflag &= ~CSTOPB;                     // 1 stop bit
            tty.c_cflag &= ~CRTSCTS;                    // No hardware flow control

            // Set custom baud rate
            tty.c_cflag &= ~CBAUD;                      // Clear baud rate bits
            tty.c_cflag |= CBAUDEX;                     // Enable custom baud rate
            tty.c_ispeed = baudrate;                    // Set input speed
            tty.c_ospeed = baudrate;                    // Set output speed

            if (ioctl(serialHandle, TCSETS2, &tty) != 0) {
                std::cerr << "Error from TCSETS2: " << strerror(errno) << std::endl;
            } else {
                std::cout << "Serial port configured with custom baud rate: " << baudrate << std::endl;
            }
            PRINT("Opened serial port " << comPort);
        }


        int read() override {
            if (!readBuffer.empty()) {
                int byte = readBuffer.front();
                readBuffer.pop();
                char readchar = (char)byte;
                std::cout << readchar;
                std::cout.flush();
                return byte;
            }

            if (available() > 0) {
                char recvbuf[512];
                int iResult = ::read(serialHandle, recvbuf, sizeof(recvbuf));
                if (iResult > 0) {
                    for (int i = 0; i < iResult; ++i) {
                        readBuffer.push(recvbuf[i]);
                    }
                    int byte = readBuffer.front();
                    readBuffer.pop();
                    return byte;
                } else if (iResult == 0) {
                    std::cout << "Connection closing...\n";
                    Close();
                } else {
                    std::cerr << "Recv failed with error: " << iResult << std::endl;
                    Close();
                }
            }
            return -1;  // No data available or error
        }

        size_t write(const uint8_t* buffer, size_t size)
        {
            size_t written = ::write(serialHandle, buffer, size);
            return written;
        }

        int available()
        {
            int nbytes;
            ioctl(serialHandle, FIONREAD, &nbytes);
            return static_cast<int>(nbytes + readBuffer.size());
        }

        void Close()
        {
            if (serialHandle != -1) close(serialHandle);
        }

    private:
        int serialHandle;
        std::queue<char> readBuffer;
    };
}

#endif // LINUXSERIAL_H
#endif // __linux__