//
// Created by NANAnoo on 2/11/2023.
//

#ifndef SPARROW_SINGLETONBASE_H
#define SPARROW_SINGLETONBASE_H

#include <thread>
#include <mutex>

namespace SPW {
    template <typename T>
    class SingletonBase {
    public:
        static T* getInstance() {
            static std::once_flag flag;
            // basically, a singleton won't be dealloc
            static T *lazyObject = nullptr;
            std::call_once(flag, [](){
                if (lazyObject == nullptr) {
                    lazyObject = new T();
                }
            });
            return lazyObject;
        }
    };
}


#endif //SPARROW_SINGLETONBASE_H
