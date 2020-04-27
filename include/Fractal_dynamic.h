#ifndef ABSTRACT_FRACTAL_DYNAMIC_H
#define ABSTRACT_FRACTAL_DYNAMIC_H

namespace abstract {

template <typename ElemType, typename SeedType, int Arity> 
class Fractal { 

    public:

        Element_t = ElemType;
        Seed_t = SeedType;
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

        // ElementInfo class 
        //
        // builds the location information of an element 
        // withing the fractal into the element
        //
        class ElementInfo {
            
            public:
                
                ElementInfo() 
                    : depth(-1), level(-1) {} 

                void check() const {
                    if (level == 0) {
                        std::cerr << "FractalElementInfo::check(): error: cannot operate at the 0 level";
                        std::exit(EXIT_FAILURE);
                    }
                }

                int depth;
                int level;
                static const int children_num = Arity;
        };
       
        class Element {

            friend class Fractal<ElemType,SeedType,Arity>;

            public:

                // customization interface 
                Element(Seed_t seed, ElementInfo info);
                virtual ~Element() {}

                virtual void grow(Seed_t seed);
                virtual bool growth_stop_condition() { return false; }; // no growth stop condition by default
                virtual Seed_t spawn_child_seed(int child_id) {
                    Seed_t ret;
                    return ret;
                }

                template <typename ComputeType, typename ComputeFunc>
                ComputeType compute(ComputeFunc func);

            protected:

                const ElementInfo& element_info() const { return info; }

            private:

                // specify the fractal this element belongs to
                void set_fractal(Fractal* fractal);
                void set_parent_element(Element* parent);

            private:

                // structural information
                ElementInfo info;
                // fractal the element belongs to
                Fractal* fractal;
                // structural links 
                Element* parent;
                std::vector<std::unique_ptr<Element>> children;
        };

        Fractal() 
            : depth(-1), top_level(-1), 
              root(nullptr), type(Type::unbalanced) {}
        
        ~Fractal() {} 

        void set_type(Type t) { type = t; }
        Type get_type() const { return type; }

        void grow(SeedType seed, int depth);

        template <typename ComputeType, typename ComputeFunc>
        ComputeType compute(ComputeFunc func);
       
        // helper functions
        size_t get_elements_num(int depth);
        int index_to_depth(int index);
    
    private:
        
        // private framework computation methods
        // (implement compute() method)
        template <typename ComputeType, typename ComputeFunc>
        ComputeType compute_unbalanced(ComputeFunc compute_func);
        
        template <typename ComputeType, typename ComputeFunc>
        ComputeType compute_balanced(ComputeFunc compute_func);

        // private framework construction methods
        // (implement grow() method)
        std::unique_ptr<Element> grow_unbalanced(SeedType seed, ElementInfo info);
        void grow_balanced(SeedType seed, ElementInfo info);

    private:

        Type type;
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

}

#endif // #ifndef ABSTRACT_FRACTAL_DYNAMIC_H
