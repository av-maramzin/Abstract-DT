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
        using Seed_t = SeedType;
        using Inject_t = InjectType;

        // ElementInfo class 
        //
        // builds the location information of an element 
        // withing the fold into the element
        //
        class ElementInfo;

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
        Fold<ElemType,SeedType,InjectType>& grow(int depth);
        Fold<ElemType,SeedType,InjectType>& grow(int depth, SeedType seed);
        //Fold<ElemType,SeedType,InjectType>& shrink(int depth);
        
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
        ComputeType compute(Fold<ElemType,SeedType,InjectType>::ComputeFunction<ComputeType>& func);

        void set_debug(bool flag) { debug = flag; } 
        bool is_debug() { return debug; } 

    private:

        int depth;
        std::vector<std::unique_ptr<Element>> elements;

        bool debug;
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
class Fold<ElemType,SeedType,InjectType>::Element { 

    friend class Fold<ElemType,SeedType,InjectType>;

    public:

        // customization interface 
        Element(const ElementInfo& info);
        virtual ~Element();

        // seedless growth
        virtual void grow() {}
        // seed-spawned growth
        virtual void grow(SeedType seed) {}
        virtual SeedType spawn_child_seed() {
            SeedType ret;
            return ret;
        }
        // growth stop condition 
        virtual bool growth_stop_condition() { return false; }; // no growth stop condition by default
        // injection interface
        virtual InjectType inject(const InjectType data) {
            injected_data = data;   
        }
        
        const ElementInfo& element_info() const { return info; }

        void plant_seed(SeedType s) { seed = s; }
        SeedType get_seed() const { return seed; }

        void plant_injected_data(const InjectType data) { injected_data = data; }
        InjectType get_injected_data() const { return injected_data; }

    private:

        // specify the fold this element belongs to
        void set_fold(Fold<ElemType,SeedType,InjectType>* f) { fold = f; }
        // link the element with its parent element
        void set_parent_element(Element* p) { parent = p; }
        // link the element with its child element
        void set_child_element(Element* c) { child = c; }

    private:

        // the seed that has been used to grow the element
        SeedType seed;
        // the data injected into the element
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
        
        using Compute_t = RetType;
        
        // interface compute functions used as a parameter 
        // functions of the Fold::compute() are supposed to override
        virtual RetType operator()(ElemType& element, RetType cumulative) {
            // stub version does not do any changes to the element
            // and just passes along its incoming cumulative value
            return cumulative;
        }
};

#include "Fold.tpp"

}

#endif // #ifndef ABSTRACT_FOLD_H
