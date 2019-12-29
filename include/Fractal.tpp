
//

template <typename ElemType, typename GrowthFunc>
void Fractal<ElemType,GrowthFunc>::grow(int top_level, int label_seed)
{
    this->top_level = top_level;
    this->depth = top_level-1;
    this->label_seed = label_seed;
   
    FractalElementInfo info;

    info.level = top_level;
    info.depth = 0;
    info.label = this->label_seed;
    info.children_num = this->children_num;

    if (this->depth >= 0) {
        root = new FractalElement<ElemType,GrowthFunc>(info, // current fractal element info
                                                       this->depth, // fractal depth
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
    
    if (info.level-1 > 0) {
        
        if (info.depth == 0) {
            // parallelize children creation
            std::vector<FractalElement_t*> tmp(info.children_num);

            #pragma omp parallel for
            for (int i = 0; i < info.children_num; i++) {
            
                FractalElementInfo child_info;
                
                child_info.level = info.level-1;
                child_info.depth = info.depth+1;
                child_info.label = info.label*info.children_num+i+1;
                child_info.children_num = info.children_num;
                
                tmp[i] = new FractalElement(child_info,
                                                fractal_depth,
                                                fractal,
                                                this,
                                                growth_func);
            }
            
            for (int i = 0; i < info.children_num; i++) {
                children.add(tmp[i]);
            }
        } else {
            // not enough granularity for parallelization
            for (int i = 0; i < info.children_num; i++) {
                
                FractalElementInfo child_info;

                child_info.level = info.level-1;
                child_info.depth = info.depth+1;
                child_info.label = info.label*info.children_num+i+1;
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
    
    growth_func(elem, info);
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
        if (info.depth == 0) {
            // parallelize 
            std::vector<ReturnType> tmp(4);

            #pragma omp parallel for
            for (int i = 0; i < info.children_num; i++) {
                tmp[i] = children[i]->template apply<ApplyFunc,ReturnType>(apply_func);
            }

            for (int i = 0; i < info.children_num; i++) {
                ret_vals.push_back(tmp[i]);
            }
        } else {
            for (int i = 0; i < info.children_num; i++) {
                ret_vals.push_back(children[i]->template apply<ApplyFunc,ReturnType>(apply_func));
            }
        }
    }
        
    return apply_func(elem, ret_vals);
}

// end
