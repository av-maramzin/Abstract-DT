#ifndef ABSTRACT_REDUCE_H
#define ABSTRACT_REDUCE_H

#include <iostream>
#include <vector>
#include <memory>

#include "Computation.h"

namespace abstract {

template <typename ComputeType, typename ElemType>
class Reduce : public Computation<ComputeType>
{

    public:

        using Compute_t = ComputeType;

        Reduce();
        ~Reduce();
       
        ComputeType compute();

        void init(ComputeType);

        void grow(size_t width);
        void shrink(size_t width);

    private:

        int width;
        std::vector<std::unique_ptr<Computation<ComputeType>>> elements;
};

template <typename ComputeType, typename ElemType>
Reduce<ComputeType,ElemType>::Reduce()
    : elements(), width(-1) {}

template <typename ComputeType, typename ElemType>
Reduce<ComputeType,ElemType>::~Reduce() {
    width = -1;
    elements.clear();
}

template <typename ComputeType, typename ElemType>
void Reduce<ComputeType,ElemType>::grow(size_t width) {
    this->width = width;
    for (size_t i = 0; i < width; i++) {
        std::unique_ptr<Computation<ComputeType>> elem(new ElemType());
        elements.push_back(std::move(elem));
    }
}

template <typename ComputeType, typename ElemType>
void Reduce<ComputeType,ElemType>::shrink(size_t new_width) {
    if ((width > new_width) && (new_width >= 0)) {
        while (width > new_width) {
            elements.pop_back();
            width--;
        }
    } else {
        std::cerr << "Fold<>::shrink(new_depth): invalid new_depth argument value" << std::endl;
    }
}

template <typename ComputeType, typename ElemType>
ComputeType Reduce<ComputeType,ElemType>::compute() {
    ComputeType ret;
    for (size_t i = 0; i < width; i++) {
        ret += elements[i]->compute();
    }
}

}

#endif // #ifndef ABSTRACT_REDUCE_H
