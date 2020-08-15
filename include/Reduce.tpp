
template <typename ElemType, typename SeedType, typename InjectType>
Reduce<ElemType,SeedType,InjectType>::Reduce()
    : elements(), width(-1), impl_type(ImplType::sequential) {}

template <typename ElemType, typename SeedType, typename InjectType>
Reduce<ElemType,SeedType,InjectType>::~Reduce() {
    width = -1;
    elements.clear();
}

template <typename ElemType, typename SeedType, typename InjectType>
Reduce<ElemType,SeedType,InjectType>::Element::Element(const ElementInfo& info)
    : injected_data(), seed(), info(info), reduce(nullptr) {}

template <typename ElemType, typename SeedType, typename InjectType>
Reduce<ElemType,SeedType,InjectType>::Element::~Element() {}

template <typename ElemType, typename SeedType, typename InjectType>
Reduce<ElemType,SeedType,InjectType>& Reduce<ElemType,SeedType,InjectType>::grow(size_t width) {
    
    // the width of the reduction
    this->width = width;
    // the vector container to hold all reduction elements
    //elements.reserve(width);

    if (this->get_impl_type() == ImplType::sequential) {
        for (size_t i=0; i<width; i++) {
            // position information 
            ElementInfo info;
            info.index = i;
            // allocate memory and set the position
            std::unique_ptr<Element> elem(new ElemType(info));
            // grow custom part of the element
            elem->grow();
            // move the grown element into its position in the reduction
            elements.push_back(std::move(elem));
        }
    } else if (this->get_impl_type() == ImplType::parallel) {
        size_t i;
        int max_threads = omp_get_max_threads();
        int threads_count = (width <= max_threads) ? width : max_threads;

        elements.resize(width);

        #pragma omp parallel for private(i) shared(elements) num_threads(threads_count)
        for (i=0; i<width; i++) {
            // position information 
            ElementInfo info;
            info.index = i;
            // allocate memory and set the position
            std::unique_ptr<Element> elem(new ElemType(info));
            // grow custom part of the element
            elem->grow();
            // move the grown element into its position in the reduction
            elements[i] = std::move(elem);
        }
    }

    return *this;
}

template <typename ElemType, typename SeedType, typename InjectType>
Reduce<ElemType,SeedType,InjectType>& Reduce<ElemType,SeedType,InjectType>::grow(size_t width, SeedType seed) {
    
    // the width of the reduction
    this->width = width;
    // the vector container to hold all reduction elements
    //elements.reserve(width);

    if (this->get_impl_type() == ImplType::sequential) {
        for (size_t i=0; i<width; i++) {
            // position information 
            ElementInfo info;
            info.index = i;
            // allocate memory and set the position
            std::unique_ptr<Element> elem(new ElemType(info));
            // grow custom part of the element
            elem->grow(seed);
            // move the grown element into its position in the reduction
            elements.push_back(std::move(elem));
        }
    } else if (this->get_impl_type() == ImplType::parallel) {
        size_t i;
        int max_threads = omp_get_max_threads();
        int threads_count = (width <= max_threads) ? width : max_threads;

        elements.resize(width);

        #pragma omp parallel for private(i) shared(elements,seed) num_threads(threads_count)
        for (i=0; i<width; i++) {
            // position information 
            ElementInfo info;
            info.index = i;
            // allocate memory and set the position
            std::unique_ptr<Element> elem(new ElemType(info));
            // grow custom part of the element
            elem->grow(seed);
            // move the grown element into its position in the reduction
            elements[i] = std::move(elem);
        }
    }

    return *this;
}

template <typename ElemType, typename SeedType, typename InjectType>
Reduce<ElemType,SeedType,InjectType>& Reduce<ElemType,SeedType,InjectType>::inject(InjectType data) {
    // propagate the data to all reduce elements
    if (this->get_impl_type() == ImplType::sequential) {
        for (size_t i=0; i<width; i++) {
            elements[i]->inject(data);
        }
    } else if (this->get_impl_type() == ImplType::parallel) {
        size_t i;
        int max_threads = omp_get_max_threads();
        int threads_count = (width <= max_threads) ? width : max_threads;

        #pragma omp parallel for private(i) shared(elements) num_threads(threads_count)
        for (size_t i=0; i<width; i++) {
            elements[i]->inject(data);
        }
    }
}

/*
template <typename ElemType, typename SeedType, typename InjectType>
Reduce<ElemType,SeedType,InjectType>& Reduce<ComputeType,ElemType>::shrink(size_t new_width) {
    if ((width > new_width) && (new_width >= 0)) {
        while (width > new_width) {
            elements.pop_back();
            width--;
        }
    } else {
        std::cerr << "Fold<>::shrink(new_depth): invalid new_depth argument value" << std::endl;
    }
}*/

template <typename ElemType, typename SeedType, typename InjectType>
template <typename ComputeType>
ComputeType Reduce<ElemType,SeedType,InjectType>::compute(Reduce<ElemType,SeedType,InjectType>::ComputeFunction<ComputeType>& compute_func) {
    // compute reduced values from all 
    // the elements of the reduce framework
    // and store them in indexed vector
    std::vector<ComputeType> rets;
    rets.resize(width);
    // fill the vector with computed values 
    // reduced from all the elements
    if (this->get_impl_type() == ImplType::sequential) {
        for (size_t i=0; i<width; i++) {
            rets[i] = compute_func((ElemType&)*static_cast<ElemType*>(elements[i].get()));
        }
    } else if (this->get_impl_type() == ImplType::parallel) {
        size_t i;
        int max_threads = omp_get_max_threads();
        int threads_count = (width <= max_threads) ? width : max_threads;

        #pragma omp parallel for private(i) shared(rets,elements) num_threads(threads_count)
        for (i=0; i<width; i++) {
            rets[i] = compute_func((ElemType&)*static_cast<ElemType*>(elements[i].get()));
        }
    }
    // call a user-defined function for a final reduction
    return compute_func(rets);
}

//
