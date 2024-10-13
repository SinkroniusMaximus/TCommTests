#ifndef DUMMYCOMMUNICATIONOBJECT_H
#define DUMMYCOMMUNICATIONOBJECT_H

#include "TemplateCommunicationObject.h"

namespace TComm {

template <typename T>
class DummyCommunicationObject : public TemplateCommunicationObject<T> {
public:
    DummyCommunicationObject() {}

    // Override to do nothing (no registration)
    void RegisterObject() override {}

    // Override other methods as needed to adjust behavior
};

} // namespace TComm

#endif // DUMMYCOMMUNICATIONOBJECT_H