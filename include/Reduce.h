#ifndef ABSTRACT_REDUCE_H
#define ABSTRACT_REDUCE_H

#include <iostream>
#include <vector>
#include <memory>

namespace abstract {

template <typename ElemType, typename SeedType>
class Reduce
{
    public:

        using Element_t = ElemType;
        using Seed_t = SeedType;

        // ElementInfo class 
        //
        // builds the location information of an element 
        // withing the fold into the element
        //
        struct ElementInfo;

        // Element class
        // 
        // Users of the Fold class template must specify their own 
        // type of elements to be folded by deriving them from below
        // Element class. The latter serves as the backbone of the 
        // fold, knows its depth and provides a customization interface
        //
        class Element;

        // ComputeFunction class
        // 
        // Users perform various kinds of reductions by providing 
        // custom reduce functions. The latter are supposed to be 
        // function objects derived from a below base class, 
        // which sets the interface to override
        //
        template <typename ComputeType>
        class ComputeFunction;

        Reduce();
        ~Reduce();
       
        // global interface to the Reduce class
        Reduce<ElemType,SeedType>& grow(size_t width, SeedType seed);
        Reduce<ElemType,SeedType>& shrink(size_t width);
        
        //
        // main compute() interface
        //
        // Takes custom compute function object as a parameter
        //
        template<typename ComputeType>
        ComputeType compute(ComputeFunction<ComputeType>& func);

    private:

        int width;
        std::vector<std::unique_ptr<Element>> elements;
};

template <typename ElemType, typename SeedType> 
struct Reduce<ElemType,SeedType>::ElementInfo { 
    
    public:
        
        ElementInfo() 
            : index(-1) {} 

        ElementInfo(const ElementInfo& ref) 
            : index(ref.index) {} 

        int index;
};

template <typename ElemType, typename SeedType> 
class Reduce<ElemType,SeedType>::Element { 

    friend class Reduce<ElemType,SeedType>;

    public:

        // customization interface 
        Element(const ElementInfo& info);
        virtual ~Element() {}

        virtual void grow(size_t width, SeedType seed) = 0;

    protected:

        const ElementInfo& element_info() const { return info; }

    private:
        // specify the reduce object this element belongs to
        void set_reduce(Reduce<ElemType,SeedType>* r) { reduce = r; }

    private:
        // the seed that has been used to grow the element
        SeedType seed;
        // structural information
        ElementInfo info;
        // reduce the element belongs to
        Reduce<ElemType,SeedType>* reduce;
};

template <typename ElemType, typename SeedType>
template <typename RetType>
class Reduce<ElemType,SeedType>::ComputeFunction {
    
    public:
        
        // interface compute functions used as a parameter 
        // functions of the Fold::compute() are supposed to override
        virtual RetType operator()(ElemType& element, RetType cumulative) {
            // stub version does not do any changes to the element
            // and just passes along its incoming cumulative value
            return cumulative;
        }
};

template <typename ElemType, typename SeedType>
Reduce<ElemType,SeedType>::Reduce()
    : elements(), width(-1) {}

template <typename ElemType, typename SeedType>
Reduce<ElemType,SeedType>::~Reduce() {
    width = -1;
    elements.clear();
}

template <typename ElemType, typename SeedType>
void Reduce<ElemType,SeedType>::grow(size_t width, SeedType seed) {
    
    // the width of the reduction
    this->width = width;
    
    for (size_t i=0; i<width; i++) {
        // information 
        ElementInfo info;
        info.index = i;
        // allocate memory and set 
        std::unique_ptr<Element> elem(new ElemType(info));
        // grow custom part of the element
        elem->grow(seed);

        elements.push_back(std::move(elem));
    }
}

/*
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
}*/

template <typename ElemType, typename SeedType>
template <typename ComputeType>
ComputeType Reduce<ElemType,SeedType>::compute(ComputeFunction<ComputeType>& compute_func) {
    ComputeType ret;
    for (size_t i=0; i<width; i++) {
        ret += compute_func(*elements[i]);
    }
    return ret;
}

}

#endif // #ifndef ABSTRACT_REDUCE_H
