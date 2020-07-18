
template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>::Fold()
    : elements(), depth(-1), debug(false) {}

template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>::~Fold() {
    depth = -1;
    elements.clear();
}

template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>::Element::Element(const Fold<ElemType,SeedType,InjectType>::ElementInfo& info) 
    : info(info), seed(), injected_data(), fold(nullptr), parent(nullptr), child(nullptr) {} 

template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>::Element::~Element() {
    fold = nullptr;
    parent = nullptr;
    child = nullptr;
}

template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>& Fold<ElemType,SeedType,InjectType>::grow(int depth) {
    this->depth = depth;
    // reserve required space        
    // elements.resize(depth);
    // fill the fold with new elements
    for (size_t i=0; i<=depth; i++) {
        // set element's structural info
        ElementInfo info;
        info.depth = i;
        info.level = depth-i;
        info.index = i;
        // allocate memory for the element
        std::unique_ptr<Element> elem(new ElemType(info));
        // grow custom element part
        elem->grow();
        // put the element into the fold 
        elements.push_back(std::move(elem));
    }
    // return grown fold
    return (*this);
}

template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>& Fold<ElemType,SeedType,InjectType>::grow(int depth, SeedType seed) {
    this->depth = depth;
    // reserve required space        
    //elements.reserve(depth);
    // fill the fold with new elements
    SeedType next_seed = seed; 
    for (size_t i=0; i<=depth; i++) {
        // set element's structural info
        ElementInfo info;
        info.depth = i;
        info.level = depth-i;
        info.index = i;
        // allocate memory for the element
        std::unique_ptr<Element> elem(new ElemType(info));
        // grow custom element part
        elem->grow(next_seed);
        // spawn child seed for the next element
        next_seed = elem->spawn_child_seed();
        // put the element into the fold 
        elements.push_back(std::move(elem));
    }
    // return grown fold
    return (*this);
}

/*
template <typename ElemType, typename ComputeType, typename SeedType>
void Fold<ElemType,ComputeType,SeedType>::shrink(int new_depth) {

    if ((depth >= new_depth) && (new_depth >= 0)) {
        while (depth > new_depth) {
            elements.pop_back();
            depth--;
        }
    } else {
        std::cerr << "Fold<>::shrink(new_depth): invalid new_depth argument value" << std::endl;
    }
}*/

template <typename ElemType, typename SeedType, typename InjectType>
template <typename ComputeType>
ComputeType Fold<ElemType,SeedType,InjectType>::compute(Fold<ElemType,SeedType,InjectType>::ComputeFunction<ComputeType>& compute_func) {
    ComputeType ret;
    for (int i = depth; i >= 0; i--) {
        ret = compute_func(*static_cast<ElemType*>(elements[i].get()), ret);
    }
    return ret;
}

template <typename ElemType, typename SeedType, typename InjectType>
Fold<ElemType,SeedType,InjectType>& Fold<ElemType,SeedType,InjectType>::inject(const InjectType inject_data) {
    InjectType inj = inject_data;
    for (size_t i=0; i<=depth; i++) {
        inj = elements[i]->inject(inj);
    }
    return (*this);
}

//
