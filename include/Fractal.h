#ifndef FRACTAL_H
#define FRACTAL_H

#include <cmath>

#include <iostream>

#include "Sequence.h"

namespace abstract {

template <typename ElemType, typename GrowthFunc>
class FractalElement;

class FractalElementInfo {
    
    public:

        FractalElementInfo(int elem_depth=-1, int elem_label=-1, int elem_children_num=-1)
            : depth(elem_depth), label(elem_label), children_num(elem_children_num) {}

        int depth;
        int label;
        int children_num;
};

template <typename ElemType, typename GrowthFunc>
class Fractal {

    public:
        
        using fractal_t = Fractal<ElemType,GrowthFunc>;

        Fractal(GrowthFunc fractal_growth_func, int scaling_factor=-1) 
            : root(nullptr), scaling_factor(scaling_factor), growth_func(fractal_growth_func)
        {
            children_num = std::pow(2,scaling_factor);
        }
        
        ~Fractal() {
            delete root;
        }

        void grow(int depth, int label_seed);
        
        template <typename ApplyFunc,typename ReturnType>
        ReturnType apply(ApplyFunc apply_func);

    private:

        FractalElement<ElemType,GrowthFunc>* root; 

        GrowthFunc growth_func;

        int scaling_factor;
        int children_num;

        int depth;
        int label_seed;
};

template <typename ElemType, typename GrowthFunc>
class FractalElement {

    public:

        using Fractal_t = Fractal<ElemType,GrowthFunc>;
        using FractalElement_t = FractalElement<ElemType,GrowthFunc>;

        FractalElement(const FractalElementInfo& elem_info,
                       int fractal_depth = -1,
                       Fractal_t* fractal = nullptr,
                       FractalElement_t* elem_parent = nullptr,
                       GrowthFunc growth_func = nullptr);

        ~FractalElement();

        template <typename ApplyFunc,typename ReturnType>
        ReturnType apply(ApplyFunc apply_func);

    private:
        
        ElemType* allocate_element() {
            return new ElemType;   
        }

    private:

        // fractal the element belongs to
        Fractal_t* fractal;
        
        // functin to grow the fractal element
        GrowthFunc growth_func;

        // structural links with parent and children fractal elements
        FractalElement_t* parent;
        Sequence<FractalElement_t*> children;

        // element information
        FractalElementInfo info;

        // satellite fractal element data
        ElemType* elem;
};

#include "Fractal.tpp"

} // namespace abstract

#endif // #ifndef FRACTAL_H
