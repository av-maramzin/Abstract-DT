#ifndef ABSTRACT_FOLD_H
#define ABSTRACT_FOLD_H

#include <iostream>
#include <vector>
#include <memory>

#include "Computation.h"

namespace abstract {

template <typename ComputeType>
class Reduce : public Computation<ComputeType> {

    public:

        using Compute_t = ComputeType;

        Reduce();
        ~Reduce();
       
        // [*] Global interface to Reduce class
        ComputeType compute();
        
        void grow(int width);
        void shrink(int width);

    private:
        int widthdepth;
        std::vector<std::unique_ptr<Computation<ComputeType>>> elements;
};

template <typename ElemType, typename ComputeType, typename SeedType>
Fold<ElemType,ComputeType,SeedType>::Fold()
    : elements(), depth(-1) {}

template <typename ElemType, typename ComputeType, typename SeedType>
Fold<ElemType,ComputeType,SeedType>::~Fold() {
    depth = -1;
    elements.clear();
}

template <typename ElemType, typename ComputeType, typename SeedType>
void Fold<ElemType,ComputeType,SeedType>::grow(int depth) {
    for (size_t i = depth; i > 0; i--) {
        std::unique_ptr<Element> elem(new ElemType());
        elements.push_back(std::move(elem));
    }
}

template <typename ElemType, typename ComputeType, typename SeedType>
void Fold<ElemType,ComputeType,SeedType>::shrink(int new_depth) {

    if ((depth >= new_depth) && (new_depth >= 0)) {
        while (depth > new_depth) {
            elements.pop_back();
            depth--;
        }
    } else {
        std::cerr << "Fold<>::shrink(new_depth): invalid new_depth argument value" << std::endl;
    }
}

template <typename ElemType, typename ComputeType, typename SeedType>
ComputeType Fold<ElemType,ComputeType,SeedType>::compute() {
    ComputeType ret;
    ret = elements[depth]->compute(ret);
    for (size_t i = depth-1; i >= 0; i--) {
        ret = elements[i]->compute(ret);
    }
}

template <typename ElemType, typename ComputeType, typename SeedType>
void Fold<ElemType,ComputeType,SeedType>::inject(const SeedType seed) {
    SeedType s;
    s = elements[0]->inject(seed);
    for (size_t i = 1; i < depth; i++) {
        s = elements[i]->inject(s);
    }
}

}

#endif // #ifndef ABSTRACT_FOLD_H
