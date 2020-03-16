#ifndef ABSTRACT_FOLD_H
#define ABSTRACT_FOLD_H

namespace abstract {

template <typename ElemType, typename ComputeType, typename SeedType>
class Fold {

    public:
        
        class Element {
            public:
                Element() {}
                ~Element() {}

                virtual SeedType inject(SeedType seed) = 0;
                virtual ComputeType compute(ComputeType) = 0;

            private:
                SeedType 
                int depth;
        };

        Fold();
        ~Fold();
       
        void inject(SeedType seed);
        void grow(int depth);
        ComputeType compute();

    private:
        std::vector<std::unique_ptr<Element>> elements;
};

template <typename ElemType, typename ComputeType, typename SeedType>
void Fold<ElemType,ComputeType,SeedType>::grow(int depth) {
    for (size_t i = depth; i > 0; i--) {
        std::unique_ptr<Element> elem(new ElemType());
        elements.push_back(elem);
    }
}

template <typename ElemType, typename ComputeType, typename SeedType>
void Fold<ElemType,ComputeType,SeedType>::inject(SeedType seed) {
    SeedType ret = elements[0]->inject(seed);
    for (size_t i = 1; i < depth; i++) {
        ret = elements[i]->inject(ret);
    }
}

template <typename ElemType, typename ComputeType, typename SeedType>
void Fold<ElemType,ComputeType,SeedType>::compute(int depth) {
    ComputeType ret;
    ret = elements[depth]->compute(ret);
    for (size_t i = depth-1; i > 0; i--) {
        ret = elements[i]->compute(ret);
    }
}

#endif // #ifndef ABSTRACT_FRACTAL_H
