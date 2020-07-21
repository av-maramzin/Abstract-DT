
//

template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>::Fractal()
    : depth(-1), top_level(-1), root(nullptr), 
      type(Type::unbalanced), impl_type(ImplType::sequential) {}
 
template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>::Element::Element(const ElementInfo& elem_info)
    : info(elem_info), fractal(nullptr), parent(nullptr), children() {} 

template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>& Fractal<ElemType,SeedType,Arity>::grow(int depth) 
{
    this->top_level = depth;
    this->depth = depth;
   
    if (this->depth >= 0) {
        // root element of the fractal tree to be created
        ElementInfo info;
        info.level = this->top_level;
        info.depth = 0;

        if (type == Type::unbalanced) {
            root = grow_unbalanced(info);
        } else if (type == Type::balanced) {
            grow_balanced(info);
        } else {
            std::cerr << "Fractal::grow():error: correct fractal type has not been specified!";
            std::exit(EXIT_FAILURE);
        }
    }
    
    return *this;
}

template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>& Fractal<ElemType,SeedType,Arity>::grow(int depth, SeedType seed) 
{
    this->top_level = depth;
    this->depth = depth;
   
    if (this->depth >= 0) {
        // root element of the fractal tree to be created
        ElementInfo info;
        info.level = this->top_level;
        info.depth = 0;

        if (type == Type::unbalanced) {
            root = grow_unbalanced(seed, info);
        } else if (type == Type::balanced) {
            grow_balanced(seed, info);
        } else {
            std::cerr << "Fractal::grow():error: correct fractal type has not been specified!";
            std::exit(EXIT_FAILURE);
        }
    }
    return *this;
}

template <typename ElemType, typename SeedType, int Arity>
std::unique_ptr<typename Fractal<ElemType,SeedType,Arity>::Element> Fractal<ElemType,SeedType,Arity>::grow_unbalanced(ElementInfo info) 
{
    if (info.level >= 0) {
        
        //
        // CREATE THE ROOT ELEMENT OF THE FRACTAL SUBTREE
        //
        // allocate memory for the root of the fractal subtree 
        std::unique_ptr<Element> root_elem(new ElemType(info));
        // set fractal the element belongs to
        root_elem->set_fractal(this);
        // grow the root element
        root_elem->grow();
        
        // 
        // GROW CHILD SUBTREES
        // 
        
        // 
        if ( (info.level-1 >= 0) && 
             (!root_elem->growth_stop_condition()) ) 
        {
            for (int child_id = 0; child_id < Arity; child_id++) {
                // root element of the subtree to be created
                // element structural info
                ElementInfo child_info;
                child_info.level = info.level-1;
                child_info.depth = info.depth+1;
                
                std::unique_ptr<Element> child_elem = std::move(grow_unbalanced(child_info));
                child_elem->set_parent_element(root_elem.get());

                root_elem->children.push_back(std::move(child_elem));
            }
        }
        
        return root_elem;
    
    } else {
        return std::unique_ptr<Element>(nullptr);
    }
}

template <typename ElemType, typename SeedType, int Arity>
std::unique_ptr<typename Fractal<ElemType,SeedType,Arity>::Element> Fractal<ElemType,SeedType,Arity>::grow_unbalanced(SeedType seed, ElementInfo info) 
{
    if (info.level >= 0) {
        
        //
        // CREATE THE ROOT ELEMENT OF THE FRACTAL SUBTREE
        //
        // allocate memory for the root of the fractal subtree 
        std::unique_ptr<Element> root_elem(new ElemType(info));
        // set fractal the element belongs to
        root_elem->set_fractal(this);
        // grow the root element
        root_elem->grow(seed);
        
        // 
        // GROW CHILD SUBTREES
        // 
        
        // 
        if ( (info.level-1 >= 0) && 
             (!root_elem->growth_stop_condition()) ) 
        {
            for (int child_id = 0; child_id < Arity; child_id++) {
                // root element of the subtree to be created
                // element structural info
                ElementInfo child_info;
                child_info.level = info.level-1;
                child_info.depth = info.depth+1;
                // seed to grow the child element
                SeedType child_seed = root_elem->spawn_child_seed(child_id);
                
                std::unique_ptr<Element> child_elem = std::move(grow_unbalanced(child_seed, child_info));
                child_elem->set_parent_element(root_elem.get());

                root_elem->children.push_back(std::move(child_elem));
            }
        }
        
        return root_elem;
    
    } else {
        return std::unique_ptr<Element>(nullptr);
    }
}

