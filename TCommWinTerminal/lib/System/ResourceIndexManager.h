#ifndef RESOURCEINDEXMANAGER_H 
#define RESOURCEINDEXMANAGER_H 
#include <set>
#include <unordered_map>
#include <cstdint>
#include <vector>

class ResourceIndexManager {
public:
    ResourceIndexManager() : nextIndex(0) {}

    uint16_t leaseIndex(uint16_t category) {
        if (!availableIndices.empty()) {
            uint16_t index = *availableIndices.begin();
            availableIndices.erase(availableIndices.begin());
            leasedIndices[category].insert(index);
            return index;
        }
        leasedIndices[category].insert(nextIndex);
        return nextIndex++;
    }

    void releaseIndex(uint16_t category, uint16_t index) {
        leasedIndices[category].erase(index);
        availableIndices.insert(index);
    }

    bool isIndexLeased(uint16_t category, uint16_t index) const {
        auto it = leasedIndices.find(category);
        if (it != leasedIndices.end()) {
            return it->second.find(index) != it->second.end();
        }
        return false;
    }

    std::vector<uint16_t> getLeasedIndices(uint16_t category) const {
        std::vector<uint16_t> indices;
        auto it = leasedIndices.find(category);
        if (it != leasedIndices.end()) {
            indices.assign(it->second.begin(), it->second.end());
        }
        return indices;
    }
private:
    uint16_t nextIndex;
    std::set<uint16_t> availableIndices;
    std::unordered_map<uint16_t, std::set<uint16_t>> leasedIndices;
};
#endif // RESOURCEINDEXMANAGER_H