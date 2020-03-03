#ifndef FRACTAL_H
#define FRACTAL_H

#include <cmath>

#include <iostream>
#include <omp.h>

#include "Sequence.h"

namespace abstract {

struct FractalElementInfo {

    FractalElementInfo()
        : depth(-1), level(-1), children_num(-1) {}

    void check() const {
        if (level == 0) {
            std::cerr << "FractalElementInfo::check(): error: cannot operate at the 0 level";
            std::exit(EXIT_FAILURE);
        }
    }

    int depth;
    int level;
    int children_num;
};

template <typename ElemType, int ChildrenNum>
class FractalElement {

    public:
        
        using Fractal_t = Fractal<ElemType,ChildrenNum>;

        FractalElement(Fractal_t* fractal, FractalElementInfo info, ElemType* parent) 
            : fractal(fractal), info(info), parent(parent) 
        {
        
            if ((info.level > 0) && !this->stop_condition()) {
                for (int i = 0; i < ChildrenNum; i++) {
    
                    FractalElementInfo child_info;
                    child_info.level = info.level-1;
                    child_info.depth = info.depth+1;
                    child_info.children_num = info.children_num;

                    children.push_back(new ElemType(fractal, child_info, this)); 
                }
            }

            this->grow();
        }

        virtual ~FractalElement();

        // the actual fractal element type must overload this method with 
        // element specific growth functionality
        virtual void grow() = 0;

        virtual bool stop_condition() { return false; }

        template <typename ApplyFunc,typename ReturnType>
        ReturnType apply(ApplyFunc apply_func);

    private:

        // Fractal the element belongs to
        Fractal_t* fractal;

        // structural links with parent and children fractal elements
        ElemType* parent;
        std::vector<ElemType*> children;

        // element information
        FractalElementInfo info;
};

template <typename ElemType, int ChildrenNum>
class Fractal {

    public:
        
        using Fractal_t = Fractal<ElemType,ChildrenNum>;

        Fractal() 
            : root(nullptr) 
        {
            children_num = ChildrenNum;
        }
        
        ~Fractal() {
            if (root != nullptr) delete root;
        }

        void grow(int depth);

        template <typename ApplyFunc,typename ReturnType>
        ReturnType apply(ApplyFunc apply_func);

    private:

        // Fractal's root element
        FractalElement<ElemType,ChildrenNum>* root;

        // Fractal's structural information
        int children_num;
        int top_level;
        int depth;
};

#include "Fractal.tpp"

} // namespace abstract

#endif // #ifndef FRACTAL_H
