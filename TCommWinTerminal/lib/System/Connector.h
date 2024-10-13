#ifndef CONNECTOR_H
#define CONNECTOR_H

namespace GenericLibrary
{
    
template<typename T>
class Connector : public BaseConnector<T>
{
protected:
    using BaseConnector<T>::value; // Bring value from base class into scope
public:

    std::uintptr_t Address() const {
        return reinterpret_cast<std::uintptr_t>(value);
    }

    // Assignment operator to update the connection
    Connector<T>& operator=(const T& newValue) {
        if (value) {
            *value = newValue;
        }
        return *this;
    }

    // Assignment operator to update the connection
    Connector<T>& operator=(const Connector<T>& connector) {
        if (value) {
            *value = *connector.value;
        }
        return *this;
    }

    // Conversion operator to access the connected object
    operator T&() {
        if (!value) {
            static T defaultVal = T();
            return defaultVal;
        }
        return *value;
    }
    // operator T*(){
    //     return value;
    // }

    // Comparison operators
    bool operator==(const Connector<T>& other) const {
        if (!value || !other.value) {
            return false;
        }
        return *value == *(other.value);
    }

    bool operator!=(const Connector<T>& other) const {
        if (!value || !other.value) {
            return true;
        }
        return *value != *(other.value);
    }

    bool operator<(const Connector<T>& other) const {
        if (!value || !other.value) {
            return false;
        }
        return *value < *(other.value);
    }

    bool operator<=(const Connector<T>& other) const {
        if (!value || !other.value) {
            return false;
        }
        return *value <= *(other.value);
    }

    bool operator>(const Connector<T>& other) const {
        if (!value || !other.value) {
            return false;
        }
        return *value > *(other.value);
    }

    bool operator>=(const Connector<T>& other) const {
        if (!value || !other.value) {
            return false;
        }
        return *value >= *(other.value);
    }

    T operator-(const Connector<T>& other) const {
        T result;
        if (value && other.value) {
            result = *value - *other.value;
        }
        return result;
    }

    T operator+(const Connector<T>& other) const {
        T result;
        if (value && other.value) {
            result = *value + *other.value;
        }
        return result;
    }

    T operator*(const Connector<T>& other) const {
        T result;
        if (value && other.value) {
            result = *value * *other.value;
        }
        return result;
    }

    T operator/(const Connector<T>& other) const {
        T result;
        if (value && other.value) {
            result = *value / *other.value;
        }
        return result;
    }

    Connector<T> operator++() const {
        if (value) {
            --(*value);
        }
        return *this;
    }

    Connector<T> operator--() const {
        if (value) {
            ++(*value);
        }
        return *this;
    }

    Connector<T> operator++(int) {
        Connector<T> temp(*this); // Create a copy of the current object
        if (value) {
            ++(*value); // Increment the underlying value
        }
        return temp; // Return the copy of the object before increment
    }

    Connector<char> operator--(int) {
        Connector<char> temp(*this); // Create a copy of the current object
        if (value) {
            --(*value); // Decrement the underlying value
        }
        return temp; // Return the copy of the object before decrement
}

};
}

#endif // CONNECTOR_H