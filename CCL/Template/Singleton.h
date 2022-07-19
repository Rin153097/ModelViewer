#pragma once

template <class T>
class Singleton {
protected:
    static T* _instance;
    Singleton() {}
    T operator=(T);
public:
    static T* Instance() {
        if (!_instance) _instance = new T();
        return _instance;
    }
};