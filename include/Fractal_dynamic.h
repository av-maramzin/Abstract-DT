#ifndef ABSTRACT_FRACTAL_DYNAMIC_H
#define ABSTRACT_FRACTAL_DYNAMIC_H

#include <vector>
#include <memory>
#include <iostream>
#include <omp.h>

namespace abstract {

template <typename ElemType, typename SeedType, int Arity> 
class Fractal { 

    public:
        
        using Fractal_t = Fractal<ElemType,SeedType,Arity>;
        using Element_t = ElemType;
        using Seed_t = SeedType;
        
        const int arity = Arity;

        // OPTIMIZATION HINT
        // 
        // balanced fractal forms a complete tree and
        // can be laid out linearly in memory as a heap
        // data structure
        //
        // unbalanced fractal on the other hand has to be
        // built as a tree allocated on the heap dynamically
        //
        enum class Type {
            balanced = 0,
            unbalanced
        };

        enum class ImplType {
            sequential = 0,
            parallel
        };

        // ElementInfo class 
        //
        // builds the location information of an element 
        // withing the fractal into the element
        //
        struct ElementInfo;

        // Element class 
        //
        // Users of the Fractal are supposed to derive their 
        // element types from the base element class below and
        // override its customization API methods
        //
        class Element;

        // ComputeFunction class 
        //
        // If a user wants to apply a function to the Fractal, that
        // function is supposed to be derived from the base class
        // below, which frames the API of the application function 
        //
        template <typename ComputeType>
        class ComputeFunction;

        Fractal(); 
        ~Fractal() {}

        void set_type(Type t) { type = t; }
        Type get_type() const { return type; }

        void set_impl_type(ImplType t) { impl_type = t; }
        ImplType get_impl_type() const { return impl_type; }

        Fractal_t& grow(int depth);
        Fractal_t& grow(int depth, SeedType seed);

        template <typename ComputeType>
        ComputeType compute(ComputeFunction<ComputeType>& func);
       
        // helper functions
        //size_t get_elements_num(int depth);
        //int index_to_depth(int index);0
    
    private:
        
        // private framework computation methods
        // (implement compute() method)
        template <typename ComputeType>
        ComputeType compute_unbalanced(ComputeFunction<ComputeType>& compute_func);
        
        template <typename ComputeType>
        ComputeType compute_balanced(ComputeFunction<ComputeType>& compute_func);

        // private framework construction methods
        // (implement grow() method)
        std::unique_ptr<Element> grow_unbalanced(SeedType seed, ElementInfo info);
        void grow_balanced(SeedType seed, ElementInfo info);

        std::unique_ptr<Element> grow_unbalanced(ElementInfo info);
        void grow_balanced(ElementInfo info);

    private:

        Type type;
        ImplType impl_type;
        
        int depth;
        int top_level;
        
        // unbalanced fractal implementation 
        // a pointer to the root element
        std::unique_ptr<Element> root;
        
        // balanced fractal implementation
        // an array of fractal tree elements
        // laid out linearly in memory
        std::vector<std::unique_ptr<Element>> elements;
};

template <typename ElemType, typename SeedType, int Arity> 
struct Fractal<ElemType,SeedType,Arity>::ElementInfo { 
    
    public:
        
        ElementInfo() 
            : depth(-1), level(-1), index(-1) {} 

        ElementInfo(const ElementInfo& ref) 
            : depth(ref.depth), level(ref.level), index(ref.index) {} 


        void check() const {
            if (level == 0) {
                std::cerr << "FractalElementInfo::check(): error: cannot operate at the 0 level";
                std::exit(EXIT_FAILURE);
            }
        }

        int depth;
        int level;
        int index;
        static const int children_num = Arity;
};

template <typename ElemType, typename SeedType, int Arity> 
class Fractal<ElemType,SeedType,Arity>::Element { 

    friend class Fractal<ElemType,SeedType,Arity>;

    public:

        // customization interface 
        Element(const ElementInfo& info);
        virtual ~Element() {}

        virtual void grow() {}
        virtual void grow(Seed_t seed) {}
        virtual bool growth_stop_condition() { return false; }; // no growth stop condition by default
        virtual Seed_t spawn_child_seed(int child_id) {
            Seed_t ret;
            return ret;
        }

        template <typename ComputeType>
        ComputeType compute(ComputeFunction<ComputeType>& func);

        const ElementInfo& element_info() const { return info; }
   
        Element* get_parent_ptr() { return parent; }
        Element* get_child_ptr(int i) { return children[i].get(); }
        
        bool has_children() { return !children.empty(); }

        // query fractal related information
        Fractal* get_fractal() {
            return fractal; 
        }

        // query the types of the fractal this element belongs to
        Type get_fractal_type() const { 
            return (fractal != nullptr) ? fractal->get_type() : Type::unbalanced; 
        }

        ImplType get_fractal_impl_type() const {
            return (fractal != nullptr) ? fractal->get_impl_type() : ImplType::sequential;
        }
        
        SeedType get_seed() const { return seed; }

    protected:

        void plant_seed(SeedType s) { seed = s; }

    private:

        // specify the fractal this element belongs to
        void set_fractal(Fractal* f) { fractal = f; }
        // link the element with its parent element
        void set_parent_element(Element* p) { parent = p; }

    private:

        // seed used to grow the element
        SeedType seed;
        // structural information
        ElementInfo info;
        // fractal the element belongs to
        Fractal* fractal;
        // structural links 
        Element* parent;
        // unbalanced fractal implementation
        // owns its children objects 
        std::vector<std::unique_ptr<Element>> children;
};

template <typename ElemType, typename SeedType, int Arity> 
template <typename ComputeType>
class Fractal<ElemType,SeedType,Arity>::ComputeFunction { 
    
    public:
        
        using Compute_t = ComputeType;

        virtual Compute_t operator()(ElemType& element, 
                                     const std::vector<Compute_t>& child_rets) = 0;
};

#include "Fractal_dynamic.tpp"

} // namespace abstract

#endif // #ifndef ABSTRACT_FRACTAL_DYNAMIC_H
