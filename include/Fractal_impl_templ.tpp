
//

template <typename ElemType, int ChildNum>
template <typename GrowthFuncType, typename GrowthSeedType, typename NextGrowthSeedFuncType, typename GrowthStopFuncType>
void Fractal<ElemType,ChildNum>::grow(int depth, 
                                        GrowthFuncType growth_func,
                                        GrowthSeedType growth_func_param, 
                                        NextGrowthSeedFuncType next_growth_seed_func,
                                        GrowthStopFuncType growth_stop_func)
{
    this->top_level = depth;
    this->depth = depth;
   
    // root element to be created
    FractalElementInfo info;
    info.level = this->top_level;
    info.depth = 0;
    info.children_num = this->children_num;

    if (this->depth >= 0) {
        root = new FractalElement<ElemType,ChildNum>(info, // current fractal element info
                                                     this, // fractal the element belongs to
                                                     this->depth, // fractal depth
                                                     nullptr, // parent fractal element (root does not have a parent pointer)
                                                     growth_func, // function to grow elements
                                                     growth_func_param, // data to grow elements
                                                     next_growth_seed_func, // generate next data for grow function
                                                     growth_stop_func); // check growth stop condition
    }
}

template <typename ElemType, int ChildNum>
template <typename ApplyFunc, typename ReturnType>
ReturnType Fractal<ElemType, ChildNum>::apply(ApplyFunc apply_func) {
        
    if (root == nullptr) {
        std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal root";
        std::exit(EXIT_FAILURE);
    }
   
    return root->template apply<ApplyFunc,ReturnType>(apply_func);
}

template <typename ElemType, int ChildNum>
template <typename WalkFunc, typename ReturnType>
ReturnType Fractal<ElemType,ChildNum>::walk(WalkFunc walk_func) {
        
    if (root == nullptr) {
        std::cerr << "Fractal::walk(): error: cannot apply the specified function to the NULL fractal root";
        std::exit(EXIT_FAILURE);
    }
   
    return root->template walk<WalkFunc,ReturnType>(walk_func);
}

template <typename ElemType, int ChildNum>
template <typename GrowthFuncType, typename GrowthSeedType, typename NextGrowthSeedFuncType, typename GrowthStopFuncType>
FractalElement<ElemType,ChildNum>::FractalElement(const FractalElementInfo& elem_info,
                                                  Fractal_t* fractal,
                                                  int fractal_depth,
                                                  FractalElement_t* elem_parent,
                                                  GrowthFuncType growth_func,
                                                  GrowthSeedType growth_func_param,
                                                  NextGrowthSeedFuncType next_growth_seed_func,
                                                  GrowthStopFuncType growth_stop_func)
    : info(elem_info), fractal(fractal), parent(elem_parent)
{
    elem = allocate_element();
    
    if ((info.level > 0) && 
        !(growth_stop_func(info, growth_func_param))) {
        
        if (info.depth == 0) {
            // parallelize children creation
            std::vector<FractalElement_t*> tmp(info.children_num);
            int threads_count = (info.children_num <= 4) ? info.children_num : 4;

            #pragma omp parallel num_threads(threads_count)
            {
                #pragma omp for
                for (int i = 0; i < info.children_num; i++) {
                    
                    int tid = omp_get_thread_num();
                    printf("Build omp_thread=%d\n", tid);

                    FractalElementInfo child_info;
                    child_info.level = info.level-1;
                    child_info.depth = info.depth+1;
                    child_info.children_num = info.children_num;

                    GrowthSeedType child_growth_seed;
                    next_growth_seed_func(growth_func_param, child_growth_seed, i);

                    tmp[i] = new FractalElement<ElemType,ChildNum>(child_info,
                                                                   fractal,
                                                                   fractal_depth,
                                                                   this,
                                                                   growth_func,
                                                                   child_growth_seed,
                                                                   next_growth_seed_func,
                                                                   growth_stop_func);
                }
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
                child_info.children_num = info.children_num;

                GrowthSeedType child_growth_seed;
                next_growth_seed_func(growth_func_param, child_growth_seed, i);
                
                children.add(new FractalElement<ElemType,ChildNum>(child_info,
                                                                   fractal,
                                                                   fractal_depth,
                                                                   this,
                                                                   growth_func,
                                                                   child_growth_seed,
                                                                   next_growth_seed_func,
                                                                   growth_stop_func)
                            );
            }
        }
    }
    
    growth_func(elem, info, growth_func_param);
}

template <typename ElemType, int ChildNum>
FractalElement<ElemType,ChildNum>::~FractalElement() {
    
    delete elem;

    if (!children.empty()) {
        for (int i = 0; i < info.children_num; i++) {
            delete children[i];
        }
    }
}

template <typename ElemType, int ChildNum>
template <typename ApplyFunc, typename ReturnType>
ReturnType FractalElement<ElemType,ChildNum>::apply(ApplyFunc apply_func) {
        
    if (elem == nullptr) {
        std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal element";
        std::exit(EXIT_FAILURE);
    }
    
    std::vector<ReturnType> ret_vals;

    if (!children.empty()) {
        if (info.depth < 1) {
            // parallelize 
            std::vector<ReturnType> tmp(info.children_num);
            int threads_count = (info.children_num <= 4) ? info.children_num : 4;

            #pragma omp parallel num_threads(threads_count)
            {
                #pragma omp for
                for (int i = 0; i < info.children_num; i++) {

                    int tid = omp_get_thread_num();
                    printf("Apply omp_thread=%d\n", tid);

                    tmp[i] = children[i]->template apply<ApplyFunc,ReturnType>(apply_func);
                }
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

template <typename ElemType, int ChildNum>
template <typename WalkFunc, typename ReturnType>
ReturnType FractalElement<ElemType,ChildNum>::walk(WalkFunc walk_func) {
        
    if (elem == nullptr) {
        std::cerr << "Fractal::walk(): error: cannot apply the specified function to the NULL fractal element";
        std::exit(EXIT_FAILURE);
    }
    
    std::vector<ReturnType> ret_vals;

    if (!children.empty()) {
        if (info.depth < 1) {
            // parallelize 
            std::vector<ReturnType> tmp(info.children_num);
            int threads_count = (info.children_num <= 4) ? info.children_num : 4;

            #pragma omp parallel num_threads(threads_count)
            {
                #pragma omp for
                for (int i = 0; i < info.children_num; i++) {

                    int tid = omp_get_thread_num();
                    printf("Walk omp_thread=%d\n", tid);

                    tmp[i] = children[i]->template walk<WalkFunc,ReturnType>(walk_func);
                }
            }

            for (int i = 0; i < info.children_num; i++) {
                ret_vals.push_back(tmp[i]);
            }

        } else {
            for (int i = 0; i < info.children_num; i++) {
                ret_vals.push_back(children[i]->template walk<WalkFunc,ReturnType>(walk_func));
            }
        }
    }

    return walk_func(this, ret_vals);
}

// end
