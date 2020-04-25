
//

template <typename ElemType, typename SeedType, int Arity>
void Fractal<ElemType,SeedType,Arity>::grow(SeedType seed, int depth) 
{
    this->top_level = depth;
    this->depth = depth;
   
    if (this->depth >= 0) {

        // root element of the fractal tree to be created
        ElementInfo info;
        info.level = this->top_level;
        info.depth = 0;
        info.children_num = Arity;

        if (type == Type::unbalanced) {
            root = grow_unbalanced(seed, info);
        } else if (type == Type::balanced) {
            grow_balanced(seed, depth);
        } else {
            std::cerr << "Fractal::grow():error: correct fractal type has not been specified!";
            std::exit(EXIT_FAILURE);
        }
    }
}

template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>::grow_unbalanced(SeedType seed, ElementInfo info) 
{
    if (info.level >= 0) {
        
        // allocate memory for the root of the fractal subtree 
        std::unique_ptr<Element> root_elem = std::make_unique<Element>(new ElemType(info));
    
        // grow the root element
        root_elem->grow(seed);

        // grow child subtrees
        if ( (info.level-1 >= 0) && 
             (!root_elem->growth_stop_condition()) ) 
        {
            for (int child_id = 0; child_id < Arity; child_id++) {
                // root element of the subtree to be created
                ElementInfo child_info;
                child_info.level = info.level-1;
                child_info.depth = info.depth+1;
                child_info.children_num = Arity;

                SeedType child_seed = root_elem->spawn_child_seed(child_id);
        
                root_elem->children.push_back(grow_unbalanced(child_seed, child_info);
            }
        }

        return root_elem;
    } else {
        return std::make_unique<Element>(nullptr);
    }
}

template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>::grow_balanced(SeedType seed, int depth) 
{
    // TODO
}

template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>::Element::Element(const ElementInfo& elem_info)
    : info(elem_info) {}

template <typename ApplyFunc, typename ReturnType>
template <typename ElemType, typename SeedType, int Arity>
ReturnType Fractal<ElemType, SeedType, Arity>::apply(ApplyFunc apply_func) {
    
    if (type == Type::unbalanced) {
        
        if (root == nullptr) {
            std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal root";
            std::exit(EXIT_FAILURE);
        }
        
        return root->template apply<ApplyFunc,ReturnType>(apply_func);
    
    } else if (type == Type::balanced) {


    } else {
        std::cerr << "Fractal::grow():error: correct fractal type has not been specified!";
        std::exit(EXIT_FAILURE);
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
