#ifndef BYTESERIALIZER_H
#define BYTESERIALIZER_H

namespace TComm {
class ByteSerializer
{
    public:
        ByteSerializer()
        {
            readIndex = 0;
        }
        void Serialize(CommunicationData data, byte out[])
        {
            //format: [startKey, deviceIndex, size, commIndex, data bytes]
            uint16_t startKey = 0xDBFB;
            memcpy(out, &startKey, 2); // 0,1
            memcpy((out + 2), &data.deviceIndex, 2); // 2,3
            memcpy((out + 4), &data.dataSize, 2); // 4,5
            memcpy((out + 6), &data.objectIndex, 2); //6,7
            memcpy((out + 8), data.buffer, data.dataSize); // size:4 -> 8,9,10,11
        };
        CommunicationData* Deserialize(byte read)
        {
            //store a byte
            buffer[readIndex] = read;
            //confirm the startKey is correct
            if(readIndex == 0 && buffer[0] != 0xFB
               || readIndex == 1 && buffer[1] != 0xDB) 
            {
                readIndex = 0;
                return nullptr;
            }
            //store the deviceIndex
            if(readIndex == 3) 
            { 
                receivedData.deviceIndex = buffer[2] + (buffer[3] << 8);
            }
            //store the size of the data
            if(readIndex == 5)
            {
                receivedData.dataSize = buffer[4] + (buffer[5] << 8);
            }
            //store the list index of the communicated data
            if(readIndex == 7)
            {
                receivedData.objectIndex = buffer[6] + (buffer[7] << 8);
            }
            //relay and inject the completed message
            if(readIndex == (receivedData.dataSize + 7))
            {
                receivedData.buffer = buffer + 8;
                readIndex = 0;
                return &receivedData;
            }
            readIndex++;
            return nullptr;
        }
    private:
        byte buffer[512];
        uint16_t readIndex;
        CommunicationData receivedData;
}; 
};

#endif //BYTESERIALIZER_H
