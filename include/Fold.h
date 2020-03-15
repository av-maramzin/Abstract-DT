#ifndef ABSTRACT_FOLD_H
#define ABSTRACT_FOLD_H

namespace abstract {

template <typename ElemType, typename ComputeType>
class Fold {

    public:
        
        class Element {
            
            virtual ComputeType compute(ComputeType) = 0;

            private:
                int depth;
        };

        Fold();
        
        template <typename SeedType>
        void grow(int depth);
        
        template <typename ComputeType>
        void compute(ComputeType);

    private:

        std::vector<std::unique_ptr<Element>> elements;

};

template <typename ElemType>
template <typename SeedType>
void Fold<ElemType>::grow(int depth) {

    for (size_t i = depth; i > 0; i--) {
        
        std::unique_ptr<Element> elem(new ElemType());
        
        elements.push_back(elem);
    }


}

template <typename ElemType>
void Fold<ElemType>::compute(int depth) {

    ComputeType ret = elements[depth]->compute();
    for (size_t i = depth-1; i > 0; i--) {
        ret = elements[i]->compute(ret);
    }


}

Fold<struct Lateral>::Element


#endif // #ifndef ABSTRACT_FRACTAL_H
