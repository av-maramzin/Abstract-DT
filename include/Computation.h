#ifndef ABSTRACT_COMPUTATION_H
#define ABSTRACT_COMPUTATION_H

#include <iostream>
#include <vector>
#include <memory>

namespace abstract {

template <typename ComputeType>
class Computation {

    public:

        using Compute_t = ComputeType;

        Computation();
        ~Computation();
       
        virtual ComputeType compute() = 0;
};

}

#endif // #ifndef ABSTRACT_COMPUTATION_H
