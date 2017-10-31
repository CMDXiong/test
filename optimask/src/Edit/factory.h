#ifndef FACTORY
#define FACTORY

#pragma once

#include <map>
#include <vector>
#include <string>

template<class AbstractProduct, class IndentifierType, typename ProductCreator = AbstractProduct* (*)()>
class Factory
{
private:
    Factory() {}
    Factory(Factory& factory);
    Factory& operator=(const Factory& factory);

public:
    bool Register(const IndentifierType& id, ProductCreator creator)
    {
        associations_[id] = creator;
        return true;
    }

    bool UnRegister(const IndentifierType& id)
    {
        return associations_.erase(id) == 1;
    }

    AbstractProduct * CreateObject(const IndentifierType& id)
    {
        typename AssocMap::const_iterator i = associations_.find(id);
        if (i != associations_.end())
        {
            return (i->second)();
        }
        return NULL;
    }

    std::vector</*typename*/ IndentifierType> Keys()
    {
        std::vector<IndentifierType> result;
        typename AssocMap::iterator itr = associations_.begin(); // 为什么要加typename

        for (; itr!= associations_.end(); itr++)
        {
            result.push_back(itr->first);
        }
        return result;
    }

    static Factory* instance()
    {
        static Factory * pFactory = NULL;
        if (!pFactory)
        {
            static Factory factory;
            pFactory = &factory;
        }
        return pFactory;
    }

private:
    typedef std::map<IndentifierType, ProductCreator> AssocMap;
    AssocMap associations_;
};

template <class AbstractProduct,class Product>
class RegisterClassToFactory
{
public:
    static AbstractProduct * Create()
    {
        return new Product;
    }

    RegisterClassToFactory(const std::string& id)
    {
        Factory<AbstractProduct, std::string>::instance()->Register(id, RegisterClassToFactory::Create);
    }
};

#define ClassNameToString(x) #x


#endif // FACTORY

