#ifdef __linux__
#ifndef LINUXSERIAL_H
#define LINUXSERIAL_H

namespace TComm
{
    class HardwareSerial
    {
    public:
        HardwareSerial() : serialHandle(-1) {}
        ~HardwareSerial()
        {
            if (serialHandle != -1) close(serialHandle);
        }

        void begin(const std::string& comPort, unsigned long baudrate)
        {
            if (serialHandle != -1) close(serialHandle);

            serialHandle = open(comPort.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
            if (serialHandle < 0)
            {
                std::cerr << "Error opening " << comPort << ": " << strerror(errno) << std::endl;
                return;
            }

            struct termios tty;
            if (tcgetattr(serialHandle, &tty) != 0)
            {
                std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
                return;
            }

            cfsetospeed(&tty, baudrate);
            cfsetispeed(&tty, baudrate);

            tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
            tty.c_iflag &= ~IGNBRK;
            tty.c_lflag = 0;
            tty.c_oflag = 0;
            tty.c_cc[VMIN] = 1;
            tty.c_cc[VTIME] = 5;
            tty.c_iflag &= ~(IXON | IXOFF | IXANY);
            tty.c_cflag |= (CLOCAL | CREAD);
            tty.c_cflag &= ~(PARENB | PARODD);
            tty.c_cflag |= 0;
            tty.c_cflag &= ~CSTOPB;
            tty.c_cflag &= ~CRTSCTS;

            if (tcsetattr(serialHandle, TCSANOW, &tty) != 0)
            {
                std::cerr << "Error from tcsetattr: " << strerror(errno) << std::endl;
            }
        }

        char read()
        {
            char buf;
            int n = ::read(serialHandle, &buf, 1);
            return (n > 0) ? buf : -1;
        }

        bool write(const uint8_t* buffer, int size)
        {
            int written = ::write(serialHandle, buffer, size);
            return written == size;
        }

        int available()
        {
            int nbytes;
            ioctl(serialHandle, FIONREAD, &nbytes);
            return nbytes;
        }

        std::vector<std::string> listAvailablePorts()
        {
            std::vector<std::string> ports;
            struct dirent* entry;
            DIR* dp = opendir("/dev");

            if (dp == nullptr)
            {
                std::cerr << "Error opening /dev directory: " << strerror(errno) << std::endl;
                return ports;
            }

            while ((entry = readdir(dp)))
            {
                std::string name = entry->d_name;
                if (name.find("ttyS") == 0 || name.find("ttyUSB") == 0)
                {
                    ports.push_back("/dev/" + name);
                }
            }

            closedir(dp);
            return ports;
        }

    private:
        int serialHandle;
    };
}

#endif // LINUXSERIAL_H
#endif // __linux__