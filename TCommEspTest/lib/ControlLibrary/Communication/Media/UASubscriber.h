#ifndef UASUBSCRIBER
#define UASUBSCRIBER

#include "open62541pp/open62541pp.h"

using namespace TComm;

// Define the type mapping outside of the main function
std::unordered_map<const std::type_info*, opcua::Type> typeToUAType = {
    {&typeid(int), opcua::Type::Int32},
    {&typeid(int32_t), opcua::Type::Int32},
    {&typeid(uint8_t), opcua::Type::Byte},
    {&typeid(uint16_t), opcua::Type::UInt16},
    {&typeid(float), opcua::Type::Float},
    {&typeid(bool), opcua::Type::Boolean},
    {&typeid(std::string), opcua::Type::String}
};
std::unordered_map<const std::type_info*, opcua::DataTypeId> typeToUADType = {
    {&typeid(int), opcua::DataTypeId::Int32},
    {&typeid(int32_t), opcua::DataTypeId::Int32},
    {&typeid(uint8_t), opcua::DataTypeId::Byte},
    {&typeid(uint16_t), opcua::DataTypeId::UInt16},
    {&typeid(float), opcua::DataTypeId::Float},
    {&typeid(bool), opcua::DataTypeId::Boolean},
    {&typeid(std::string), opcua::DataTypeId::String}
};

class UASubscriber : BaseSubscriber
{
    public:
            // callback generator that bakes objectIndex into the lambda expression 
        std::function<void(const opcua::DataValue&)> createCustomCallback(int deviceIndex, int objectIndex, int dataSize) 
        {
            return [=](const opcua::DataValue& value) {
                if(!writingToNode)
                {
                    CommunicationData commData;
                    commData.buffer = (byte*)value.getValue().data();
                    commData.deviceIndex = deviceIndex;
                    commData.objectIndex = objectIndex;
                    commData.dataSize = dataSize;
                    PRINT("injected by opc");
                    InjectCommObject(commData, this);
                }
            };
        }
        std::function<void(const opcua::DataValue&)> createCustomCallbackForString(int deviceIndex, int objectIndex) 
        {
            return [=](const opcua::DataValue& value) {
                if(!writingToNode)
                {
                    CommunicationData commData;
                    UA_String* uaString = (UA_String*)(value.getValue().data());
                    commData.buffer = uaString->data;
                    commData.deviceIndex = deviceIndex;
                    commData.objectIndex = objectIndex;
                    commData.dataSize = uaString->length;
                    std::string receivedString(reinterpret_cast<char*>(commData.buffer), commData.dataSize);
                    // PRINT("injected by opc");
                    // PRINT(receivedString);
                    InjectCommObject(commData, this);
                }
            };
        }

        void Xinit(opcua::Server* newServer) 
        { 
            server = newServer;
            // uaRunning = false;
        }

        void RegisterDevice(uint16_t deviceIndex)
        {
            // PRINT("Registring device");
            std::map<uint32_t, bool> existingVariables =  FetchNodeMap(deviceIndex, false);
            auto sub = server->createSubscription();
            for (uint32_t objectIndex = 0; objectIndex < GetListSize(deviceIndex); objectIndex++) 
            {
                uint32_t id = GetID(deviceIndex, objectIndex, false);
                opcua::Node node = server->getNode({1, id});
                // PRINT(GetObjectName(deviceIndex, objectIndex));
                if(!node.exists())
                {
                    // PRINT("new");
                    CreateVariable(deviceIndex, objectIndex);
                }
                else
                {
                    // PRINT("here");
                    if(existingVariables.find(id) != existingVariables.end())
                    {
                        existingVariables[id] = true; // is updated
                    }
                    // PRINT("there");
                    if(NodeNeedsUpdate(deviceIndex, objectIndex))
                    {
                        // PRINT("updated");
                        node.deleteNode(true);
                        CreateVariable(deviceIndex, objectIndex);
                    }
                    else
                    {
                        ;
                        // PRINT("do nothing");
                    }                    
                }
            }
            // Remove obsolete variables
            for (const auto& [id, isUpdated] : existingVariables) {
                if (!isUpdated) {
                    opcua::Node node = server->getNode({1, id});
                    // PRINT("removing old node");
                    // PRINT(std::string(node.readBrowseName().getName()));
                    node.deleteNode(true);
                }
            }

            DeleteEmptyFolders(deviceIndex);
        }
   
