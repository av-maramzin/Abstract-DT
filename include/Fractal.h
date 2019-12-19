#ifndef FRACTAL_H
#define FRACTAL_H

namespace abstract {

template <typename ElemType, typename GrowthFunc>
class Fractal {

    public:    
        
        using Fractal_t = Fractal<ElemType,GrowthFunc>;

        Fractal(int scaling_factor=-1) 
            : parent(nullptr), elem(nullptr), scaling_factor(scaling_factor), depth(0)
        {
            children.clear();
        }
        
        virtual ~Fractal() {
            
            parent = nullptr;
            
            if (elem != nullptr) {
                delete elem;
            }

            for (auto child_fractal_ptr : children) {
                delete child_fractal_ptr;
            }

            children.clear();
        }

        virtual Fractal_t* grow(unsigned int depth, unsigned int label_seed, Fractal_t* parent_fractal);
        
        template <typename ApplyFunc>
        virtual void apply(ApplyFunc apply_func);

    private:

        ElemType* allocate_element();

        unsigned int scaling_factor;

        std::size_t depth_level;
        label_t label;
        std::size_t children_num;

        ElemType* elem;
        Sequence<Fractal_t*> children;
        Fractal_t* parent;
};

template <typename ElemType, typename GrowthFunc>
Fractal_t* Fractal<ElemType,GrowthFunc>::grow(unsigned int depth, unsigned int label_seed, Fractal_t* parent_fractal) {

    std::size_t children_num = std::pow(2,scaling_factor);
    std::size_t i;

    parent = parent_fractal;

    if (depth == 0) {
        return nullptr;
    } else {
        elem = allocate_element();
        
        GrowthFunc(elem, depth, label_seed);
        
        for (i = 0; i < children_num; i++) {
            Fractal_t* child_fractal = grow(depth-1, label_seed*children_num+i-1, this);
            if (child_fractal != nullptr) {
                children.add(child_fractal);
            }
        }
    }

    return this;
}

template <typename ElemType, typename GrowthFunc>
template <typename ApplyFunc, typename ReturnType>
ReturnType Fractal<ElemType, GrowthFunc>::apply(ApplyFunc apply_func) {
        
    if (elem == nullptr) {
        std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal element";
        std::exit(EXIT_FAILURE);
    }
    
    std::vector<ReturnType> ret_vals;

    if (!children.empty()) {
        constexpr std::size_t children_num = std::pow(2,scaling_factor);
        std::vector<ReturnType> ret_vals;
        for (std::size_t i = 0; i < children_num; i++) {
            ret_vals.push_back(children[i].apply(apply_func));
        }
    }
        
    return apply_func(elem, ret_vals);
}

} // namespace abstract

#endif // #ifndef FRACTAL_H
