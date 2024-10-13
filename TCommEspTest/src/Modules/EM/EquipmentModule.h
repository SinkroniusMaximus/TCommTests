#ifndef EQUIPMENTMODULE_H
#define EQUIPMENTMODULE_H

using namespace GenericLibrary;

class EquipmentModule : Entity
{
    public:
        CTRENTITY(EquipmentModule) {};
        void Run()
        {
            updateTimer.Loop();
            if(updateTimer.Done())
            {
                data.controlModule.Run();
                data.replyTestBit = data.testBit;
                configuration.replyTestInt = configuration.testInt;
                configuration.manual.replyTestFloat = configuration.manual.testFloat;
            }      
        }
    private:
        struct Data : Entity
        {
            CTRENTITY(Data){}
            ControlModule INSTENTITY(controlModule);
            XBOOL(testBit);
            XBOOL(replyTestBit);
        } INSTENTITY(data);
        struct Configuration : Entity
        {
            CTRENTITY(Configuration){}
            XINT(testInt);
            XINT(replyTestInt);
            struct Manual : Entity
            {
                CTRENTITY(Manual){}
                XFLOAT(testFloat);
                XFLOAT(replyTestFloat);
            } INSTENTITY(manual);
        } INSTENTITY(configuration);
        Timer updateTimer{50};
};

#endif // EQUIPMENTMODULE_H