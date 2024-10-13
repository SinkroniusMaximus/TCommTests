#ifndef ABSTRACTCONNECTOR_H
#define ABSTRACTCONNECTOR_H

class AbstractConnector {
public:
    virtual ~AbstractConnector() = default;
    
    // Add common methods if needed, like:
    virtual void Connect(void* obj) = 0;  // Pure virtual method to enforce implementation
    // Virtual method to get the type_info of the underlying type
    virtual const std::type_info& GetType() const = 0;
};

#endif // ABSTRACTCONNECTOR_H