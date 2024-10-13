#ifndef TYPEREGISTRY_H
#define TYPEREGISTRY_H
namespace TComm
{
    class TypeRegistry {
    public:
        using CreateFunction = void(*)(int);
        // Register a type with the registry
        template<typename T>
        static void RegisterType() 
        {
            const std::string typeName = TypeName<T>::Get();
            registry()[typeName] = [](int deviceIndex) -> void
            {
                TemplateCommunicationObject<T>* object = new TemplateCommunicationObject<T>(deviceIndex);
            };
        }

        // Create an object based on the registered type name
        static void CreateObject(const char* typeName, int deviceIndex) 
        {
            // PRINT("creating object");
            auto it = registry().find(typeName);
            if (it != registry().end()) 
            {
                it->second(deviceIndex);
            }
            // PRINT("object created");
        }
        static std::string Demangle(const char* name) 
        {
            int status = 0;
            std::unique_ptr<char, void(*)(void*)> demangled(
                abi::__cxa_demangle(name, nullptr, nullptr, &status), 
                std::free
            );

            if (status != 0) 
            {
                // If demangling fails, return the original name
                return std::string(name);
            }
            return std::string(demangled.get());
        }

    private:
        template<typename T>
        struct TypeName 
        {
            static const std::string Get() 
            {
                static std::string demangledName = Demangle(typeid(T).name());
                return demangledName;
            }
        };

        static std::unordered_map<std::string, CreateFunction>& registry() {
            static std::unordered_map<std::string, CreateFunction> instance;
            return instance;
        }
    };
};
#endif // TYPEREGISTRY_H