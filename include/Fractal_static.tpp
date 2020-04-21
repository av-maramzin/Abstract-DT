
//

template <typename ElemType, int ChildrenNum>
void Fractal<ElemType,ChildrenNum>::grow(int depth) 
{
    this->top_level = depth;
    this->depth = depth;
   
    // root element to be created
    FractalElementInfo info;
    info.level = this->top_level;
    info.depth = 0;
    info.children_num = this->children_num;

    if (this->depth >= 0) {
        root = new ElemType(this, // fractal the element belongs to
                            info, // current fractal element info
                            nullptr // parent fractal element (root does not have a parent pointer)
                            };
    }
}

template <typename ElemType, int ChildrenNum>
template <typename ApplyFunc, typename ReturnType>
ReturnType Fractal<ElemType, ChildrenNum>::apply(ApplyFunc apply_func) {
        
    if (root == nullptr) {
        std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal root";
        std::exit(EXIT_FAILURE);
    }
   
    return root->template apply<ApplyFunc,ReturnType>(apply_func);
}

template <typename ElemType, int ChildrenNum>
FractalElement<ElemType,ChildrenNum>::~FractalElement() {
    if (!children.empty()) {
        for (int i = 0; i < info.children_num; i++) {
            delete children[i];
        }
    }
}

template <typename ElemType, int ChildrenNum>
template <typename ApplyFunc, typename ReturnType>
ReturnType FractalElement<ElemType,ChildrenNum>::apply(ApplyFunc apply_func) {
        
    std::vector<ReturnType> ret_vals;

    if (!children.empty()) {
        for (int i = 0; i < info.children_num; i++) {
            ret_vals.push_back(children[i]->template apply<ApplyFunc,ReturnType>(apply_func));
        }
    }
        
    return apply_func(this, ret_vals);
}

// end
