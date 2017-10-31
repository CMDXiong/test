#ifndef CUTIL
#define CUTIL

template <typename T>
void ContainerDeleter(T& Container)
{
    for (typename T::iterator iter = Container.begin(); iter != Container.end(); iter++)
    {
        delete (*iter);
    }
    Container.clear();
}

#endif // CUTIL

