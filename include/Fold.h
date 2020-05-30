#ifndef ABSTRACT_FOLD_H
#define ABSTRACT_FOLD_H

#include <iostream>
#include <vector>
#include <memory>

namespace abstract {

template <typename ElemType, typename SeedType, typename InjectType>
class Fold {

    public:

        using Element_t = ElemType;
        using Compute_t = ComputeType;
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
        // Users perform computations on the Fold by providing custom
        // compute functions or combining operations. The latter are
        // supposed to be function objects derived from a below base
        // class, which sets the interface to override
        //
        template <typename ComputeType>
        class ComputeFunction;

        Fold();
        ~Fold();
       
        // Global interface to the Fold class
        Fold<ElemType,SeedType,InjectType>& grow(int depth, SeedType seed);
        Fold<ElemType,SeedType,InjectType>& shrink(int depth);
        
        // 
        // inject()
        //
        // injects the data through the whole fold list by planting the 
        // data inside every fold element. The planted data can be further  
        // used inside compute method()
        //
        Fold<ElemType,SeedType,InjectType>& inject(const InjectType data);
       
        //
        // main compute() interface
        //
        // Takes custom compute function object as a parameter
        //
        template<typename ComputeType>
        ComputeType compute(ComputeFunction<ComputeType>& func);

    private:
        int depth;
        std::vector<std::unique_ptr<Element>> elements;
};

template <typename ElemType, typename SeedType, typename InjectType> 
struct Fold<ElemType,SeedType,InjectType>::ElementInfo { 
    
    public:
        
        ElementInfo() 
            : depth(-1), level(-1), index(-1) {} 

        ElementInfo(const ElementInfo& ref) 
            : depth(ref.depth), level(ref.level), index(ref.index) {} 

        void check() const {
            if (level == 0) {
                std::cerr << "Fold::ElementInfo::check(): error: cannot operate at the 0 level";
                std::exit(EXIT_FAILURE);
            }
        }

        int depth;
        int level;
        int index;
};

template <typename ElemType, typename SeedType, typename InjectType>
class Fold::Element {
    public:

        Element() : depth(-1), seed() {}
        virtual ~Element() {}

        InjectType inject(const InjectType) = 0;

        void plant_seed(SeedType s) { seed = s; }
        const SeedType extract_seed() const { return seed; }

    private:
        // data injected into the element 
        InjectType injected_data;
        // seed value the element has been grown from
        SeedType seed;
        // depth of the element withing the fold
        int depth;
};

template <typename ElemType, typename SeedType, typename InjectType> 
class Fold<ElemType,SeedType,InjectType>::Element { 

    friend class Fold<ElemType,SeedType,InjectType>;

    public:

        // customization interface 
        Element(const ElementInfo& info);
        virtual ~Element() {}

        virtual void grow(SeedType seed) = 0;
        virtual bool growth_stop_condition() { return false; }; // no growth stop condition by default
        virtual SeedType spawn_child_seed() {
            SeedType ret;
            return ret;
        }

    protected:

        const ElementInfo& element_info() const { return info; }

    private:

        // specify the fractal this element belongs to
        void set_fold(Fractal* f) { fractal = f; }
        // link the element with its parent element
        void set_parent_element(Element* p) { parent = p; }

    private:

        // the seed that has been used to grow the element
        SeedType seed;

        InjectType injected_data;

        // structural information
        ElementInfo info;
        // fold the element belongs to
        Fold<ElemType,SeedType,InjectType>* fold;
        // structural links to adjacent fold elements 
        Element* parent;
        Element* child;
};

template <typename ElemType, typename SeedType, typename InjectType>
template <typename RetType>
class Fold<ElemType,SeedType,InjectType>::ComputeFunction {
    
    public:
        
        using Compute_t = ComputeType;
        
        // interface compute functions used as a parameter 
        // functions of the Fold::compute() are supposed to override
        virtual RetType operator()(ElemType& element, RetType cumulative) {
            // stub version does not do any changes to the element
            // and just passes along its incoming cumulative value
            return cumulative;
        }
};

template <typename ElemType, typename ComputeType, typename SeedType>
Fold<ElemType,SeedType,InjectType>::Fold()
    : elements(), depth(-1) {}

template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>::~Fold() {
    depth = -1;
    elements.clear();
}

template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>& Fold<ElemType,SeedType,InjectType>::grow(int depth, SeedType seed) {
    
    SeedType next_seed = seed; 
    for (size_t i=0; i<depth; i++) {
        ElementInfo info;
        info.depth = i;
        info.level = depth-i;
        info.index = i;
        
        std::unique_ptr<Element> elem(new ElemType(info));
        elem->grow(next_seed);
        next_seed = elem->spawn_child_seed();
        
        elements.push_back(std::move(elem));
    }

    return (*this);
}

/*
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
}*/

template <typename ElemType, typename SeedType, typename InjectType>
template <typename ComputeType>
ComputeType Fold<ElemType,SeedType,InjectType>::compute(const ComputeFunction<ComputeType>& compute_func) {
    ComputeType ret;
    for (size_t i = depth; i >= 0; i--) {
        ret = compute_func(*elements[i], ret);
    }
    return ret;
}

template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>& Fold<ElemType,SeedType,InjectType>::inject(const InjectType inject_data) {
    InjectType inj = inject_data;
    for (size_t i=0; i<=depth; i++) {
        inj = elements[i]->inject(inj);
    }
    return (*this);
}

}

#endif // #ifndef ABSTRACT_FOLD_H
