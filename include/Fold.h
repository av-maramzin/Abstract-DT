#ifndef ABSTRACT_FOLD_H
#define ABSTRACT_FOLD_H

namespace abstract {


template <typename ElemType>
class Fold {

    public:
        
        class Element {
            


        };

        Fold();
        
        template <typename SeedType>
        void grow(int depth);

        void compute();

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





Fold<struct Lateral>::Element


#endif // #ifndef ABSTRACT_FRACTAL_H