        // move the variable to another parent node, this includes changing to and or creating a new folder
        // void MoveVariable(int index, String location)


        void Write(CommunicationData data) override 
        {
            uint32_t id = (data.deviceIndex & 0x7FFF) << 16 | (data.objectIndex + 1);
            opcua::Node node = server->getNode({1, id});
            if(node.exists())
            {
                // PRINT("writing opc variable");
                const String name = GetObjectName(data.deviceIndex, data.objectIndex);
                // PRINT("object name");
                // PRINT(name);
                // const std::string path = GetObjectPath(data.deviceIndex, data.objectIndex);
                // PRINT("object path");
                // PRINT(path);
                // const std::type_info* type = GetObjectType(data.deviceIndex,data.objectIndex);
                // PRINT("type");
                // PRINT(TypeRegistry::Demangle(type->name()));
                // PRINT(data.deviceIndex);
                // const std::type_info* type = commList.get(data.commIndex)->getType();
                const std::type_info* type = GetObjectType(data.deviceIndex, data.objectIndex);
                writingToNode = true;
                if(&typeid(int) == type)
                {
                    int typedValue = *reinterpret_cast<int*>(data.buffer);
                    node.writeValueScalar(typedValue);
                }
                else if (&typeid(float) == type)
                {
                    float typedValue = *reinterpret_cast<float*>(data.buffer);
                    node.writeValueScalar(typedValue);
                }
                else if (&typeid(bool) == type)
                {
                    bool typedValue = *reinterpret_cast<bool*>(data.buffer);
                    node.writeValueScalar(typedValue);
                }
                else if (&typeid(int32_t) == type)
                {
                    int32_t typedValue = *reinterpret_cast<int32_t*>(data.buffer);
                    node.writeValueScalar(typedValue);
                }
                else if (&typeid(uint8_t) == type)
                {
                    uint8_t typedValue = *reinterpret_cast<uint8_t*>(data.buffer);
                    node.writeValueScalar(typedValue);
                } 
                else if (&typeid(uint16_t) == type)
                {
                    uint16_t typedValue = *reinterpret_cast<uint16_t*>(data.buffer);
                    node.writeValueScalar(typedValue);
                }
                else if (&typeid(std::string) == type)
                {
                    std::string typedValue = std::string(reinterpret_cast<char*>(data.buffer), data.dataSize);
                    // PRINT("writing string");
                    PRINT(typedValue);
                    node.writeValueScalar(typedValue);
                }
                else
                {
                    // PRINT("i am a structure");
                    std::vector<uint8_t> defaultBytes(data.dataSize, *data.buffer);
                    opcua::ByteString byteString(defaultBytes);  // Create ByteString from vector
                    node.writeValueScalar(byteString);
                }
                writingToNode = false;
                // PRINT("after write");
            }
        }

        void Read()
        {
            // cyclic called loop
            server->runIterate();                    
        }

        void DeregisterDevice(uint16_t deviceIndex) {
            PRINT("Deregistering device");

            // Remove all variables
            uint32_t objectIndex = 0;
            while (true) {
                uint32_t id = GetID(deviceIndex, objectIndex, false);
                opcua::Node node = server->getNode({1, id});
                if (!node.exists()) {
                    break;
                }
                node.deleteNode(true); // true to delete references
                objectIndex++;
            }

            // Remove all folders
            uint16_t folderIndex = 0;
            while (true) {
                uint32_t id = GetID(deviceIndex, folderIndex, true);
                opcua::Node node = server->getNode({1, id});
                if (!node.exists()) {
                    break;
                }
                node.deleteNode(true); // true to delete references
                folderIndex++;
            }
        }

