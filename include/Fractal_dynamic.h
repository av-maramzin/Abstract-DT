#ifndef ABSTRACT_FRACTAL_DYNAMIC_H
#define ABSTRACT_FRACTAL_DYNAMIC_H

#include <vector>
#include <memory>
#include <cmath>
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
       
        // HELPER FUNCTIONS

        // sum of i first members of the 
        // geometric progression 
        // [ 0 Arity^0 Arity^1 ... ]
        int geo_sum(int i) {
            return (std::pow(Arity,i)-1)/(Arity-1);
        }

        // the number of elements 
        // at the specified depth
        size_t depth_elements_num(int depth) {
            return std::pow(Arity,depth);    
        }

        size_t level_elements_num(int lvl) {
            return std::pow(Arity,top_level-lvl);    
        }

        // the number of elements 
        // in the whole fractal
        size_t fractal_elements_num() {
            if (!geo_progression.empty()) {
                return geo_progression[top_level];
            } else {
                return (std::pow(Arity,top_level)-1)/(Arity-1);
            }
        }
        
        // index of the first child 
        size_t first_child(int parent_index) {
            return Arity*parent_index+1;
        }

        // index of the last child 
        size_t last_child(int parent_index) {
            return Arity*parent_index+Arity;
        }

        // index of the child 
        size_t child_index(int parent_index, int child_id) {
            return Arity*parent_index+child_id;
        }

        int index_to_depth(int index) {
            for (int i=1; i<geo_progression.size(); i++) {
                if (index < geo_progression[i]) return i-1;
            }
            return -1;
        }
 
        int index_to_level(int index) {
            return top_level-index_to_depth(index); 
        }

        int level_start_index(int lvl) {
            return depth_start_index(top_level-lvl);
        }

        int level_end_index(int lvl) {
            return depth_end_index(top_level-lvl);
        }

        int depth_start_index(int d) {
            return geo_progression[d];
        }
 
        int depth_end_index(int d) {
            return geo_progression[d+1]-1;
        }

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

        // refine the type of the fractal
        // for optimization purposes
        Type type;
        ImplType impl_type;

        // fractal parameters
        // will be set after the grow()
        // method has been called
        int depth;
        int top_level;
        size_t elements_num;
        size_t leaves_num;

        // unbalanced fractal implementation 
        // a pointer to the root element as 
        // the starting point
        std::unique_ptr<Element> root;
        
        // balanced fractal implementation
        // an array of mapped fractal tree elements
        // laid out linearly in memory
        std::vector<std::unique_ptr<Element>> elements;
        
        // sum of i first members of the 
        // geometric progression 
        // [ 0 Arity^0 Arity^1 ... ]
        //
        // s_{n-1} = (Arity^n-1)/(Arity-1)
        //
        std::vector<int> geo_progression;
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
        int child_id; // [0 .. Arity-1]
        int index; // [0 .. n]
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
