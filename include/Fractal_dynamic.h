#ifndef ABSTRACT_FRACTAL_DYNAMIC_H
#define ABSTRACT_FRACTAL_DYNAMIC_H

namespace abstract {

template <typename ElemType, int Arity> 
class Fractal { 

    public:

        class Element {

        };

        template <typename ComputeType>
        class Computation {
        
            public:
                
                ComputeType operator()(Element*) {
                    
                }
        };
        

        compute(Computation<ComputeType> );

        void grow(size_t depth);


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

            public:

                Element();
                Element(Fractal<ElemType,Arity>* fractal, ElementInfo info, Element* parent); 

                virtual ~Element() {
                    if (!children.empty()) {
                        for (int i = 0; i < Arity; i++) {
                            delete children[i];
                        }
                    }
                }

                // customization interface 
                virtual void grow(int depth) = 0; // growth 
                virtual bool stop_condition() { return false; }; // no growth stop condition by default
        
                virtual compute


            protected:

                const ElementInfo& element_info() const { return info; }

            private:
                
                // [*] structural information
                ElementInfo info;
                
                // fractal the element belongs to
                Fractal* fractal;
                // 
                Element* parent;
                std::vector<std::unique_ptr<Element>> children;
        };

        Fractal() {}
        ~Fractal() {} 

        void grow(int depth);
        
    private:

        int depth;
        std::unique_ptr<Element> root;
};

}

#endif // #ifndef ABSTRACT_FRACTAL_DYNAMIC_H