    private:
        opcua::Server* server;
        bool writingToNode;
        // bool uaRunning;
        // Timer reconnectTimer;
        // bool reconnecting;
        ResourceIndexManager folderIndexManager;
        // Helper function to split a string into a vector based on a delimiter
        std::vector<std::string> SplitPath(const std::string& path, char delimiter = '\\') 
        {
            std::vector<std::string> tokens;
            std::istringstream tokenStream(path);
            std::string token;
            while (std::getline(tokenStream, token, delimiter)) {
                tokens.push_back(token);
            }
            return tokens;
        }
        std::string GetNodePath(uint32_t id)
        {
            std::vector<std::string> pathComponents;
            opcua::Node node = server->getNode({1, id});
            uint32_t currentNodeId = node.browseParent().getNodeId().getIdentifierAs<uint32_t>();
            uint16_t namespaceIndex = node.browseParent().getNodeId().getNamespaceIndex();
            // opcua::Node currentNode = server->getNode({1, currentNodeId})
            // // Traverse up the node hierarchy to build the path components
            while (true) {
                if(!pathComponents.empty())
                {
                    PRINT(pathComponents.back());
                }
                PRINT(currentNodeId);
                PRINT(namespaceIndex);
                opcua::Node currentNode = server->getNode({namespaceIndex, currentNodeId});
                if (currentNode == server->getObjectsNode())
                {
                    break;
                }
                auto browseName = currentNode.readBrowseName();
                pathComponents.push_back(std::string(browseName.getName()));
                currentNodeId = currentNode.browseParent().getNodeId().getIdentifierAs<uint32_t>();
                namespaceIndex = currentNode.browseParent().getNodeId().getNamespaceIndex();
            }

            // Reverse the path components to get the correct order
            std::reverse(pathComponents.begin(), pathComponents.end());

            // Join the components into a single path string
            std::string path;
            for (const auto& component : pathComponents) {
                if (!path.empty()) {
                    path += '\\';
                }
                path += component;
            }

            return path;
        }
        // Function to recursively find or create folders based on path
        opcua::Node<opcua::Server> GetOrCreateFolder(uint16_t deviceIndex, opcua::Node<opcua::Server>& parentNode, const std::vector<std::string>& pathComponents, size_t index = 0) 
        {
            if (index >= pathComponents.size()) 
            {
                return parentNode;
            }
            std::string currentComponent = pathComponents[index];

            try 
            {
                opcua::Node<opcua::Server> childNode = parentNode.browseChild({{1, currentComponent}}); // Try to find the existing child node
                return GetOrCreateFolder(deviceIndex, childNode, pathComponents, index + 1);
            } 
            catch (...) 
            {
                try
                {
                    uint16_t folderIndex = folderIndexManager.leaseIndex(deviceIndex);
                    uint32_t id = GetID(deviceIndex, folderIndex, true);
                    opcua::Node<opcua::Server> childNode = parentNode.addFolder(opcua::NodeId(1, id), currentComponent); // If not found, create a new folder
                    return GetOrCreateFolder(deviceIndex, childNode, pathComponents, index + 1);   
                }
                catch(...)
                {
                    throw std::runtime_error("Failed to create or find the folder: " + currentComponent);
                }
            }
        }

        std::map<uint32_t, bool> FetchNodeMap(uint16_t deviceIndex, bool getFolders) {
            std::map<uint32_t, bool> nodeMap;
            uint16_t index = 0;

            while (true) {
                uint32_t id = GetID(deviceIndex, index, getFolders);
                opcua::Node<opcua::Server> node = server->getNode({1, id});
                if (!node.exists()) {
                    break;
                }

                nodeMap[id] = false;
                index++;
            }

            return nodeMap;
        }

