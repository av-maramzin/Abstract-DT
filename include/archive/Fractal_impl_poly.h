#ifndef FRACTAL_POLY_H
#define FRACTAL_POLY_H

class Fractal {

    public:
        
        virtual ~Fractal();
        
        virtual grow() = 0;
        virtual apply() = 0;


};

class Fractal_poly : public Fractal {
    



};




template <typename ElemType, int ChildrenNum>
class FractalElement_poly {

    public:
        FractalElement_poly();

        void grow();
        void apply();

    private:
        
        std::vector<FractalElement_poly<ElemType,ChildrenNum>*> children;

        int level;
        int depth;
        int children_num;
};

template <int ChildrenNum>
class Fractal_poly {

    public:
        
        Fractal();
        ~Fractal();

        grow();

        apply();

    private:
        
        FractalElement<ChildrenNum>* root;
};

#endif // #ifndef FRACTAL_POLY_H