template <typename ElemType, typename SeedType, int Arity>
void Fractal<ElemType,SeedType,Arity>::grow_balanced(ElementInfo info) 
{
    // TODO: implement balanced growth method
    return;
}

template <typename ElemType, typename SeedType, int Arity>
void Fractal<ElemType,SeedType,Arity>::grow_balanced(SeedType seed, ElementInfo info) 
{
    // TODO: implement balanced growth method
    return;
}

template <typename ElemType, typename SeedType, int Arity>
template <typename ComputeType>
ComputeType Fractal<ElemType,SeedType,Arity>::compute(ComputeFunction<ComputeType>& compute_func) {
    if (type == Type::unbalanced) {
        return this->template compute_unbalanced<ComputeType>(compute_func);
    } else if (type == Type::balanced) {
        return this->template compute_balanced<ComputeType>(compute_func);
    } else {
        std::cerr << "Fractal::grow():error: correct fractal type has not been specified!";
        std::exit(EXIT_FAILURE);
    }
}

template <typename ElemType, typename SeedType, int Arity>
template <typename ComputeType>
ComputeType Fractal<ElemType,SeedType,Arity>::compute_unbalanced(ComputeFunction<ComputeType>& compute_func) {
    if (root == nullptr) {
        std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal root";
        std::exit(EXIT_FAILURE);
    }
    return root->template compute<ComputeType>(compute_func);
}

template <typename ElemType, typename SeedType, int Arity>
template <typename ComputeType>
ComputeType Fractal<ElemType,SeedType,Arity>::compute_balanced(ComputeFunction<ComputeType>& compute_func) {
    // TODO: implement balanced computation
    ComputeType ret;
    return ret;
}

template <typename ElemType, typename SeedType, int Arity>
template <typename ComputeType>
ComputeType Fractal<ElemType,SeedType,Arity>::Element::compute(ComputeFunction<ComputeType>& compute_func) {
    
    if (fractal->get_type() == Fractal_t::Type::unbalanced) {
        
        std::vector<ComputeType> ret_vals;
        
        if (!children.empty()) {
            if (fractal->get_impl_type() == Fractal_t::ImplType::parallel) {
                if (info.depth < 1) {
                    // parallelize 
                    std::vector<ComputeType> tmp(info.children_num);
                    int threads_count = (info.children_num <= 4) ? info.children_num : 4;

                    #pragma omp parallel num_threads(threads_count)
                    {
                        #pragma omp for
                        for (int i = 0; i < info.children_num; i++) {

                            //int tid = omp_get_thread_num();
                            //printf("Apply omp_thread=%d\n", tid);

                            tmp[i] = children[i]->template compute<ComputeType>(compute_func);
                        }
                    }

                    for (int i = 0; i < info.children_num; i++) {
                        ret_vals.push_back(tmp[i]);
                    }
                } else {
                    for (int i = 0; i < info.children_num; i++) {
                        ret_vals.push_back(children[i]->template compute<ComputeType>(compute_func));
                    }
                }
            } else {
                for (int i = 0; i < info.children_num; i++) {
                    ret_vals.push_back(children[i]->template compute<ComputeType>(compute_func));
                }
            }
        }
       
        return compute_func(*(static_cast<ElemType*>(this)), ret_vals);

    } else { // balanced fractal computation implementation
        ComputeType ret;
        return ret;
    }
}

// end
