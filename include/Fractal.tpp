
//

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
}

template <typename ElemType, int Arity> 
void Fractal<ElemType,Arity>::Element::Element(Fractal_t* fractal, ElementInfo info, ElemType* parent) 
    : fractal(fractal), info(info), parent(parent)
{
    if ((info.level > 0) && !this->stop_condition()) {
        for (int i = 0; i < ChildrenNum; i++) {

            ElementInfo child_info;
            child_info.level = info.level-1;
            child_info.depth = info.depth+1;
            child_info.children_num = info.children_num;

            children.push_back(
                    std::unique_ptr<ElemType>( new ElemType(fractal, child_info, this) )
            );
        }
    }
}

// end
