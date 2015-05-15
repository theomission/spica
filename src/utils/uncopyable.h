#ifndef _SPICA_UNCOPYABLE_H_
#define _SPICA_UNCOPYABLE_H_

#if defined(_WIN32) || defined(__WIN32__)
#define SPICA_UNCOPYABLE_DLL __declspec(dllexport)
#else
#define SPICA_UNCOPYABLE_DLL
#endif

namespace spica {

    class SPICA_UNCOPYABLE_DLL Uncopyable {
    public:
        Uncopyable() {}

    private:
        Uncopyable(const Uncopyable& inst);
        Uncopyable& operator=(const Uncopyable& inst);
    };

}

#endif  // _SPICA_UNCOPYABLE_H_