        void DeleteEmptyFolders(uint16_t deviceIndex) {
            bool folderRemoved;
            // PRINT("removing old folders");
            do {
                folderRemoved = false;
                std::vector<uint16_t> leasedIndices = folderIndexManager.getLeasedIndices(deviceIndex);
                for (uint16_t index : leasedIndices) {
                    uint32_t id = GetID(deviceIndex, index, true);
                    opcua::Node<opcua::Server> node = server->getNode({1, id});
                    if (node.browseChildren().empty()) {
                        PRINT("folder removed");
                        PRINT(std::string(node.readBrowseName().getName()));
                        node.deleteNode();
                        folderIndexManager.releaseIndex(deviceIndex, index);
                        folderRemoved = true;
                        break;  // Restart the loop after removing a folder
                    }
                }
            } while (folderRemoved);
        }

        void CreateVariable(uint16_t deviceIndex, uint16_t objectIndex)
        {
            // PRINT("creating opc variable");
            const String name = GetObjectName(deviceIndex, objectIndex);
            const std::string path = GetObjectPath(deviceIndex, objectIndex);
            const std::type_info* type = GetObjectType(deviceIndex,objectIndex);
            const size_t dataSize = GetObjectDataSize(deviceIndex,objectIndex);
            std::vector<std::string> pathComponents = SplitPath(path);
            uint32_t id = GetID(deviceIndex, objectIndex, false);
            // Add a variable node to the Objects node
            opcua::Node parentNode = server->getObjectsNode();
            // PRINT("object name");
            // PRINT(name);
            // PRINT("object path");
            // PRINT(path);
            // PRINT("type");
            // PRINT(TypeRegistry::Demangle(type->name()));
            // PRINT("finding path");
            opcua::Node<opcua::Server> targetNode = GetOrCreateFolder(deviceIndex, parentNode, pathComponents);
            
            auto it = typeToUADType.find(type);
            opcua::DataTypeId dataType = (it != typeToUADType.end()) ? it->second : opcua::DataTypeId::ByteString;
    
            opcua::Node newNode = targetNode.addVariable(opcua::NodeId{1, id}, name, opcua::VariableAttributes{}
            .setAccessLevel(UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE)
            .setValueRank(opcua::ValueRank::Scalar)
            .setDataType(dataType)
            );
            opcua::ValueCallback valueCallback;
            if(type != &typeid(std::string))
            {
                valueCallback.onAfterWrite = createCustomCallback(deviceIndex, objectIndex, dataSize);
            }
            else
            {
                valueCallback.onAfterWrite = createCustomCallbackForString(deviceIndex, objectIndex);
            }
            // set value callback as receive notification
            server->setVariableNodeValueCallback(newNode.getNodeId(), valueCallback);
            // PRINT("variable added");
            Write(GetObjectData(deviceIndex,objectIndex));
        }

        bool NodeNeedsUpdate(uint16_t deviceIndex, uint16_t objectIndex)
        {
            uint32_t id = GetID(deviceIndex, objectIndex, false);
            opcua::Node node = server->getNode({1, id});
            std::string nodePath = GetNodePath(id);
            std::string browseName = std::string(node.readBrowseName().getName());
            opcua::DataTypeId opcTypId = opcua::DataTypeId(node.readDataType().getIdentifierAs<uint32_t>());

            const std::string name = GetObjectName(deviceIndex, objectIndex);
            const std::string path = GetObjectPath(deviceIndex, objectIndex);
            const std::type_info* typeInfo = GetObjectType(deviceIndex,objectIndex);
            bool nodeNeedsUpdate = browseName != name || nodePath != path || opcTypId != typeToUADType[typeInfo];
            if(nodeNeedsUpdate)
            {
                PRINT("needs update");
                PRINT(nodePath);
                PRINT(browseName);
                PRINT(path);
                PRINT(name);
            }
            return nodeNeedsUpdate;
        }
        uint32_t GetID(uint16_t deviceIndex, uint16_t objectIndex, bool isFolder)
        {
            return (isFolder << 31) | (deviceIndex & 0x7FFF) << 16 | (objectIndex + 1);
        }
        uint16_t GetObjectIndexFromId(uint32_t id)
        {
            return (uint16_t)((id >> 16) & 0x7FFF);
        }
};

#endif //UASUBSCRIBER