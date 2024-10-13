#ifndef ENTITY_H
#define ENTITY_H

class Entity 
{
    public:
        Entity(Entity* parent = nullptr, const std::string& name = "")
            : parent(parent), name(name) {}
        virtual ~Entity() 
        {
            ;
        } 
        std::string GetPath() 
        {
            if(parent) 
            {
                return parent->GetPath() + "\\" + name;
            } else 
            {
                return name;
            }
        }

        std::string GetName() 
        {
            return name;
        }
    protected:
        void SetName(std::string name)
        {
            this->name = name;
        }

        void RegisterParent(Entity* parent)
        {
            this->parent = parent;
        }

        std::string name;
        Entity* parent;
};
#endif // ENTITY_H