#ifndef FRACTAL_H
#define FRACTAL_H

#include <cmath>

#include <iostream>
#include <omp.h>

#include "Sequence.h"

namespace abstract {

template <typename ElemType, int ChildNum>
class FractalElement;

class FractalElementInfo {
    
    public:

        FractalElementInfo(int elem_level=-1, int elem_depth=-1, int elem_label=-1, int elem_children_num=-1)
            : level(elem_level), depth(elem_depth), children_num(elem_children_num) {}

        void check() const {
            if (level == 0) {
                std::cerr << "FractalElementInfo::check(): error: cannot operate at the 0 level";
                std::exit(EXIT_FAILURE);
            }
        }

        int level;
        int depth;
        int children_num;
};

template <typename ElemType, int ChildNum>
class Fractal {

    public:
        
        using Fractal_t = Fractal<ElemType,ChildNum>;

        Fractal() 
            : root(nullptr) 
        {
            children_num = ChildNum;
        }
        
        ~Fractal() {
            delete root;
        }

        // Fractal grow method
        template <typename GrowthFuncType, // function to grow and fill fractal elements 
                  typename GrowthSeedType, // data to use for elements growth and filling 
                  typename NextGrowthSeedFuncType, // transformation of data from element to element 
                  typename GrowthStopFuncType> // growth stop condition
        void grow(int depth, 
                  GrowthFuncType growth_func, 
                  GrowthSeedType growth_seed, 
                  NextGrowthSeedFuncType next_growth_seed_func,
                  GrowthStopFuncType growth_stop_func);
               
        template <typename ApplyFunc,typename ReturnType>
        ReturnType apply(ApplyFunc apply_func);

        template <typename WalkFunc,typename ReturnType>
        ReturnType walk(WalkFunc apply_func);

    private:

        // Fractal's root element
        FractalElement<ElemType,ChildNum>* root; 

        // Fractal's structural information
        int children_num;
        int top_level;
        int depth;
};

template <typename ElemType, int ChildNum>
class FractalElement {

    public:

        using Fractal_t = Fractal<ElemType,ChildNum>;
        using FractalElement_t = FractalElement<ElemType,ChildNum>;

        template <typename GrowthFuncType, 
                  typename GrowthSeedType, 
                  typename NextGrowthSeedFuncType, 
                  typename GrowthStopFuncType>
        explicit FractalElement(const FractalElementInfo& elem_info,
                                Fractal_t* fractal,
                                int fractal_depth,
                                FractalElement_t* elem_parent,
                                GrowthFuncType growth_func,
                                GrowthSeedType growth_func_param,
                                NextGrowthSeedFuncType next_growth_seed_func,
                                GrowthStopFuncType growth_stop_func);
        ~FractalElement();

        template <typename ApplyFunc,typename ReturnType>
        ReturnType apply(ApplyFunc apply_func);

        template <typename WalkFunc,typename ReturnType>
        ReturnType walk(WalkFunc walk_func);

        FractalElement_t* get_parent_ptr() { return parent; }
        FractalElement_t* get_child_ptr(int i) { return children[i]; }
        
        ElemType* get_elem_data() { return elem; }

        bool has_children() { return !children.empty(); }

    private:
        
        ElemType* allocate_element() {
            return new ElemType;   
        }

    private:

        // Fractal the element belongs to
        Fractal_t* fractal;
        
        // structural links with parent and children fractal elements
        FractalElement_t* parent;
        Sequence<FractalElement_t*> children;

        // element information
        FractalElementInfo info;

        // satellite fractal element data
        ElemType* elem;
};

#include "Fractal_static.tpp"

} // namespace abstract

#endif // #ifndef FRACTAL_H
