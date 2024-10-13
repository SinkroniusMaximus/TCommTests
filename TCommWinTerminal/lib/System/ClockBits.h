#ifndef CLOCKBITS_H
#define CLOCKBITS_H
namespace GenericLibrary
{
static class ClockBits
{
    public:
        // return clock edged
        bool ClockEdge_0_5hz()
        {
            return data.edgeClock_0_5hz;
        }
        bool ClockEdge_0_625hz()
        {
            return data.edgeClock_0_625hz;
        }
        bool ClockEdge_1hz()
        {
            return data.edgeClock_1hz;
        }
        bool ClockEdge_2hz()
        {
            return data.edgeClock_2hz;
        }
        bool ClockEdge_2_5hz()
        {
            return data.edgeClock_2_5hz;
        }
        bool ClockEdge_5hz()
        {
            return data.edgeClock_5hz;
        }
        bool ClockEdge_10hz()
        {
            return data.edgeClock_10hz;
        }
        // return clock bits
        bool Clock_0_5hz()
        {
            return data.clock_0_5hz;
        }
        bool Clock_0_625hz()
        {
            return data.clock_0_625hz;
        }
        bool Clock_1hz()
        {
            return data.clock_1hz;
        }
        bool Clock_2hz()
        {
            return data.clock_2hz;
        }
        bool Clock_2_5hz()
        {
            return data.clock_2_5hz;
        }
        bool Clock_5hz()
        {
            return data.clock_5hz;
        }
        bool Clock_10hz()
        {
            return data.clock_10hz;
        }
        // run logic to generate the clock bits
        void Run()
        {
            // run a timer and detect clock pulse at 10 hz
            intervalTimer.Loop();   // TODO make interval based on interrupt
            if(intervalTimer.Done())
            {
                data.clock_10hz = !data.clock_10hz;
            }
            if(data.clock_10hz ^ data.prevClock_10hz)
            {
                data.count = (data.count + 1) % 160;
            }
            data.clock_5hz = (data.count % 4) >= 2;
            data.clock_2_5hz = (data.count % 8) >= 4;
            data.clock_2hz = (data.count % 10) >= 5;
            data.clock_1hz = (data.count % 20) >= 10;
            data.clock_0_625hz = (data.count % 32) >= 16;
            data.clock_0_5hz = (data.count % 40) >= 20;

            // detect rising edges of the clock bits
            data.edgeClock_0_5hz = data.clock_0_5hz && !data.prevClock_0_5hz;
            data.edgeClock_0_625hz = data.clock_0_625hz && !data.prevClock_0_625hz;
            data.edgeClock_1hz = data.clock_1hz && !data.prevClock_1hz;
            data.edgeClock_2hz = data.clock_2hz && !data.prevClock_2hz;
            data.edgeClock_2_5hz = data.clock_2_5hz && !data.prevClock_2_5hz;
            data.edgeClock_5hz = data.clock_5hz && !data.prevClock_5hz;
            data.edgeClock_10hz = data.clock_10hz && !data.prevClock_10hz;
            
            // memorize the state of the clockbits for next iteration
            data.prevClock_0_5hz = data.clock_0_5hz;
            data.prevClock_0_625hz = data.clock_0_625hz;
            data.prevClock_1hz = data.clock_1hz;
            data.prevClock_2hz = data.clock_2hz;
            data.prevClock_2_5hz = data.clock_2_5hz;
            data.prevClock_5hz = data.clock_5hz;
            data.prevClock_10hz = data.clock_10hz;
            
        }
    private:
        struct
        {
            bool clock_0_5hz;
            bool clock_0_625hz;
            bool clock_1hz;
            bool clock_2hz;
            bool clock_2_5hz;
            bool clock_5hz;
            bool clock_10hz;
            uint16_t count;
            bool prevClock_0_5hz;
            bool prevClock_0_625hz;
            bool prevClock_1hz;
            bool prevClock_2hz;
            bool prevClock_2_5hz;
            bool prevClock_5hz;
            bool prevClock_10hz;
            bool edgeClock_0_5hz;
            bool edgeClock_0_625hz;
            bool edgeClock_1hz;
            bool edgeClock_2hz;
            bool edgeClock_2_5hz;
            bool edgeClock_5hz;
            bool edgeClock_10hz;
        } data;

    Timer intervalTimer{50}; // 50 milliseconds

} clockBits;
};
#endif //CLOCKBITS_H