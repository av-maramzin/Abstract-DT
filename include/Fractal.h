#ifndef ABSTRACT_FRACTAL_H
#define ABSTRACT_FRACTAL_H

namespace abstract {

// [*] Fractal is a hierarchical data structure / computational pattern /
// algorithmic skeleton / which possesses the following informal properties:
// 
// * self-similarity
// * structuredness
// * unfolding symmetry
// * replication
// * parallelism
//

template <typename ElemType, int Arity> 
class Fractal {

    public:
        
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

                Element() 
                    : parent(nullptr) 
                {
                    children.clear();
                }

                Element(Fractal_t* fractal, ElementInfo info, Element* parent) 
                    : fractal(fractal), info(info), parent(parent) 
                {
                    if ((info.level > 0) && !this->stop_condition()) {
                        for (int i = 0; i < Arity; i++) {
        
                            ElementInfo child_info;
                            child_info.level = info.level-1;
                            child_info.depth = info.depth+1;
                            child_info.children_num = info.children_num;

                            children.push_back(new ElemType(fractal, child_info, this)); 
                        }
                    }
                }

                virtual ~Element() {
                    if (!children.empty()) {
                        for (int i = 0; i < Arity; i++) {
                            delete children[i];
                        }
                    }
                }

                // customization interface 
                virtual void grow(int depth) = 0; // growth 
                virtual bool stop_condition() {}; // no growth stop condition by default
                
            protected:

                ElementInfo info;

            private:
                
                // structural information
                Fractal* fractal;
                Element* parent;
                std::vector<Element*> children;
        };

        Fractal() : depth(-1) {} 

        const int arity = Arity;

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
