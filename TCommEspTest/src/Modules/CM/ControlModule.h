#ifndef CONTROLMODULE_H
#define CONTROLMODULE_H

using namespace GenericLibrary;

class ControlModule : Entity
{
public:
    CTRENTITY(ControlModule) {
    }

    void Run()
    {
        // echo the test variables
        data.replyModuleActive = data.moduleActive;
        configuration.replyTestNumber = configuration.testNumber;
    }

private:
    struct Data : Entity
    {
        CTRENTITY(Data) {}
        XBOOL(moduleActive);
        XBOOL(replyModuleActive);
    } INSTENTITY(data);
    struct Configuration : Entity
    {
        CTRENTITY(Configuration) {}
        XINT(testNumber);
        XINT(replyTestNumber);
    } INSTENTITY(configuration);
};
#endif // CONTROLMODULE_H