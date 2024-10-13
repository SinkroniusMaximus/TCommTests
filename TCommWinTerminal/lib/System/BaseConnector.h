#ifndef CONNECTORBASE_H
#define CONNECTORBASE_H

namespace GenericLibrary {

template<typename T>
class BaseConnector : public AbstractConnector
{
public:
    BaseConnector() : value(nullptr) {}

    void Connect(T& connection) {
        value = &connection;
    }

    void Connect(void* obj) override {
        value = static_cast<T*>(obj);
    }

    void Disconnect() {
        value = nullptr;
    }
    
    bool IsConnected() {
        return value;
    }
    
    // Override GetType to return the correct type_info
    const std::type_info& GetType() const override {
        return typeid(T);
    }

protected:
    T* value;
};

}

#endif // CONNECTORBASE_H