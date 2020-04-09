#ifndef ABSTRACT_FRAMEWORK_H
#define ABSTRACT_FRAMEWORK_H

#include <iostream>
#include <vector>
#include <memory>

namespace abstract {

class Framework {

    public:

        Framework();
        ~Framework();
       
        virtual void grow(size_t) = 0;
        virtual void shrink(size_t) = 0;
};

}

#endif // #ifndef ABSTRACT_FRAMEWORK_H
