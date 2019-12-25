
//

template <typename ElemType, typename GrowthFunc>
void Fractal<ElemType,GrowthFunc>::grow(int depth, int label_seed)
{
    this->depth = depth;
    this->label_seed = label_seed;
   
    FractalElementInfo info;

    info.depth = this->depth;
    info.label = this->label_seed;
    info.children_num = this->children_num;

    if (depth >= 0) {
        root = new FractalElement<ElemType,GrowthFunc>(info, // current fractal element info
                                                       depth, // fractal depth
                                                       this, // fractal the element belongs to
                                                       nullptr, // parent fractal element (root does not have a parent pointer)
                                                       growth_func); // function to grow elements
    }
}

template <typename ElemType, typename GrowthFunc>
template <typename ApplyFunc, typename ReturnType>
ReturnType Fractal<ElemType, GrowthFunc>::apply(ApplyFunc apply_func) {
        
    if (root == nullptr) {
        std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal root";
        std::exit(EXIT_FAILURE);
    }
   
    return root->template apply<ApplyFunc,ReturnType>(apply_func);
}

template <typename ElemType, typename GrowthFunc>
FractalElement<ElemType,GrowthFunc>::FractalElement(const FractalElementInfo& elem_info,
                                                    int fractal_depth /* = -1*/,
                                                    Fractal_t* fractal /* = nullptr */,
                                                    FractalElement_t* elem_parent /* = nullptr */,
                                                    GrowthFunc growth_func /* = nullptr */)
    : info(elem_info), fractal(fractal), parent(elem_parent), growth_func(growth_func)
{
    elem = allocate_element();

    growth_func(elem, info);

    if (info.depth < fractal_depth) {
        for (int i = 0; i < info.children_num; i++) {
            
            FractalElementInfo child_info;

            child_info.depth = info.depth+1;
            child_info.label = info.label*info.children_num+i-1;
            child_info.children_num = info.children_num;
            
            children.add(new FractalElement(child_info,
                                            fractal_depth,
                                            fractal,
                                            this,
                                            growth_func)
                        );
        }
    }
}

template <typename ElemType, typename GrowthFunc>
FractalElement<ElemType,GrowthFunc>::~FractalElement() {
    
    delete elem;

    if (!children.empty()) {
        for (int i = 0; i < info.children_num; i++) {
            delete children[i];
        }
    }
}

template <typename ElemType, typename GrowthFunc>
template <typename ApplyFunc, typename ReturnType>
ReturnType FractalElement<ElemType,GrowthFunc>::apply(ApplyFunc apply_func) {
        
    if (elem == nullptr) {
        std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal element";
        std::exit(EXIT_FAILURE);
    }
    
    std::vector<ReturnType> ret_vals;

    if (!children.empty()) {
        for (int i = 0; i < info.children_num; i++) {
            ret_vals.push_back(children[i]->template apply<ApplyFunc,ReturnType>(apply_func));
        }
    }
        
    return apply_func(elem, ret_vals);
}

// end
