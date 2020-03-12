
template <typename ElemType, int Arity> 
void Fractal::grow(int depth) {

    this->top_level = depth;
    this->depth = depth;
   
    // root element to be created
    ElementInfo info;
    info.level = this->top_level;
    info.depth = 0;
    info.children_num = this->children_num;

    if (this->depth >= 0) {
        root = new ElemType(this, // fractal the element belongs to
                            info, // current fractal element info
                            nullptr // parent fractal element (root does not have a parent pointer)
                            );
    }



    this->depth = depth;

    if (this->depth >= 0) {
        root = new ElemType(depth);
    }
}

template <typename ElemType, int Arity> 
void FractalElement::FractalElement(int depth) {


}

// [*] Fractal is a hierarchical data structure / computational pattern /
// algorithmic skeleton / which possesses the following informal properties:
// 
// * self-similarity
// * unfolding symmetry
// * replication
// * parallelism
//

template <typename ElemType, int Arity> 
class Fractal {

    public:
        
        Fractal() {} 

        const int arity = Arity;


        virtual void grow(int depth) = 0;
        
        virtual ~Fractal();

    private:

        FractalElement<ElemType,Arity>* root;
};

class FractalElement {
    
    public:
        
        FractalElement() 
            : parent(nullptr)
        
        

    private:


        // hierarchy 

        FractalElement* parent;
        std::vector<FractalElement*> children;
};


class FractalElementParameters {


};

class FractalElementInfo {



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
