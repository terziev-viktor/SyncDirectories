#include <string>
#include <vector>
using std::vector;
using std::string;

namespace dirtree
{
    enum EntityType
    {
        Unknown, Folder, File
    };

    class Entity
    {
    public:
        Entity()
        {
            this->Type = EntityType::Unknown;
        }

        Entity(EntityType type)
        {
            this->Type = type;
        }

        Entity(string Name, EntityType Type)
        {
            this->Name = Name;
            this->Type = Type;
        }

        string Name;

        EntityType Type;
    };

    class Folder : public Entity
    {
    public:
        Folder():Entity(EntityType::Folder) 
        {}

        Folder(string name):Entity(name, EntityType::Folder)
        {}

        ~Folder()
        {
        for(size_t i = 0; i < this->Contents.size(); ++i)
        {
            if(this->Contents[i])
            {
                delete this->Contents[i];
            }
        }
        }

        vector<Entity*> Contents;
    };

    class File : public Entity
    {
    public:
        File()
            :Entity(EntityType::File)
        {}
        File(const string & name)
            :Entity(name, EntityType::File)
        { }

        string Hash;

        string Date;

        long long SizeInBytes; // in bytes;
    };

    class DirectoryTree
    {
    public:
        Folder Root;
    };

}