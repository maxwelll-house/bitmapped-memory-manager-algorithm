#pragma once

extern MemoryManager externMemManager;

class ComplexObject
{
public:
    ComplexObject(double a, double b) : r(a), c(b) {}
    inline void* operator new(size_t size)
    {
        return externMemManager.allocate(size);
    }
    inline void operator delete(void* object)
    {
        externMemManager.free(object);
      
    }
    inline void* operator new [](size_t size)
    {
        return externMemManager.allocate(size);
    }
    inline void operator delete [](void* object)
    {
        externMemManager.free(object);
    }
private:
    union 
    {
        struct {
            double r; // Real number part
            double c; // ComplexObject number Part
        };
        ComplexObject* next;
    };
};