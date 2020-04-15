#ifndef ABSTRACT_FRACTAL_H
#define ABSTRACT_FRACTAL_H

namespace abstract {

template <typename ElemType, int Arity> 
class Fractal {

    public:
       
        using Fractal_t = Fractal<ElemType,Arity>;

        static const int arity = Arity;

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

        Fractal();


        void grow(int depth);
        
        ~Fractal() { 
            delete root;   
        }

    private:

        int depth;
        Element* root;
};



template <typename T>
void map(int (*f)(int), int array[], size_t n) {
    for (unsigned i = 0; i < n; i++) {
        array[i] = f(array[i]);
    }
}

template <typename T>
T reduce(T array[], size_t n) {
    T res;

    res = 0;
    for (unsigned i = 0; i < n; i++) {
        res += array[i];
    }

    return res;
}

//reduce
//sort


}

#endif // #ifndef ABSTRACT_FRACTAL_H
