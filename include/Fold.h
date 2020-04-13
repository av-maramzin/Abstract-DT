#ifndef ABSTRACT_FOLD_H
#define ABSTRACT_FOLD_H

#include <iostream>
#include <vector>
#include <memory>

namespace abstract {

template <typename ComputeType, typename ElemType, typename SeedType>
class Fold : public Computation<ComputeType> {

    public:

        using Element_t = ElemType;
        using Compute_t = ComputeType;
        using Seed_t = SeedType;

        // [*] Usage:
        // 
        // Users of the Fold class template must specify their own 
        // type of elements to be folded by deriving it from below
        // Element class. The latter serves as the backbone of the 
        // fold, knows its depth and provides an interface function
        // compute() to be overriden by concrete classes of elements
        //
        template <typename ElemType>
        class Element : public Computation<ComputeType> {
            
            public:

                Element() : depth(-1), seed() {}
                virtual ~Element() {}

                virtual ComputeType compute(const ComputeType) = 0;
                virtual SeedType inject(const SeedType) = 0;

            protected:

                void plant_seed(SeedType s) { seed = s; }
                const SeedType extract_seed() const { return seed; }

            private:
                
                SeedType seed; 
                int depth;
        };

        Fold();
        ~Fold();
       
        // [*] Global interface to the Fold class
        void grow(int depth);
        void shrink(int depth);
        void inject(const SeedType seed);
        ComputeType compute();

    private:
        int depth;
        std::vector<std::unique_ptr<Element>> elements;
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
